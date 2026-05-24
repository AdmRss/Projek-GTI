#include "input_ui.h"
#include "globals.h"
#include "utils.h"
#include <GL/glut.h>
#include <math.h>
#include <cstdio>
#include <cstring>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ==============================
// HELPER: RENDER STRING ORTHO
// ==============================
void renderString(float x, float y, void* font, const char* str)
{
    glRasterPos2f(x, y);
    for (int i = 0; str[i] != '\0'; i++)
        glutBitmapCharacter(font, str[i]);
}

// ==============================
// HELPER: Masuk/Keluar mode 2D ortho
// ==============================
static void beginOrtho()
{
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    gluOrtho2D(0, 1000, 0, 550);
    glMatrixMode(GL_MODELVIEW);  glPushMatrix(); glLoadIdentity();
}

static void endOrtho()
{
    glPopMatrix();
    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_LIGHTING);
}

// ==============================
// HELPER: Panel latar belakang semi-transparan
// ==============================
static void drawPanel(float x, float y, float w, float h,
                      float r, float g, float b, float a)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(r, g, b, a);
    glBegin(GL_QUADS);
        glVertex2f(x,     y);     glVertex2f(x + w, y);
        glVertex2f(x + w, y + h); glVertex2f(x,     y + h);
    glEnd();
    glDisable(GL_BLEND);
}

// ==============================
// FORMAT WAKTU  mm:ss.tt
// ==============================
static void formatTime(float sec, char* buf)
{
    int m  = (int)(sec / 60);
    int s  = (int)(sec) % 60;
    int ms = (int)((sec - (int)sec) * 100.0f);
    sprintf(buf, "%02d:%02d.%02d", m, s, ms);
}

// ==============================
// HUD UTAMA
// ==============================
void drawText()
{
    beginOrtho();

    // --- DRIFT MODE LABEL ---
    if (isDrifting) {
        glColor3f(1.0f, 0.2f, 0.8f);
        renderString(30, 515, GLUT_BITMAP_HELVETICA_18, "DRIFT MODE!");
    }

    // --- INSTRUKSI KONTROL ---
    glColor3f(0.9f, 0.9f, 0.9f);
    renderString(30, 490, GLUT_BITMAP_HELVETICA_12,
        "W/S = gas/rem   A/D = steer   SPACE = drift   C = camera   N = siang/malam   0 = reset");

    // --- SPEED & SCORE ---
    int kmh = (int)(fabs(carSpeed) * 650.0f);
    char buf[180];
    sprintf(buf, "Speed: %d km/h   Drift Score: %.0f   Combo: x%.1f",
            kmh, driftScore, comboMultiplier);
    glColor3f(1.0f, 1.0f, 1.0f);
    renderString(30, 465, GLUT_BITMAP_HELVETICA_12, buf);

    // --- LAP & CHECKPOINT ---
    char timeBuf[32]; formatTime(raceTime, timeBuf);
    char bestBuf[32];
    if (bestTime >= 0.0f) formatTime(bestTime, bestBuf);
    else strcpy(bestBuf, "--:--.--");

    sprintf(buf, "Lap: %d   Checkpoint: %d/4   Time: %s   Best: %s   Cam: %d   [%s]",
            lapCount, nextCheckpoint + 1,
            timeBuf, bestBuf,
            cameraMode + 1,
            nightMode ? "Malam" : "Siang");
    renderString(30, 440, GLUT_BITMAP_HELVETICA_12, buf);

    // --- SLIDER SPEED MULTIPLIER ---
    // Track bar
    drawPanel(798, 498, 152, 14, 0.12f, 0.12f, 0.12f, 0.85f);

    float t     = (speedMultiplier - 0.5f) / 1.5f;
    if (t < 0) t = 0; if (t > 1) t = 1;
    float hx    = 800.0f + t * 150.0f;

    glColor3f(1.0f, 0.78f, 0.0f);
    glBegin(GL_QUADS);
        glVertex2f(hx - 6, 494); glVertex2f(hx + 6, 494);
        glVertex2f(hx + 6, 516); glVertex2f(hx - 6, 516);
    glEnd();

    char sl[64];
    sprintf(sl, "Speed/Handling: %.1fx", speedMultiplier);
    glColor3f(1.0f, 1.0f, 1.0f);
    renderString(800, 524, GLUT_BITMAP_HELVETICA_12, sl);

    endOrtho();
}

// ==============================
// CONFETTI OVERLAY
// ==============================
void drawConfettiOverlay()
{
    if (!confettiActive) return;

    beginOrtho();
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i = 0; i < MAX_CONFETTI; i++) {
        const Confetti& c = confettiParticles[i];
        glPushMatrix();
            glTranslatef(c.x, c.y, 0.0f);
            glRotatef(c.rot, 0, 0, 1);
            glColor4f(c.r, c.g, c.b, 0.92f);
            glBegin(GL_QUADS);
                glVertex2f(-c.size * 0.5f, -c.size * 0.3f);
                glVertex2f( c.size * 0.5f, -c.size * 0.3f);
                glVertex2f( c.size * 0.5f,  c.size * 0.3f);
                glVertex2f(-c.size * 0.5f,  c.size * 0.3f);
            glEnd();
        glPopMatrix();
    }

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    endOrtho();
}

