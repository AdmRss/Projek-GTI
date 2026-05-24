#include "car_graphics.h"
#include "globals.h"
#include "utils.h"
#include <GL/glut.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ==============================
// SKID MARKS (MEMUDAR)
// ==============================
void drawSkidMarks()
{
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i = 0; i < MAX_SKID; i++) {
        if (!skidMarks[i].active) continue;
        float alpha = (skidMarks[i].lifeTime / 5.0f) * 0.85f;
        if (alpha < 0.0f) alpha = 0.0f;
        glColor4f(0.012f, 0.012f, 0.012f, alpha);
        glPushMatrix();
            glTranslatef(skidMarks[i].x, 0.075f, skidMarks[i].z);
            glRotatef(skidMarks[i].angle, 0, 1, 0);
            drawBox(0.18f, 0.01f, 0.56f);
        glPopMatrix();
    }

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

// ==============================
// ASAP BAN (SMOKE)
// ==============================
void drawSmoke()
{
    if (!isDrifting && fabs(carSpeed) < 0.10f) return;

    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushMatrix();
        glTranslatef(carX, 0.18f, carZ);
        glRotatef(carAngle, 0, 1, 0);

        int cnt = isDrifting ? 22 : 7;
        for (int i = 0; i < cnt; i++) {
            float t     = i / (float)(cnt - 1);
            float sx    = 1.3f + t * 4.2f;
            float sy    = 0.20f + sinf(smokeTime + i * 0.6f) * 0.12f;
            float sz    = sinf(t * 5.0f + smokeTime) * 0.48f;
            float size  = isDrifting ? 0.20f + t * 0.62f : 0.14f + t * 0.30f;
            float alpha = isDrifting ? 0.50f - t * 0.28f : 0.25f - t * 0.14f;

            // Warna asap sedikit lebih biru malam
            float sv = nightMode ? 0.78f : 0.90f;
            glColor4f(sv, sv, sv - 0.02f, alpha);
            glPushMatrix();
                glTranslatef(sx, sy, sz);
                glScalef(1.7f, 0.75f, 0.95f);
                glutSolidSphere(size, 14, 14);
            glPopMatrix();
        }
    glPopMatrix();

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

// ==============================
// RODA
// ==============================
void drawWheel(float x, float y, float z, bool steerable)
{
    glPushMatrix();
        glTranslatef(x, y, z);
        if (steerable) glRotatef(steerVisualAngle, 0, 1, 0);
        glRotatef(wheelRot, 0, 0, 1);

        // Ban
        setMaterial(0.05f, 0.05f, 0.05f, 22.0f);
        glutSolidTorus(0.10f, 0.31f, 18, 32);

        // Velg perak
        setMaterial(0.80f, 0.80f, 0.82f, 95.0f);
        glutSolidTorus(0.036f, 0.15f, 14, 24);

        // Hub tengah
        setMaterial(0.88f, 0.88f, 0.90f, 110.0f);
        glutSolidSphere(0.072f, 14, 14);

        // Jari-jari
        glDisable(GL_LIGHTING);
        float lf = nightMode ? 0.80f : 0.96f;
        glColor3f(lf, lf, lf);
        glBegin(GL_LINES);
        for (int i = 0; i < 8; i++) {
            float a = i * 2.0f * M_PI / 8.0f;
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(cosf(a) * 0.20f, sinf(a) * 0.20f, 0.0f);
        }
        glEnd();
        glEnable(GL_LIGHTING);
    glPopMatrix();
}

// ==============================
// BODY MOBIL
// ==============================
void drawCarBody()
{
    bool braking = normalKey['s'] || normalKey['S'];

    // Faktor terang malam
    float f = nightMode ? 0.72f : 1.0f;

    // Bodi utama (pink)
    setMaterial(1.0f * f, 0.02f * f, 0.55f * f, 68.0f);
    glPushMatrix(); glTranslatef( 0.0f, 0.38f,  0.0f); drawBox(3.95f, 0.52f, 1.60f); glPopMatrix();

    // Bumper depan & belakang
    setMaterial(0.94f * f, 0.01f * f, 0.46f * f, 55.0f);
    glPushMatrix(); glTranslatef(-2.06f, 0.25f,  0.0f); drawBox(0.22f, 0.33f, 1.68f); glPopMatrix();
    glPushMatrix(); glTranslatef( 2.06f, 0.27f,  0.0f); drawBox(0.22f, 0.34f, 1.64f); glPopMatrix();

    // Side skirt
    setMaterial(0.06f * f, 0.06f * f, 0.07f * f, 45.0f);
    glPushMatrix(); glTranslatef( 0.0f, 0.15f,  0.92f); drawBox(3.75f, 0.16f, 0.12f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.0f, 0.15f, -0.92f); drawBox(3.75f, 0.16f, 0.12f); glPopMatrix();

    // Atap
    setMaterial(0.03f * f, 0.03f * f, 0.03f * f, 72.0f);
    glPushMatrix(); glTranslatef(-1.02f, 0.66f,  0.0f); drawBox(1.38f, 0.08f, 1.28f); glPopMatrix();

    // Roll cage
    setMaterial(0.0f, 0.0f, 0.0f, 25.0f);
    glPushMatrix(); glTranslatef(-1.05f, 0.72f, -0.28f); drawBox(0.46f, 0.03f, 0.12f); glPopMatrix();
    glPushMatrix(); glTranslatef(-1.05f, 0.72f,  0.28f); drawBox(0.46f, 0.03f, 0.12f); glPopMatrix();

    // Hood
    setMaterial(1.0f * f, 0.02f * f, 0.55f * f, 68.0f);
    glPushMatrix(); glTranslatef( 1.20f, 0.64f,  0.0f); drawBox(0.92f, 0.12f, 1.40f); glPopMatrix();

    // Kabin
    glPushMatrix(); glTranslatef( 0.38f, 0.90f,  0.0f); drawBox(1.58f, 0.38f, 1.28f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.34f, 1.17f,  0.0f); drawBox(1.10f, 0.12f, 1.12f); glPopMatrix();

    // Kaca depan & belakang
    setMaterial(0.42f * f, 0.83f * f, 1.0f * f, 98.0f);
    glPushMatrix(); glTranslatef(-0.62f, 0.94f, 0.0f); glRotatef(-39.0f, 0, 0, 1); drawBox(0.12f, 0.65f, 1.26f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.96f, 0.98f, 0.0f); glRotatef( 30.0f, 0, 0, 1); drawBox(0.12f, 0.52f, 1.15f); glPopMatrix();

    // Jendela samping
    for (int s = -1; s <= 1; s += 2) {
        glPushMatrix(); glTranslatef(0.18f, 1.00f, s * 0.67f); drawBox(1.12f, 0.27f, 0.045f); glPopMatrix();
    }

    // Pilar A/B
    setMaterial(0.07f * f, 0.07f * f, 0.08f * f, 40.0f);
    for (int s = -1; s <= 1; s += 2) {
        glPushMatrix(); glTranslatef(-0.28f, 1.00f, s * 0.69f); drawBox(0.05f, 0.36f, 0.05f); glPopMatrix();
        glPushMatrix(); glTranslatef( 0.34f, 1.00f, s * 0.69f); drawBox(0.05f, 0.36f, 0.05f); glPopMatrix();
    }

    // Bumper depan tengah
    setMaterial(0.03f * f, 0.03f * f, 0.03f * f, 20.0f);
    glPushMatrix(); glTranslatef(-2.16f, 0.35f, 0.0f); drawBox(0.05f, 0.22f, 0.74f); glPopMatrix();

    // --- LAMPU DEPAN ---
    if (nightMode)
        setMaterial(1.0f, 1.0f, 0.62f, 130.0f);
    else
        setMaterial(0.96f * f, 0.88f * f, 0.42f * f, 98.0f);
    glPushMatrix(); glTranslatef(-2.12f, 0.40f, -0.54f); drawBox(0.07f, 0.18f, 0.30f); glPopMatrix();
    glPushMatrix(); glTranslatef(-2.12f, 0.40f,  0.54f); drawBox(0.07f, 0.18f, 0.30f); glPopMatrix();

    // --- LAMPU BELAKANG ---
    if (braking)
        setMaterial(1.0f, 0.02f, 0.02f, 125.0f);   // terang saat ngerem
    else
        setMaterial(0.72f * f, 0.02f * f, 0.02f * f, 80.0f);
    glPushMatrix(); glTranslatef(2.12f, 0.38f, -0.50f); drawBox(0.07f, 0.18f, 0.30f); glPopMatrix();
    glPushMatrix(); glTranslatef(2.12f, 0.38f,  0.50f); drawBox(0.07f, 0.18f, 0.30f); glPopMatrix();

    // Diffuser belakang
    setMaterial(0.03f * f, 0.03f * f, 0.03f * f, 40.0f);
    glPushMatrix(); glTranslatef(2.18f, 0.16f, 0.0f); drawBox(0.10f, 0.18f, 1.08f); glPopMatrix();

    // Knalpot
    setMaterial(0.76f * f, 0.76f * f, 0.78f * f, 105.0f);
    glPushMatrix(); glTranslatef(2.27f, 0.18f, -0.34f); glRotatef(90, 0, 1, 0); glutSolidTorus(0.025f, 0.08f, 14, 24); glPopMatrix();
    glPushMatrix(); glTranslatef(2.27f, 0.18f,  0.34f); glRotatef(90, 0, 1, 0); glutSolidTorus(0.025f, 0.08f, 14, 24); glPopMatrix();

    // Spoiler
    setMaterial(1.0f * f, 0.02f * f, 0.55f * f, 72.0f);
    glPushMatrix(); glTranslatef(1.74f, 1.08f, 0.0f); drawBox(0.16f, 0.11f, 1.82f); glPopMatrix();

    // Tiang spoiler
    setMaterial(0.05f * f, 0.05f * f, 0.06f * f, 45.0f);
    glPushMatrix(); glTranslatef(1.65f, 0.87f, -0.60f); drawBox(0.07f, 0.42f, 0.07f); glPopMatrix();
    glPushMatrix(); glTranslatef(1.65f, 0.87f,  0.60f); drawBox(0.07f, 0.42f, 0.07f); glPopMatrix();

    // Garis putih dekorasi
    setMaterial(1.0f * f, 1.0f * f, 1.0f * f, 52.0f);
    glPushMatrix(); glTranslatef(0.05f, 0.67f,  0.82f); drawBox(2.75f, 0.07f, 0.035f); glPopMatrix();
    glPushMatrix(); glTranslatef(0.05f, 0.67f, -0.82f); drawBox(2.75f, 0.07f, 0.035f); glPopMatrix();
}

// ==============================
// DRAWCAR (ENTRY POINT)
// ==============================
void drawCar()
{
    glPushMatrix();
        glTranslatef(carX, 0.12f, carZ);
        glRotatef(carAngle, 0, 1, 0);
        drawCarBody();
        drawWheel(-1.15f, 0.28f,  0.93f, true);
        drawWheel(-1.15f, 0.28f, -0.93f, true);
        drawWheel( 1.15f, 0.28f,  0.93f, false);
        drawWheel( 1.15f, 0.28f, -0.93f, false);
    glPopMatrix();
}