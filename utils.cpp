#include "utils.h"
#include "globals.h"
#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ==============================
// UTIL DASAR
// ==============================
void drawBox(float sx, float sy, float sz)
{
    glPushMatrix();
    glScalef(sx, sy, sz);
    glutSolidCube(1.0);
    glPopMatrix();
}

void setMaterial(float r, float g, float b, float shininess)
{
    GLfloat diffuse[]  = { r, g, b, 1.0f };
    GLfloat ambient[]  = { r * 0.30f, g * 0.30f, b * 0.30f, 1.0f };
    GLfloat specular[] = { 0.35f, 0.35f, 0.35f, 1.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT,   ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  specular);
    glMaterialf (GL_FRONT, GL_SHININESS, shininess);
}

bool isInsideRect(float x, float z,
                  float left, float right,
                  float bottom, float top)
{
    return (x > left && x < right && z > bottom && z < top);
}

// ==============================
// CEK POSISI DI LINTASAN
// ==============================
bool isOnTrack(float x, float z)
{
    bool insideOuter = isInsideRect(x, z,
        OUTER_LEFT  + TRACK_MARGIN,
        OUTER_RIGHT - TRACK_MARGIN,
        OUTER_BOTTOM + TRACK_MARGIN,
        OUTER_TOP   - TRACK_MARGIN);

    bool insideInner = isInsideRect(x, z,
        INNER_LEFT  - TRACK_MARGIN,
        INNER_RIGHT + TRACK_MARGIN,
        INNER_BOTTOM - TRACK_MARGIN,
        INNER_TOP   + TRACK_MARGIN);

    return insideOuter && !insideInner;
}

// ==============================
// CLAMP POSISI KE TRACK (COLLISION FIX)
// ==============================
// Langsung clamp posisi agar tidak keluar track.
// Mengisi nx, nz dengan normal dinding yang tersentuh.
// Return true jika ada collision.
bool clampToTrack(float &x, float &z, float &nx, float &nz)
{
    bool hit = false;
    nx = 0.0f; nz = 0.0f;

    // --- Outer walls ---
    float oL = OUTER_LEFT  + TRACK_MARGIN;
    float oR = OUTER_RIGHT - TRACK_MARGIN;
    float oB = OUTER_BOTTOM + TRACK_MARGIN;
    float oT = OUTER_TOP   - TRACK_MARGIN;

    if (x < oL) { x = oL; nx += 1.0f;  hit = true; }
    if (x > oR) { x = oR; nx -= 1.0f;  hit = true; }
    if (z < oB) { z = oB; nz += 1.0f;  hit = true; }
    if (z > oT) { z = oT; nz -= 1.0f;  hit = true; }

    // --- Inner island walls ---
    float iL = INNER_LEFT  - TRACK_MARGIN;
    float iR = INNER_RIGHT + TRACK_MARGIN;
    float iB = INNER_BOTTOM - TRACK_MARGIN;
    float iT = INNER_TOP   + TRACK_MARGIN;

    // Cek apakah posisi saat ini menembus inner island
    if (x > iL && x < iR && z > iB && z < iT)
    {
        // Hitung penetrasi ke setiap sisi inner island
        float dL = x - iL;   // jarak dari sisi kiri island
        float dR = iR - x;   // jarak dari sisi kanan island
        float dB = z - iB;   // jarak dari sisi bawah island
        float dT = iT - z;   // jarak dari sisi atas island

        // Dorong ke sisi yang paling dekat (penetrasi paling kecil)
        float minD = dL;
        int   side = 0;
        if (dR < minD) { minD = dR; side = 1; }
        if (dB < minD) { minD = dB; side = 2; }
        if (dT < minD) { minD = dT; side = 3; }

        if      (side == 0) { x = iL; nx -= 1.0f; }
        else if (side == 1) { x = iR; nx += 1.0f; }
        else if (side == 2) { z = iB; nz -= 1.0f; }
        else                { z = iT; nz += 1.0f; }
        hit = true;
    }

    return hit;
}

// ==============================
// RESET
// ==============================
void resetSkidMarks()
{
    for (int i = 0; i < MAX_SKID; i++) {
        skidMarks[i].active   = false;
        skidMarks[i].x        = 0.0f;
        skidMarks[i].z        = 0.0f;
        skidMarks[i].angle    = 0.0f;
        skidMarks[i].lifeTime = 0.0f;
    }
    skidIndex = 0;
    skidFrame = 0;
}

void resetScene()
{
    // Reset posisi & fisika
    carX     = 20.0f;
    carZ     = -12.0f;
    carAngle = 0.0f;
    carSpeed = 0.0f;

    wheelRot       = 0.0f;
    smokeTime      = 0.0f;
    steerVisualAngle = 0.0f;
    carYawVel      = 0.0f;
    driftSlipAngle = 0.0f;
    isDrifting     = false;

    // Reset skor & lap
    driftScore      = 0.0f;
    comboMultiplier = 1.0f;
    driftFrames     = 0;

    lapCount          = 0;
    nextCheckpoint    = 0;
    lapStarted        = false;
    checkpointCooldown = 0;

    // Reset race system (termasuk highscore)
    gameState    = STATE_PLAYING;
    raceTime     = 0.0f;
    bestTime     = -1.0f;
    isNewRecord  = false;
    confettiActive = false;

    resetSkidMarks();
}

void restartRace()
{
    // Restart race tanpa menghapus highscore
    carX     = 20.0f;
    carZ     = -12.0f;
    carAngle = 0.0f;
    carSpeed = 0.0f;

    wheelRot       = 0.0f;
    smokeTime      = 0.0f;
    steerVisualAngle = 0.0f;
    carYawVel      = 0.0f;
    driftSlipAngle = 0.0f;
    isDrifting     = false;

    driftScore      = 0.0f;
    comboMultiplier = 1.0f;
    driftFrames     = 0;

    lapCount          = 0;
    nextCheckpoint    = 0;
    lapStarted        = false;
    checkpointCooldown = 0;

    gameState      = STATE_PLAYING;
    raceTime       = 0.0f;
    isNewRecord    = false;
    confettiActive = false;

    resetSkidMarks();
}

