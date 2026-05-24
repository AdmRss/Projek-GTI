#include "globals.h"

// ==============================
// GLOBAL MOBIL
// ==============================
float carX     = 20.0f;   // mulai di straight selatan, heading barat
float carZ     = -12.0f;
float carAngle = 0.0f;    // angle=0 → forwardX=-1 (bergerak ke -X / barat)
float carSpeed = 0.0f;

float wheelRot       = 0.0f;
float smokeTime      = 0.0f;

float steerVisualAngle = 0.0f;
float carYawVel        = 0.0f;
float driftSlipAngle   = 0.0f;

bool specialKey[256] = { false };
bool normalKey[256]  = { false };
bool isDrifting      = false;

// ==============================
// FITUR GAME
// ==============================
int  cameraMode = 0;
bool nightMode  = false;

float driftScore      = 0.0f;
float comboMultiplier = 1.0f;
int   driftFrames     = 0;

int  lapCount         = 0;
int  nextCheckpoint   = 0;
bool lapStarted       = false;
int  checkpointCooldown = 0;

float speedMultiplier  = 1.0f;
bool  isDraggingSlider = false;

// ==============================
// RACE / WIN-LOSE SYSTEM
// ==============================
GameState gameState  = STATE_PLAYING;
float     raceTime   = 0.0f;
float     bestTime   = -1.0f;   // -1 = belum pernah finish
bool      isNewRecord = false;

// ==============================
// CONFETTI
// ==============================
Confetti confettiParticles[MAX_CONFETTI];
bool confettiActive = false;

// ==============================
// SKID MARK
// ==============================
SkidMark skidMarks[MAX_SKID];
int skidIndex = 0;
int skidFrame = 0;

// ==============================
// TEXTURE IDs
// ==============================
GLuint texAsphalt  = 0;
GLuint texGrass    = 0;
GLuint texConcrete = 0;