#ifndef PHYSICS_H
#define PHYSICS_H

// ==============================
// SKID MARK DINAMIS
// ==============================
void addSkidMark(float localZ)
{
    float rad = carAngle * M_PI / 180.0f;
    float rearLocalX = 1.25f;
    float worldX = carX + rearLocalX * cos(rad) + localZ * sin(rad);
    float worldZ = carZ - rearLocalX * sin(rad) + localZ * cos(rad);

    skidMarks[skidIndex].x = worldX;
    skidMarks[skidIndex].z = worldZ;
    skidMarks[skidIndex].angle = carAngle;
    skidMarks[skidIndex].active = true;
    skidMarks[skidIndex].lifeTime = 5.0f; 

    skidIndex++;
    if (skidIndex >= MAX_SKID) skidIndex = 0;
}

void updateSkidMarks()
{
    for (int i = 0; i < MAX_SKID; i++) {
        if (skidMarks[i].active) {
            skidMarks[i].lifeTime -= 0.016f; 
            if (skidMarks[i].lifeTime <= 0.0f) {
                skidMarks[i].active = false;
            }
        }
    }

    if (isDrifting) {
        skidFrame++;
        if (skidFrame % 3 == 0) {
            addSkidMark(0.72f);
            addSkidMark(-0.72f);
        }
    }
}

// ==============================
// CHECKPOINT / LAP SYSTEM
// ==============================
int getCheckpointZone()
{
    if (carX > -2.5f && carX < 2.5f && carZ > 4.8f && carZ < 16.0f) return 0;   // Atas
    if (carZ > -2.5f && carZ < 2.5f && carX > 8.8f && carX < 24.0f) return 1;   // Kanan
    if (carX > -2.5f && carX < 2.5f && carZ < -4.8f && carZ > -16.0f) return 2; // Bawah (OFFICIAL FINISH LINE)
    if (carZ > -2.5f && carZ < 2.5f && carX < -8.8f && carX > -24.0f) return 3; // Kiri
    return -1;
}

void updateLapSystem()
{
    if (checkpointCooldown > 0) checkpointCooldown--;

    int zone = getCheckpointZone();
    if (zone == nextCheckpoint && checkpointCooldown == 0) {
        
        // Checkpoint 2 (Bawah) bertindak sebagai Finish Line utama
        if (nextCheckpoint == 2) { 
            if (lapStarted) lapCount++;
            else lapStarted = true;
            nextCheckpoint = 3; // Setelah garis Finish dilewati, target berikutnya reset/lanjut ke Kiri (3)
        }
        else if (nextCheckpoint == 3) {
            nextCheckpoint = 0; // Setelah Kiri (3), lanjut ke Atas (0)
        }
        else if (nextCheckpoint == 0) {
            nextCheckpoint = 1; // Setelah Atas (0), lanjut ke Kanan (1)
        }
        else if (nextCheckpoint == 1) {
            nextCheckpoint = 2; // Setelah Kanan (1), bersiap kembali ke Finish Line Bawah (2)
        }
        
        checkpointCooldown = 45;
    }
}