// ==============================
// LIGHTING: UPDATE TIAP FRAME
// ==============================
void updateLighting()
{
    if (nightMode)
    {
        // Malam: cahaya bulan dingin, ambient sangat gelap
        GLfloat pos[]  = {  8.0f, 22.0f, -12.0f, 1.0f };
        GLfloat amb[]  = { 0.04f,  0.05f,  0.10f, 1.0f };
        GLfloat dif[]  = { 0.35f,  0.38f,  0.52f, 1.0f };
        GLfloat spec[] = { 0.15f,  0.16f,  0.25f, 1.0f };

        glLightfv(GL_LIGHT0, GL_POSITION, pos);
        glLightfv(GL_LIGHT0, GL_AMBIENT,  amb);
        glLightfv(GL_LIGHT0, GL_DIFFUSE,  dif);
        glLightfv(GL_LIGHT0, GL_SPECULAR, spec);
    }
    else
    {
        // Siang: matahari hangat, posisi dari kiri atas
        GLfloat pos[]  = { -9.0f, 20.0f,  11.0f, 1.0f };
        GLfloat amb[]  = { 0.30f,  0.29f,  0.24f, 1.0f };
        GLfloat dif[]  = { 0.96f,  0.90f,  0.78f, 1.0f };
        GLfloat spec[] = { 0.65f,  0.60f,  0.50f, 1.0f };

        glLightfv(GL_LIGHT0, GL_POSITION, pos);
        glLightfv(GL_LIGHT0, GL_AMBIENT,  amb);
        glLightfv(GL_LIGHT0, GL_DIFFUSE,  dif);
        glLightfv(GL_LIGHT0, GL_SPECULAR, spec);
    }
}

// ==============================
// INISIALISASI OPENGL
// ==============================
void init()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    // State awal lighting diisi oleh updateLighting() di frame pertama
}

// ==============================
// TEKSTUR PROSEDURAL
// ==============================
static GLuint makeTexture(unsigned char* data, int w, int h)
{
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, data);
    return tex;
}

void createTextures()
{
    srand(42); // fixed seed: tekstur selalu sama

    const int W = 256, H = 256;
    unsigned char data[W * H * 3];

    // --- Asphalt (abu gelap dengan kerikil) ---
    for (int i = 0; i < W * H; i++) {
        int v = 48 + rand() % 22;           // basis abu gelap
        if (rand() % 12 == 0) v += 28;      // kerikil terang
        if (rand() % 25 == 0) v -= 18;      // retakan gelap
        if (v < 20) v = 20;
        if (v > 130) v = 130;
        data[i*3+0] = (unsigned char)v;
        data[i*3+1] = (unsigned char)v;
        data[i*3+2] = (unsigned char)(v + 3);
    }
    texAsphalt = makeTexture(data, W, H);

    // --- Grass (hijau bervariasi) ---
    for (int i = 0; i < W * H; i++) {
        int r = 55  + rand() % 30;
        int g = 105 + rand() % 55;
        int b = 40  + rand() % 25;
        data[i*3+0] = (unsigned char)r;
        data[i*3+1] = (unsigned char)g;
        data[i*3+2] = (unsigned char)b;
    }
    texGrass = makeTexture(data, W, H);

    // --- Concrete (beton abu terang untuk dinding) ---
    for (int i = 0; i < W * H; i++) {
        int v = 150 + rand() % 40;
        if (rand() % 20 == 0) v -= 30; // sendi beton
        if (v < 100) v = 100;
        if (v > 210) v = 210;
        data[i*3+0] = (unsigned char)v;
        data[i*3+1] = (unsigned char)v;
        data[i*3+2] = (unsigned char)(v - 5);
    }
    texConcrete = makeTexture(data, W, H);
}

// ==============================
// CONFETTI
// ==============================
void spawnConfetti()
{
    srand(123); // bisa pakai rand() biasa, bukan perlu seed tetap
    for (int i = 0; i < MAX_CONFETTI; i++) {
        Confetti& c = confettiParticles[i];
        c.x        = (float)(rand() % 1000);
        c.y        = 550.0f + (float)(rand() % 250);
        c.vx       = (float)(rand() % 60 - 30) * 0.08f;
        c.vy       = -(1.8f + (float)(rand() % 35) * 0.1f);
        c.r        = (float)(rand() % 100) / 100.0f;
        c.g        = (float)(rand() % 100) / 100.0f;
        c.b        = (float)(rand() % 100) / 100.0f;
        c.size     = 5.0f + (float)(rand() % 8);
        c.rot      = (float)(rand() % 360);
        c.rotSpeed = (float)(rand() % 16 - 8) * 0.5f;
    }
    confettiActive = true;
}

void updateConfetti()
{
    if (!confettiActive) return;
    for (int i = 0; i < MAX_CONFETTI; i++) {
        Confetti& c = confettiParticles[i];
        c.x   += c.vx;
        c.y   += c.vy;
        c.rot += c.rotSpeed;
        c.vx  *= 0.995f; // sedikit hambatan udara

        // Daur ulang saat keluar layar
        if (c.y < -20.0f) {
            c.x  = (float)(rand() % 1000);
            c.y  = 560.0f;
            c.vx = (float)(rand() % 60 - 30) * 0.08f;
            c.vy = -(1.8f + (float)(rand() % 35) * 0.1f);
        }
    }
}