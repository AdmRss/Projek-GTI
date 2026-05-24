#include "physics.h"
#include "globals.h"
#include "utils.h"
#include <math.h>
#include <GL/glut.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ==============================
// SKID MARK DINAMIS
// ==============================
void addSkidMark(float localZ)
{
    float rad      = carAngle * M_PI / 180.0f;
    float rearX    = 1.25f;
    float worldX   = carX + rearX * cos(rad) + localZ * sin(rad);
    float worldZ   = carZ - rearX * sin(rad) + localZ * cos(rad);

    skidMarks[skidIndex].x        = worldX;
    skidMarks[skidIndex].z        = worldZ;
    skidMarks[skidIndex].angle    = carAngle;
    skidMarks[skidIndex].active   = true;
    skidMarks[skidIndex].lifeTime = 5.0f;

    skidIndex = (skidIndex + 1) % MAX_SKID;
}

void updateSkidMarks()
{
    const float dt = 0.016f;
    for (int i = 0; i < MAX_SKID; i++) {
        if (!skidMarks[i].active) continue;
        skidMarks[i].lifeTime -= dt;
        if (skidMarks[i].lifeTime <= 0.0f)
            skidMarks[i].active = false;
    }

    if (isDrifting) {
        skidFrame++;
        if (skidFrame % 3 == 0) {
            addSkidMark( 0.75f);
            addSkidMark(-0.75f);
        }
    }
}

// ==============================
// CHECKPOINT / LAP SYSTEM
// ==============================
// Layout checkpoint (disesuaikan dengan track baru):
//   CP0 – straight utara  (x≈0, z>6)
//   CP1 – straight timur  (z≈0, x>9.5)
//   CP2 – straight selatan / FINISH LINE  (x≈0, z<-6)
//   CP3 – straight barat  (z≈0, x<-9.5)
int getCheckpointZone()
{
    if (carX > -2.5f && carX < 2.5f &&  carZ >  6.0f && carZ <  15.5f) return 0; // Utara
    if (carZ > -2.5f && carZ < 2.5f &&  carX >  9.5f && carX <  23.5f) return 1; // Timur
    if (carX > -2.5f && carX < 2.5f &&  carZ < -6.0f && carZ > -15.5f) return 2; // Selatan (FINISH)
    if (carZ > -2.5f && carZ < 2.5f &&  carX < -9.5f && carX > -23.5f) return 3; // Barat
    return -1;
}

void updateLapSystem()
{
    // Jangan update lap kalau sudah finish
    if (gameState == STATE_FINISHED) return;

    if (checkpointCooldown > 0) checkpointCooldown--;

    int zone = getCheckpointZone();
    if (zone != nextCheckpoint || checkpointCooldown > 0) return;

    if (nextCheckpoint == 2) {          // FINISH LINE
        if (lapStarted) {
            lapCount++;
            // --- RACE SELESAI (1 lap penuh) ---
            gameState = STATE_FINISHED;
            // Update high-score
            if (bestTime < 0.0f || raceTime < bestTime) {
                bestTime   = raceTime;
                isNewRecord = true;
            } else {
                isNewRecord = false;
            }
            spawnConfetti();
        } else {
            lapStarted = true;
        }
        nextCheckpoint = 3;
    } else if (nextCheckpoint == 3) { nextCheckpoint = 0; }
    else if  (nextCheckpoint == 0) { nextCheckpoint = 1; }
    else if  (nextCheckpoint == 1) { nextCheckpoint = 2; }

    checkpointCooldown = 48;
}

