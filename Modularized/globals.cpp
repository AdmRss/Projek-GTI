#include "globals.h"

// ==============================
// GLOBAL MOBIL (Inisialisasi)
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
// FITUR GAME (Inisialisasi)
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
// SKID MARK (Inisialisasi Array & Counter)
// ==============================
// Array ini akan otomatis terisi nol/false di memori saat program dimulai
SkidMark skidMarks[MAX_SKID]; 
int skidIndex = 0;
int skidFrame = 0;