// ==============================
// UPDATE MOBIL
// ==============================
void updateCar()
{
    const float maxForwardSpeed = 0.23f * speedMultiplier; 
    const float maxReverseSpeed = -0.08f * speedMultiplier;
    const float acceleration    = 0.0055f * speedMultiplier;
    const float brakePower      = 0.0075f * speedMultiplier;
    const float rollingFriction = 0.985f;

    if (normalKey['w'] || normalKey['W']) {
        carSpeed += acceleration;
        if (carSpeed > maxForwardSpeed) carSpeed = maxForwardSpeed;
    } else if (normalKey['s'] || normalKey['S']) {
        carSpeed -= brakePower;
        if (carSpeed < maxReverseSpeed) carSpeed = maxReverseSpeed;
    } else {
        carSpeed *= rollingFriction;
        if (fabs(carSpeed) < 0.001f) carSpeed = 0.0f;
    }

    float steerInput = 0.0f;
    if (normalKey['a'] || normalKey['A']) steerInput =  1.0f;
    if (normalKey['d'] || normalKey['D']) steerInput = -1.0f;

    bool handbrake = normalKey[' '];
    float speedRatio = fabs(carSpeed) / maxForwardSpeed;
    if (speedRatio > 1.0f) speedRatio = 1.0f;

    bool canDrift = (fabs(carSpeed) > 0.06f * speedMultiplier);
    if (handbrake && canDrift) {
        isDrifting = true;
    } else if (!handbrake) {
        isDrifting = false; 
    }

    if (isDrifting) {
        driftFrames++;
        if (driftFrames > 240) comboMultiplier = 4.0f;
        else if (driftFrames > 160) comboMultiplier = 3.0f;
        else if (driftFrames > 80) comboMultiplier = 2.0f;
        else comboMultiplier = 1.0f;
    } else {
        driftFrames = 0;
        comboMultiplier = 1.0f;
    }

    float targetSteerVisual = steerInput * 25.0f;
    steerVisualAngle += (targetSteerVisual - steerVisualAngle) * 0.15f;

    float turnPower = steerInput * (0.3f + speedRatio * 0.45f) * speedMultiplier;
    if (carSpeed < 0.0f) turnPower = -turnPower * 0.65f;

    carYawVel += turnPower;
    carYawVel *= isDrifting ? 0.95f : 0.82f;
    carAngle += carYawVel;

    static float lateralSpeed = 0.0f;

    if (isDrifting) {
        lateralSpeed += (carYawVel * speedRatio * 0.022f);
        float maxSlip = 0.085f * speedMultiplier;
        if (lateralSpeed >  maxSlip) lateralSpeed =  maxSlip;
        if (lateralSpeed < -maxSlip) lateralSpeed = -maxSlip;
    }

    float grip = isDrifting ? 0.985f : 0.85f; 
    lateralSpeed *= grip;

    float targetSlip = isDrifting ? (lateralSpeed * -160.0f) : 0.0f;
    driftSlipAngle += (targetSlip - driftSlipAngle) * 0.12f;
    if (fabs(driftSlipAngle) < 0.1f) driftSlipAngle = 0.0f;

    float headingRad = carAngle * M_PI / 180.0f; 
    
    float forwardX = -cos(headingRad);
    float forwardZ =  sin(headingRad);
    float sideX = sin(headingRad);
    float sideZ = cos(headingRad);

    float nextX = carX + (forwardX * carSpeed) + (sideX * lateralSpeed);
    float nextZ = carZ + (forwardZ * carSpeed) + (sideZ * lateralSpeed);

    if (isDrifting) {
        carSpeed *= 0.995f; 
        driftScore += (fabs(carSpeed) * 6.0f + fabs(lateralSpeed) * 25.0f) * comboMultiplier;
    }

    bool hitWall = false;

    if (isOnTrack(nextX, nextZ)) {
        carX = nextX;
        carZ = nextZ;
    } else if (isOnTrack(nextX, carZ)) {
        carX = nextX;
        carSpeed *= -0.2f; carYawVel *= 0.2f; lateralSpeed *= -0.5f; 
        hitWall = true;
    } else if (isOnTrack(carX, nextZ)) {
        carZ = nextZ;
        carSpeed *= -0.2f; carYawVel *= 0.2f; lateralSpeed *= -0.5f;
        hitWall = true;
    } else {
        carSpeed *= -0.25f; carYawVel *= 0.2f; lateralSpeed *= -0.5f;
        hitWall = true;
    }

    if (hitWall) {
        driftFrames = 0; comboMultiplier = 1.0f; isDrifting = false;
        driftSlipAngle *= 0.35f;
    }

    wheelRot += carSpeed * 220.0f;
    if (wheelRot > 360.0f) wheelRot -= 360.0f;
    if (wheelRot < -360.0f) wheelRot += 360.0f;

    updateSkidMarks();
    updateLapSystem();
}

#endif