// ==============================
// UPDATE MOBIL
// ==============================
void updateCar()
{
    // Hentikan input saat sudah finish
    if (gameState == STATE_FINISHED) {
        // Gesekan natural hingga berhenti
        carSpeed   *= 0.94f;
        carYawVel  *= 0.88f;
        if (fabs(carSpeed) < 0.001f) carSpeed = 0.0f;

        float headRad  = carAngle * M_PI / 180.0f;
        float nextX2   = carX - cos(headRad) * carSpeed;
        float nextZ2   = carZ + sin(headRad) * carSpeed;
        float nx, nz;
        if (!clampToTrack(nextX2, nextZ2, nx, nz)) {
            carX = nextX2; carZ = nextZ2;
        }

        carAngle += carYawVel;
        wheelRot += carSpeed * 220.0f;
        updateSkidMarks();
        updateConfetti();
        return;
    }

    // Timer race
    if (lapStarted) raceTime += 0.016f;

    // --- KONSTANTA FISIKA ---
    const float maxFwd   = 0.24f * speedMultiplier;
    const float maxRev   = 0.09f * speedMultiplier;
    const float accel    = 0.0056f * speedMultiplier;
    const float brake    = 0.0078f * speedMultiplier;
    const float rollFric = 0.986f;

    // --- INPUT GAS/REM ---
    if (normalKey['w'] || normalKey['W']) {
        carSpeed += accel;
        if (carSpeed >  maxFwd) carSpeed =  maxFwd;
    } else if (normalKey['s'] || normalKey['S']) {
        carSpeed -= brake;
        if (carSpeed < -maxRev) carSpeed = -maxRev;
    } else {
        carSpeed *= rollFric;
        if (fabs(carSpeed) < 0.001f) carSpeed = 0.0f;
    }

    // --- STEER INPUT ---
    float steerInput = 0.0f;
    if (normalKey['a'] || normalKey['A']) steerInput =  1.0f;
    if (normalKey['d'] || normalKey['D']) steerInput = -1.0f;

    bool handbrake  = normalKey[' '];
    float speedRatio = fabs(carSpeed) / maxFwd;
    if (speedRatio > 1.0f) speedRatio = 1.0f;

    // --- DRIFT ---
    bool canDrift = (fabs(carSpeed) > 0.06f * speedMultiplier);
    isDrifting = handbrake && canDrift;

    if (isDrifting) {
        driftFrames++;
        if      (driftFrames > 240) comboMultiplier = 4.0f;
        else if (driftFrames > 160) comboMultiplier = 3.0f;
        else if (driftFrames >  80) comboMultiplier = 2.0f;
        else                        comboMultiplier = 1.0f;
    } else {
        driftFrames     = 0;
        comboMultiplier = 1.0f;
    }

    // --- STEER VISUAL ---
    float targetSteerVis = steerInput * 26.0f;
    steerVisualAngle += (targetSteerVis - steerVisualAngle) * 0.15f;

    // --- YAW ---
    float turnPow = steerInput * (0.28f + speedRatio * 0.46f) * speedMultiplier;
    if (carSpeed < 0.0f) turnPow = -turnPow * 0.65f;

    carYawVel += turnPow;
    carYawVel *= isDrifting ? 0.955f : 0.82f;
    carAngle  += carYawVel;

    // --- SLIP LATERAL ---
    static float lateralSpd = 0.0f;
    if (isDrifting) {
        lateralSpd += carYawVel * speedRatio * 0.022f;
        float maxSlip = 0.088f * speedMultiplier;
        if (lateralSpd >  maxSlip) lateralSpd =  maxSlip;
        if (lateralSpd < -maxSlip) lateralSpd = -maxSlip;
    }
    float grip     = isDrifting ? 0.986f : 0.82f;
    lateralSpd    *= grip;

    float targetSlip = isDrifting ? (lateralSpd * -162.0f) : 0.0f;
    driftSlipAngle  += (targetSlip - driftSlipAngle) * 0.12f;
    if (fabs(driftSlipAngle) < 0.1f) driftSlipAngle = 0.0f;

    // --- HEADING VECTOR ---
    float headRad = carAngle * M_PI / 180.0f;
    float fwdX    = -cos(headRad);
    float fwdZ    =  sin(headRad);
    float sideX   =  sin(headRad);
    float sideZ   =  cos(headRad);

    float nextX = carX + fwdX * carSpeed + sideX * lateralSpd;
    float nextZ = carZ + fwdZ * carSpeed + sideZ * lateralSpd;

    if (isDrifting) {
        carSpeed  *= 0.995f;
        driftScore += (fabs(carSpeed) * 6.0f + fabs(lateralSpd) * 26.0f) * comboMultiplier;
    }

    // ==============================
    // COLLISION – CLAMP KE TRACK
    // ==============================
    float nx = 0.0f, nz = 0.0f;
    bool hitWall = clampToTrack(nextX, nextZ, nx, nz);

    carX = nextX;
    carZ = nextZ;

    if (hitWall) {
        // Normalisasi normal (diagonal sudut)
        float nlen = sqrtf(nx * nx + nz * nz);
        if (nlen > 0.0f) { nx /= nlen; nz /= nlen; }

        // Refleksikan kecepatan forward terhadap normal dinding
        float dot = fwdX * nx + fwdZ * nz;
        if (dot < 0.0f) {  // hanya jika bergerak ke arah dinding
            carSpeed   *= 0.22f;
            carYawVel  *= 0.25f;
            lateralSpd *= -0.45f;
        }
        driftFrames    = 0;
        comboMultiplier = 1.0f;
        isDrifting      = false;
        driftSlipAngle *= 0.30f;
    }

    // --- WHEEL ROT ---
    wheelRot += carSpeed * 222.0f;
    if (wheelRot >  360.0f) wheelRot -= 360.0f;
    if (wheelRot < -360.0f) wheelRot += 360.0f;

    updateSkidMarks();
    updateLapSystem();
    updateConfetti();
}