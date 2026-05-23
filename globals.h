#ifndef GLOBALS_H
#define GLOBALS_H

// ==============================
// GLOBAL MOBIL (Hanya Deklarasi)
// ==============================
extern float carX;
extern float carZ;
extern float carAngle;
extern float carSpeed;

extern float wheelRot;
extern float smokeTime;

extern float steerVisualAngle;
extern float carYawVel;
extern float driftSlipAngle;

extern bool specialKey[256];
extern bool normalKey[256];
extern bool isDrifting;

// ==============================
// FITUR GAME (Hanya Deklarasi)
// ==============================
extern int cameraMode;
extern bool nightMode;

extern float driftScore;
extern float comboMultiplier;
extern int driftFrames;

extern int lapCount;
extern int nextCheckpoint;
extern bool lapStarted;
extern int checkpointCooldown;

// UI SLIDER MOVEMENT
extern float speedMultiplier;
extern bool isDraggingSlider;

// ==============================
// UKURAN TRACK (Konstanta aman di Header)
// ==============================
const float OUTER_LEFT   = -24.0f;
const float OUTER_RIGHT  =  24.0f;
const float OUTER_BOTTOM = -16.0f;
const float OUTER_TOP    =  16.0f;

const float INNER_LEFT   = -8.8f;
const float INNER_RIGHT  =  8.8f;
const float INNER_BOTTOM = -4.8f;
const float INNER_TOP    =  4.8f;

const float TRACK_MARGIN = 1.15f;

// ==============================
// SKID MARK (Definisi Struct & Deklarasi Array)
// ==============================
// Struct harus didefinisikan di header agar file lain tahu bentuk datanya
struct SkidMark
{
    float x;
    float z;
    float angle;
    bool active;
    float lifeTime; // Waktu sisa sebelum memudar
};

const int MAX_SKID = 600;

// Variabel array dan index menggunakan extern
extern SkidMark skidMarks[MAX_SKID];
extern int skidIndex;
extern int skidFrame;

#endif