#include "car_graphics.h" // buat fungsi gambar mobil sama efek visual
#include "environment.h" // buat fungsi gambar track, gunung, dll
#include "utils.h" // buat fungsi utilitas seperti setMaterial, drawBox, dll
#include "globals.h" // buat deklarasi variabel global seperti posisi mobil, kecepatan, skor, dll
#include <GL/glut.h> // buat fungsi OpenGL   
#include <math.h> // buat fungsi matematika seperti sin, cos, dll       

// ==============================
// MOBIL & EFEK VISUAL
// ==============================

// Skidmarks memudar perlahan
void drawSkidMarks()
{
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i = 0; i < MAX_SKID; i++)
    {
        if (!skidMarks[i].active)
            continue;

        //  persentase fade out 
        float alpha = (skidMarks[i].lifeTime / 5.0f) * 0.85f;
        if (alpha < 0.0f) alpha = 0.0f;

        glColor4f(0.015f, 0.015f, 0.015f, alpha);

        glPushMatrix();
        glTranslatef(skidMarks[i].x, 0.075f, skidMarks[i].z);
        glRotatef(skidMarks[i].angle, 0, 1, 0);
        drawBox(0.16f, 0.01f, 0.55f);
        glPopMatrix();
    }

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

void drawSmoke()
{
    if (!isDrifting && fabs(carSpeed) < 0.10f) return;

    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushMatrix();
    glTranslatef(carX, 0.18f, carZ);
    glRotatef(carAngle, 0, 1, 0);

    int smokeCount = isDrifting ? 22 : 7;
    for (int i = 0; i < smokeCount; i++) {
        float t = i / (float)(smokeCount - 1);
        float x = 1.3f + t * 4.2f;
        float y = 0.20f + sin(smokeTime + i * 0.6f) * 0.12f;
        float z = sin(t * 5.0f + smokeTime) * 0.48f;

        float size = isDrifting ? 0.20f + t * 0.62f : 0.14f + t * 0.30f;
        float alpha = isDrifting ? 0.50f - t * 0.28f : 0.25f - t * 0.14f;

        glColor4f(0.90f, 0.90f, 0.88f, alpha);
        glPushMatrix();
        glTranslatef(x, y, z);
        glScalef(1.7f, 0.75f, 0.95f);
        glutSolidSphere(size, 22, 22);
        glPopMatrix();
    }
    glPopMatrix();
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

void drawWheel(float x, float y, float z, bool steerable)
{
    glPushMatrix();
    glTranslatef(x, y, z);
    if (steerable) glRotatef(steerVisualAngle, 0, 1, 0);
    glRotatef(wheelRot, 0, 0, 1);

    setMaterial(0.03f, 0.03f, 0.03f, 20.0f);
    glutSolidTorus(0.10f, 0.31f, 18, 32);

    setMaterial(0.80f, 0.80f, 0.82f, 90.0f);
    glutSolidTorus(0.035f, 0.15f, 14, 24);

    setMaterial(0.88f, 0.88f, 0.90f, 100.0f);
    glutSolidSphere(0.07f, 14, 14);

    glDisable(GL_LIGHTING);
    glColor3f(0.96f, 0.96f, 0.96f);
    glBegin(GL_LINES);
    for (int i = 0; i < 8; i++) {
        float a = i * 2.0f * M_PI / 8.0f;
        glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(cos(a) * 0.20f, sin(a) * 0.20f, 0.0f);
    }
    glEnd();
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

void drawCarBody()
{
    bool braking = specialKey[GLUT_KEY_DOWN];
    setMaterial(1.0f, 0.02f, 0.55f, 65.0f);
    glPushMatrix(); glTranslatef(0.0f, 0.38f, 0.0f); drawBox(3.95f, 0.52f, 1.60f); glPopMatrix();

    setMaterial(0.95f, 0.01f, 0.48f, 55.0f);
    glPushMatrix(); glTranslatef(-2.06f, 0.25f, 0.0f); drawBox(0.22f, 0.33f, 1.68f); glPopMatrix();

    glPushMatrix(); glTranslatef(2.06f, 0.27f, 0.0f); drawBox(0.22f, 0.34f, 1.64f); glPopMatrix();

    setMaterial(0.04f, 0.04f, 0.05f, 45.0f);
    glPushMatrix(); glTranslatef(0.0f, 0.15f, 0.90f); drawBox(3.75f, 0.16f, 0.12f); glPopMatrix();
    glPushMatrix(); glTranslatef(0.0f, 0.15f, -0.90f); drawBox(3.75f, 0.16f, 0.12f); glPopMatrix();

    setMaterial(0.02f, 0.02f, 0.025f, 70.0f);
    glPushMatrix(); glTranslatef(-1.02f, 0.66f, 0.0f); drawBox(1.38f, 0.08f, 1.28f); glPopMatrix();

    setMaterial(0.0f, 0.0f, 0.0f, 25.0f);
    glPushMatrix(); glTranslatef(-1.05f, 0.72f, -0.28f); drawBox(0.46f, 0.03f, 0.12f); glPopMatrix();
    glPushMatrix(); glTranslatef(-1.05f, 0.72f, 0.28f); drawBox(0.46f, 0.03f, 0.12f); glPopMatrix();

    setMaterial(1.0f, 0.02f, 0.55f, 65.0f);
    glPushMatrix(); glTranslatef(1.20f, 0.64f, 0.0f); drawBox(0.92f, 0.12f, 1.40f); glPopMatrix();

    glPushMatrix(); glTranslatef(0.38f, 0.90f, 0.0f); drawBox(1.58f, 0.38f, 1.28f); glPopMatrix();

    glPushMatrix(); glTranslatef(0.34f, 1.17f, 0.0f); drawBox(1.10f, 0.12f, 1.12f); glPopMatrix();

    setMaterial(0.42f, 0.83f, 1.0f, 95.0f);
    glPushMatrix(); glTranslatef(-0.62f, 0.94f, 0.0f); glRotatef(-39.0f, 0, 0, 1); drawBox(0.12f, 0.65f, 1.26f); glPopMatrix();

    glPushMatrix(); glTranslatef(0.96f, 0.98f, 0.0f); glRotatef(30.0f, 0, 0, 1); drawBox(0.12f, 0.52f, 1.15f); glPopMatrix();

    for (int s = -1; s <= 1; s += 2) {
        glPushMatrix(); glTranslatef(0.18f, 1.00f, s * 0.67f); drawBox(1.12f, 0.27f, 0.045f); glPopMatrix();
    }

    setMaterial(0.06f, 0.06f, 0.07f, 40.0f);
    for (int s = -1; s <= 1; s += 2) {
        glPushMatrix(); glTranslatef(-0.28f, 1.00f, s * 0.69f); drawBox(0.05f, 0.36f, 0.05f); glPopMatrix();
        glPushMatrix(); glTranslatef(0.34f, 1.00f, s * 0.69f); drawBox(0.05f, 0.36f, 0.05f); glPopMatrix();
    }

    setMaterial(0.02f, 0.02f, 0.02f, 20.0f);
    glPushMatrix(); glTranslatef(-2.16f, 0.35f, 0.0f); drawBox(0.05f, 0.22f, 0.74f); glPopMatrix();

    if (nightMode) setMaterial(1.0f, 1.0f, 0.55f, 120.0f);
    else setMaterial(0.96f, 0.88f, 0.42f, 95.0f);

    glPushMatrix(); glTranslatef(-2.12f, 0.40f, -0.54f); drawBox(0.07f, 0.18f, 0.30f); glPopMatrix();
    glPushMatrix(); glTranslatef(-2.12f, 0.40f, 0.54f); drawBox(0.07f, 0.18f, 0.30f); glPopMatrix();

    if (braking) setMaterial(1.0f, 0.0f, 0.0f, 120.0f);
    else setMaterial(0.75f, 0.02f, 0.02f, 80.0f);

    glPushMatrix(); glTranslatef(2.12f, 0.38f, -0.50f); drawBox(0.07f, 0.18f, 0.30f); glPopMatrix();
    glPushMatrix(); glTranslatef(2.12f, 0.38f, 0.50f); drawBox(0.07f, 0.18f, 0.30f); glPopMatrix();

    setMaterial(0.02f, 0.02f, 0.025f, 40.0f);
    glPushMatrix(); glTranslatef(2.18f, 0.16f, 0.0f); drawBox(0.10f, 0.18f, 1.08f); glPopMatrix();

    setMaterial(0.78f, 0.78f, 0.80f, 100.0f);
    glPushMatrix(); glTranslatef(2.27f, 0.18f, -0.34f); glRotatef(90, 0, 1, 0); glutSolidTorus(0.025f, 0.08f, 14, 24); glPopMatrix();
    glPushMatrix(); glTranslatef(2.27f, 0.18f, 0.34f); glRotatef(90, 0, 1, 0); glutSolidTorus(0.025f, 0.08f, 14, 24); glPopMatrix();

    setMaterial(1.0f, 0.02f, 0.55f, 70.0f);
    glPushMatrix(); glTranslatef(1.74f, 1.08f, 0.0f); drawBox(0.16f, 0.11f, 1.82f); glPopMatrix();

    setMaterial(0.04f, 0.04f, 0.05f, 45.0f);
    glPushMatrix(); glTranslatef(1.65f, 0.87f, -0.60f); drawBox(0.07f, 0.42f, 0.07f); glPopMatrix();
    glPushMatrix(); glTranslatef(1.65f, 0.87f, 0.60f); drawBox(0.07f, 0.42f, 0.07f); glPopMatrix();

    setMaterial(1.0f, 1.0f, 1.0f, 50.0f);
    glPushMatrix(); glTranslatef(0.05f, 0.67f, 0.82f); drawBox(2.75f, 0.07f, 0.035f); glPopMatrix();
    glPushMatrix(); glTranslatef(0.05f, 0.67f, -0.82f); drawBox(2.75f, 0.07f, 0.035f); glPopMatrix();
}

void drawCar()
{
    glPushMatrix();
    glTranslatef(carX, 0.12f, carZ);
    glRotatef(carAngle, 0, 1, 0);
    drawCarBody();
    drawWheel(-1.15f, 0.28f,  0.92f, true);
    drawWheel(-1.15f, 0.28f, -0.92f, true);
    drawWheel( 1.15f, 0.28f,  0.92f, false);
    drawWheel( 1.15f, 0.28f, -0.92f, false);
    glPopMatrix();
}
