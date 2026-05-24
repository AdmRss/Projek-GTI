#ifndef GLOBALS_H
#define GLOBALS_H

#include <GL/glut.h>

// ==============================
// GLOBAL MOBIL
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
// FITUR GAME
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

// UI SLIDER
extern float speedMultiplier;
extern bool isDraggingSlider;

// ==============================
// RACE / WIN-LOSE SYSTEM
// ==============================
enum GameState { STATE_PLAYING, STATE_FINISHED };
extern GameState gameState;
extern float raceTime;       // detik sejak start
extern float bestTime;       // rekor terbaik (-1 = belum ada)
extern bool  isNewRecord;    // true jika finish terakhir adalah rekor baru

// ==============================
// CONFETTI
// ==============================
struct Confetti {
    float x, y;
    float vx, vy;
    float r, g, b;
    float size;
    float rot, rotSpeed;
};
const int MAX_CONFETTI = 200;
extern Confetti confettiParticles[MAX_CONFETTI];
extern bool confettiActive;

// ==============================
// UKURAN TRACK
// ==============================
const float OUTER_LEFT   = -24.0f;
const float OUTER_RIGHT  =  24.0f;
const float OUTER_BOTTOM = -16.0f;
const float OUTER_TOP    =  16.0f;

const float INNER_LEFT   = -8.8f;
const float INNER_RIGHT  =  8.8f;
const float INNER_BOTTOM = -4.8f;
const float INNER_TOP    =  4.8f;

const float TRACK_MARGIN = 1.20f;

// ==============================
// SKID MARK
// ==============================
struct SkidMark {
    float x, z, angle;
    bool  active;
    float lifeTime;
};
const int MAX_SKID = 600;
extern SkidMark skidMarks[MAX_SKID];
extern int skidIndex;
extern int skidFrame;

// ==============================
// TEXTURE IDs (prosedural)
// ==============================
extern GLuint texAsphalt;
extern GLuint texGrass;
extern GLuint texConcrete;

#endif