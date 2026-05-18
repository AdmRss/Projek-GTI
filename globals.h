#ifndef GLOBALS_H
#define GLOBALS_H

// ==============================
// GLOBAL MOBIL
// ==============================
float carX = -18.0f;
float carZ = 10.0f;
float carAngle = 180.0f;
float carSpeed = 0.0f;

float wheelRot = 0.0f;
float smokeTime = 0.0f;

float steerVisualAngle = 0.0f;
float carYawVel = 0.0f;
float driftSlipAngle = 0.0f;

bool specialKey[256] = { false };
bool normalKey[256] = { false };
bool isDrifting = false;

// ==============================
// FITUR GAME
// ==============================
int cameraMode = 0;
bool nightMode = false;

float driftScore = 0.0f;
float comboMultiplier = 1.0f;
int driftFrames = 0;

int lapCount = 0;
int nextCheckpoint = 0;
bool lapStarted = false;
int checkpointCooldown = 0;

// UI SLIDER MOVEMENT
float speedMultiplier = 1.0f;
bool isDraggingSlider = false;

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

const float TRACK_MARGIN = 1.15f;

// ==============================
// SKID MARK 
// ==============================
struct SkidMark
{
    float x;
    float z;
    float angle;
    bool active;
    float lifeTime; // Waktu sisa sebelum memudar
};

const int MAX_SKID = 600;
SkidMark skidMarks[MAX_SKID];
int skidIndex = 0;
int skidFrame = 0;

#endif