// ==============================
// VICTORY SCREEN OVERLAY
// ==============================
void drawVictoryScreen()
{
    if (gameState != STATE_FINISHED) return;

    beginOrtho();
    glDisable(GL_DEPTH_TEST);

    // --- Panel gelap semi-transparan ---
    drawPanel(250, 120, 500, 310, 0.0f, 0.0f, 0.0f, 0.72f);

    // --- Border pink ---
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 0.20f, 0.60f, 0.90f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(250, 120); glVertex2f(750, 120);
        glVertex2f(750, 430); glVertex2f(250, 430);
    glEnd();
    glLineWidth(1.0f);
    glDisable(GL_BLEND);

    // --- VICTORY ---
    glColor3f(1.0f, 0.22f, 0.62f);
    renderString(328, 385, GLUT_BITMAP_TIMES_ROMAN_24, "V I C T O R Y !");

    // --- NEW RECORD ---
    if (isNewRecord) {
        glColor3f(1.0f, 0.90f, 0.10f);
        renderString(358, 355, GLUT_BITMAP_HELVETICA_18, "*** NEW RECORD! ***");
    }

    // --- Statistik ---
    char tbuf[32], bbuf[32];
    formatTime(raceTime, tbuf);
    if (bestTime >= 0.0f) formatTime(bestTime, bbuf);
    else strcpy(bbuf, "--:--.--");

    char line[128];
    glColor3f(0.95f, 0.95f, 0.95f);

    sprintf(line, "Waktu Tempuh  :  %s", tbuf);
    renderString(295, 320, GLUT_BITMAP_HELVETICA_18, line);

    sprintf(line, "Best Time     :  %s", bbuf);
    renderString(295, 292, GLUT_BITMAP_HELVETICA_18, line);

    sprintf(line, "Drift Score   :  %.0f", driftScore);
    renderString(295, 264, GLUT_BITMAP_HELVETICA_18, line);

    sprintf(line, "Max Combo     :  %.0fx", comboMultiplier);
    renderString(295, 236, GLUT_BITMAP_HELVETICA_18, line);

    // --- Tombol RESTART ---
    // Panel tombol
    drawPanel(340, 148, 160, 40, 1.0f, 0.20f, 0.60f, 0.92f);
    glColor3f(1.0f, 1.0f, 1.0f);
    renderString(365, 163, GLUT_BITMAP_HELVETICA_18, "R = Restart");

    // Tombol reset total
    drawPanel(520, 148, 180, 40, 0.18f, 0.18f, 0.22f, 0.92f);
    glColor3f(0.88f, 0.88f, 0.88f);
    renderString(545, 163, GLUT_BITMAP_HELVETICA_18, "0 = Full Reset");

    glEnable(GL_DEPTH_TEST);
    endOrtho();
}

// ==============================
// KEYBOARD
// ==============================
void keyboardDown(unsigned char key, int x, int y)
{
    normalKey[(unsigned char)key] = true;

    switch (key)
    {
        case '0':
            resetScene();
            break;
        case 'r': case 'R':
            if (gameState == STATE_FINISHED) restartRace();
            break;
        case 'c': case 'C':
            cameraMode = (cameraMode + 1) % 3;
            break;
        case 'n': case 'N':
            nightMode = !nightMode;
            break;
        case 27:  // ESC
            exit(0);
            break;
    }
    glutPostRedisplay();
}

void keyboardUp(unsigned char key, int x, int y)
{
    normalKey[(unsigned char)key] = false;
}

void specialDown(int key, int x, int y) { specialKey[key] = true;  }
void specialUp  (int key, int x, int y) { specialKey[key] = false; }

// ==============================
// MOUSE (SLIDER)
// ==============================
void mouseButton(int button, int state, int x, int y)
{
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    float ox = (float)x / w * 1000.0f;
    float oy = 550.0f - ((float)y / h * 550.0f);

    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            if (ox >= 780 && ox <= 972 && oy >= 480 && oy <= 532) {
                isDraggingSlider = true;
                float t = (ox - 800.0f) / 150.0f;
                if (t < 0) t = 0; if (t > 1) t = 1;
                speedMultiplier = 0.5f + t * 1.5f;
            }
        } else {
            isDraggingSlider = false;
        }
    }
}

void mouseMotion(int x, int y)
{
    if (!isDraggingSlider) return;
    int w = glutGet(GLUT_WINDOW_WIDTH);
    float ox = (float)x / w * 1000.0f;
    float t  = (ox - 800.0f) / 150.0f;
    if (t < 0) t = 0; if (t > 1) t = 1;
    speedMultiplier = 0.5f + t * 1.5f;
}