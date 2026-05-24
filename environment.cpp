#include "environment.h"
#include "utils.h"
#include "globals.h"
#include <GL/glut.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ==============================
// HELPER: Warna lingkungan dari nightMode
// ==============================
// Kalikan warna (r,g,b) dengan faktor siang/malam sebelum set material
static void envMaterial(float r, float g, float b, float shininess = 20.0f)
{
    float f = nightMode ? 0.40f : 1.0f; // malam lebih gelap
    setMaterial(r * f, g * f, b * f, shininess);
}

// ==============================
// HELPER: Gambar quad bertekstur (asphalt/grass)
// ==============================
static void drawTexQuad(GLuint tex, float y,
                        float x1, float z1,
                        float x2, float z2,
                        float sScale, float tScale)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // Warna modulator: siang putih hangat, malam biru keabu-abuan
    if (nightMode) glColor3f(0.45f, 0.47f, 0.62f);
    else           glColor3f(0.95f, 0.92f, 0.88f);

    glDisable(GL_LIGHTING); // ground tidak butuh per-vertex lighting
    glBegin(GL_QUADS);
        glTexCoord2f(x1 * sScale, z1 * tScale); glVertex3f(x1, y, z1);
        glTexCoord2f(x2 * sScale, z1 * tScale); glVertex3f(x2, y, z1);
        glTexCoord2f(x2 * sScale, z2 * tScale); glVertex3f(x2, y, z2);
        glTexCoord2f(x1 * sScale, z2 * tScale); glVertex3f(x1, y, z2);
    glEnd();
    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
}

// ==============================
// KERB (RUMBLE STRIP)
// ==============================
void drawCurbHorizontal(float x1, float x2, float z, float depth)
{
    float seg = 1.2f;
    int count = (int)((x2 - x1) / seg);
    for (int i = 0; i < count; i++) {
        float cx = x1 + i * seg + seg * 0.5f;
        float f  = nightMode ? 0.55f : 1.0f;
        if (i % 2 == 0) setMaterial(0.95f * f, 0.08f * f, 0.08f * f, 15.0f);
        else             setMaterial(0.96f * f, 0.96f * f, 0.96f * f, 15.0f);
        glPushMatrix();
        glTranslatef(cx, 0.040f, z);
        drawBox(seg, 0.08f, depth);
        glPopMatrix();
    }
}

void drawCurbVertical(float x, float z1, float z2, float width)
{
    float seg = 1.2f;
    int count = (int)((z2 - z1) / seg);
    for (int i = 0; i < count; i++) {
        float cz = z1 + i * seg + seg * 0.5f;
        float f  = nightMode ? 0.55f : 1.0f;
        if (i % 2 == 0) setMaterial(0.95f * f, 0.08f * f, 0.08f * f, 15.0f);
        else             setMaterial(0.96f * f, 0.96f * f, 0.96f * f, 15.0f);
        glPushMatrix();
        glTranslatef(x, 0.040f, cz);
        drawBox(width, 0.08f, seg);
        glPopMatrix();
    }
}

// ==============================
// GUARD RAIL
// ==============================
static void grMat() { envMaterial(0.70f, 0.72f, 0.75f, 55.0f); }

void drawGuardRailHorizontal(float x1, float x2, float z)
{
    grMat();
    float cx = (x1 + x2) * 0.5f, len = x2 - x1;
    glPushMatrix(); glTranslatef(cx, 0.55f, z); drawBox(len, 0.08f, 0.10f); glPopMatrix();
    glPushMatrix(); glTranslatef(cx, 0.90f, z); drawBox(len, 0.08f, 0.10f); glPopMatrix();
    for (float x = x1; x <= x2; x += 1.8f) {
        glPushMatrix(); glTranslatef(x, 0.50f, z); drawBox(0.08f, 1.0f, 0.08f); glPopMatrix();
    }
}

void drawGuardRailVertical(float x, float z1, float z2)
{
    grMat();
    float cz = (z1 + z2) * 0.5f, len = z2 - z1;
    glPushMatrix(); glTranslatef(x, 0.55f, cz); drawBox(0.10f, 0.08f, len); glPopMatrix();
    glPushMatrix(); glTranslatef(x, 0.90f, cz); drawBox(0.10f, 0.08f, len); glPopMatrix();
    for (float z = z1; z <= z2; z += 1.8f) {
        glPushMatrix(); glTranslatef(x, 0.50f, z); drawBox(0.08f, 1.0f, 0.08f); glPopMatrix();
    }
}

// ==============================
// CONE
// ==============================
void drawCone(float x, float z)
{
    float f = nightMode ? 0.65f : 1.0f;
    glPushMatrix();
    glTranslatef(x, 0.02f, z);
    setMaterial(1.0f * f, 0.35f * f, 0.05f * f, 20.0f);
    glPushMatrix(); glRotatef(-90, 1, 0, 0); glutSolidCone(0.27f, 0.75f, 20, 12); glPopMatrix();
    setMaterial(0.98f * f, 0.98f * f, 0.98f * f, 10.0f);
    glPushMatrix(); glTranslatef(0, 0.22f, 0); drawBox(0.44f, 0.07f, 0.44f); glPopMatrix();
    glPopMatrix();
}

// ==============================
// GARIS KOTAK-KOTAK (FINISH)
// ==============================
void drawCheckeredLine(float minX, float maxX, float minZ, float maxZ)
{
    float size = 0.8f;
    int xC = (int)((maxX - minX) / size + 0.1f);
    int zC = (int)((maxZ - minZ) / size + 0.1f);
    float f = nightMode ? 0.55f : 1.0f;

    glDisable(GL_LIGHTING);
    glBegin(GL_QUADS);
    for (int i = 0; i < xC; i++) {
        for (int j = 0; j < zC; j++) {
            if ((i + j) % 2 == 0) glColor3f(0.95f * f, 0.95f * f, 0.95f * f);
            else                   glColor3f(0.05f, 0.05f, 0.05f);
            float x1 = minX + i * size, x2 = x1 + size;
            float z1 = minZ + j * size, z2 = z1 + size;
            glVertex3f(x1, 0.068f, z1); glVertex3f(x2, 0.068f, z1);
            glVertex3f(x2, 0.068f, z2); glVertex3f(x1, 0.068f, z2);
        }
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

// ==============================
// CHECKPOINT & FINISH MARKERS
// ==============================
void drawCheckpointMarkers()
{
    glDisable(GL_LIGHTING);

    // Finish line (catur) di straight selatan, x=0, z≈-10
    drawCheckeredLine(-1.0f, 1.0f, -14.5f, -6.0f);

    // Checkpoint biru neon transparan
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.1f, 0.8f, 1.0f, 0.40f);

    // CP0: straight atas (x≈0)
    glBegin(GL_QUADS);
        glVertex3f(-1.0f, 0.068f,  6.0f); glVertex3f( 1.0f, 0.068f,  6.0f);
        glVertex3f( 1.0f, 0.068f, 14.5f); glVertex3f(-1.0f, 0.068f, 14.5f);
    glEnd();

    // CP1: straight kanan (z≈0)
    glBegin(GL_QUADS);
        glVertex3f( 9.5f, 0.068f, -1.0f); glVertex3f(22.5f, 0.068f, -1.0f);
        glVertex3f(22.5f, 0.068f,  1.0f); glVertex3f( 9.5f, 0.068f,  1.0f);
    glEnd();

    // CP3: straight kiri (z≈0)
    glBegin(GL_QUADS);
        glVertex3f(-22.5f, 0.068f, -1.0f); glVertex3f(-9.5f, 0.068f, -1.0f);
        glVertex3f( -9.5f, 0.068f,  1.0f); glVertex3f(-22.5f, 0.068f,  1.0f);
    glEnd();

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

// ==============================
// GAPURA START/FINISH
// ==============================
void drawStartGantry()
{
    float f = nightMode ? 0.5f : 1.0f;

    // Tiang pilar kiri (sisi outer, z≈-14)
    setMaterial(0.82f * f, 0.82f * f, 0.86f * f, 70.0f);
    glPushMatrix(); glTranslatef(0.0f, 2.6f, -14.0f); drawBox(0.45f, 5.2f, 0.45f); glPopMatrix();

    // Tiang pilar kanan (sisi inner, z≈-6.5)
    glPushMatrix(); glTranslatef(0.0f, 2.6f, -6.5f); drawBox(0.45f, 5.2f, 0.45f); glPopMatrix();

    // Balok horizontal
    glPushMatrix(); glTranslatef(0.0f, 5.35f, -10.25f); drawBox(0.5f, 0.5f, 7.8f); glPopMatrix();

    // Papan hitam (tulisan "START/FINISH")
    setMaterial(0.06f * f, 0.06f * f, 0.08f * f, 30.0f);
    glPushMatrix(); glTranslatef(0.0f, 4.65f, -10.25f); drawBox(0.22f, 0.90f, 6.0f); glPopMatrix();

    // Strip pink pada balok
    setMaterial(1.0f * f, 0.22f * f, 0.60f * f, 90.0f);
    glPushMatrix(); glTranslatef(0.0f, 5.65f, -10.25f); drawBox(0.55f, 0.30f, 7.8f); glPopMatrix();

    // Lampu kuning di ujung balok (menyala lebih cerah malam)
    float lf = nightMode ? 1.0f : 0.8f;
    setMaterial(1.0f * lf, 0.88f * lf, 0.15f * lf, 120.0f);
    glPushMatrix(); glTranslatef(0.0f, 5.35f, -14.3f); glutSolidSphere(0.22f, 14, 14); glPopMatrix();
    glPushMatrix(); glTranslatef(0.0f, 5.35f,  -6.2f); glutSolidSphere(0.22f, 14, 14); glPopMatrix();
}

// ==============================
// TRIBUNE PENONTON
// ==============================
void drawGrandstand(float cx, float cz, float rotY)
{
    glPushMatrix();
    glTranslatef(cx, 0.0f, cz);
    glRotatef(rotY, 0, 1, 0);

    float f = nightMode ? 0.5f : 1.0f;
    // Baris kursi (4 tingkat)
    for (int row = 0; row < 4; row++) {
        float ry = 0.55f + row * 0.55f;
        float rz = -(float)row * 0.85f;
        if (row % 2 == 0) setMaterial(1.0f * f, 0.22f * f, 0.60f * f, 20.0f);
        else               setMaterial(0.90f * f, 0.90f * f, 0.90f * f, 15.0f);
        glPushMatrix(); glTranslatef(0, ry, rz); drawBox(7.5f, 0.45f, 0.70f); glPopMatrix();
    }
    // Rangka belakang
    setMaterial(0.50f * f, 0.50f * f, 0.52f * f, 40.0f);
    glPushMatrix(); glTranslatef(0, 1.2f, -2.6f); drawBox(7.5f, 2.4f, 0.18f); glPopMatrix();
    // Atap
    setMaterial(0.80f * f, 0.80f * f, 0.82f * f, 50.0f);
    glPushMatrix(); glTranslatef(0, 2.55f, -1.0f); drawBox(8.0f, 0.18f, 3.5f); glPopMatrix();

    glPopMatrix();
}

// ==============================
// POHON
// ==============================
void drawTree(float x, float z, float height)
{
    float f = nightMode ? 0.40f : 1.0f;
    glPushMatrix();
    glTranslatef(x, 0.0f, z);

    // Batang
    setMaterial(0.40f * f, 0.26f * f, 0.10f * f, 10.0f);
    glPushMatrix(); glTranslatef(0, height * 0.28f, 0); drawBox(0.25f, height * 0.55f, 0.25f); glPopMatrix();

    // Kanopi (3 lapisan kerucut)
    for (int k = 0; k < 3; k++) {
        float ky = height * 0.45f + k * (height * 0.18f);
        float kr = 0.95f - k * 0.22f;
        setMaterial(0.18f * f * kr, (0.52f + k * 0.06f) * f * kr, 0.15f * f * kr, 12.0f);
        glPushMatrix();
        glTranslatef(0, ky, 0);
        glRotatef(-90, 1, 0, 0);
        glutSolidCone(height * 0.32f - k * 0.06f, height * 0.38f, 18, 10);
        glPopMatrix();
    }
    glPopMatrix();
}

// ==============================
// TRACK UTAMA
// ==============================
void drawTrack()
{
    float texS = 0.042f; // skala UV asphalt (≈ 24 tile per 48 unit)
    float texG = 0.080f; // skala UV grass

    // --- Ground / infield besar (grass) ---
    drawTexQuad(texGrass, -0.05f,
                -37.0f, -27.0f, 37.0f, 27.0f,
                texG, texG);

    // --- Asphalt: outer rectangle (gambar 4 sisi sesuai koridor) ---
    // Straight atas (north)
    drawTexQuad(texAsphalt, 0.001f,
                OUTER_LEFT, INNER_TOP, OUTER_RIGHT, OUTER_TOP,
                texS, texS);
    // Straight bawah (south, finish straight)
    drawTexQuad(texAsphalt, 0.001f,
                OUTER_LEFT, OUTER_BOTTOM, OUTER_RIGHT, INNER_BOTTOM,
                texS, texS);
    // Straight kanan (east)
    drawTexQuad(texAsphalt, 0.001f,
                INNER_RIGHT, INNER_BOTTOM, OUTER_RIGHT, INNER_TOP,
                texS, texS);
    // Straight kiri (west)
    drawTexQuad(texAsphalt, 0.001f,
                OUTER_LEFT, INNER_BOTTOM, INNER_LEFT, INNER_TOP,
                texS, texS);
    // Sudut NE
    drawTexQuad(texAsphalt, 0.001f,
                INNER_RIGHT, INNER_TOP, OUTER_RIGHT, OUTER_TOP,
                texS, texS);
    // Sudut NW
    drawTexQuad(texAsphalt, 0.001f,
                OUTER_LEFT, INNER_TOP, INNER_LEFT, OUTER_TOP,
                texS, texS);
    // Sudut SE
    drawTexQuad(texAsphalt, 0.001f,
                INNER_RIGHT, OUTER_BOTTOM, OUTER_RIGHT, INNER_BOTTOM,
                texS, texS);
    // Sudut SW
    drawTexQuad(texAsphalt, 0.001f,
                OUTER_LEFT, OUTER_BOTTOM, INNER_LEFT, INNER_BOTTOM,
                texS, texS);

    // --- Infield island (grass di atas asphalt) ---
    drawTexQuad(texGrass, 0.022f,
                INNER_LEFT, INNER_BOTTOM, INNER_RIGHT, INNER_TOP,
                texG, texG);

    // --- Garis putih tepi lintasan ---
    glDisable(GL_LIGHTING);
    float lf = nightMode ? 0.70f : 0.98f;
    glColor3f(lf, lf, lf);
    // Garis tepi dalam & luar (titik-titik)
    for (int i = -8; i <= 8; i++) {
        // Straight atas
        glBegin(GL_QUADS);
            glVertex3f(i * 2.6f - 0.85f, 0.05f,  8.4f); glVertex3f(i * 2.6f + 0.85f, 0.05f,  8.4f);
            glVertex3f(i * 2.6f + 0.85f, 0.05f,  8.65f); glVertex3f(i * 2.6f - 0.85f, 0.05f,  8.65f);
        glEnd();
        // Straight bawah
        glBegin(GL_QUADS);
            glVertex3f(i * 2.6f - 0.85f, 0.05f, -8.65f); glVertex3f(i * 2.6f + 0.85f, 0.05f, -8.65f);
            glVertex3f(i * 2.6f + 0.85f, 0.05f, -8.4f); glVertex3f(i * 2.6f - 0.85f, 0.05f, -8.4f);
        glEnd();
    }
    glEnable(GL_LIGHTING);

    // --- Checkpoint markers ---
    drawCheckpointMarkers();

    // --- Garis start (kotak-kotak) ---
    drawCheckeredLine(-1.0f, 1.0f, -14.5f, -6.0f);

    // --- Kerb ---
    drawCurbHorizontal(-24.0f, 24.0f, -16.0f, 0.58f);
    drawCurbHorizontal(-24.0f, 24.0f,  16.0f, 0.58f);
    drawCurbVertical  (-24.0f, -16.0f, 16.0f, 0.58f);
    drawCurbVertical  ( 24.0f, -16.0f, 16.0f, 0.58f);
    drawCurbHorizontal(-8.8f,  8.8f, -4.8f,  0.52f);
    drawCurbHorizontal(-8.8f,  8.8f,  4.8f,  0.52f);
    drawCurbVertical  (-8.8f, -4.8f,  4.8f,  0.52f);
    drawCurbVertical  ( 8.8f, -4.8f,  4.8f,  0.52f);

    // --- Guard rail ---
    drawGuardRailHorizontal(-25.6f, 25.6f, -17.8f);
    drawGuardRailHorizontal(-25.6f, 25.6f,  17.8f);
    drawGuardRailVertical  (-25.8f, -17.8f, 17.8f);
    drawGuardRailVertical  ( 25.8f, -17.8f, 17.8f);
    drawGuardRailHorizontal(-9.7f,  9.7f,  -5.8f);
    drawGuardRailHorizontal(-9.7f,  9.7f,   5.8f);
    drawGuardRailVertical  (-9.8f, -5.8f,   5.8f);
    drawGuardRailVertical  ( 9.8f, -5.8f,   5.8f);

    // --- Cone di pojok inner island ---
    drawCone(-7.2f,  7.5f); drawCone(-3.0f,  7.6f); drawCone( 3.0f,  7.6f); drawCone( 7.2f,  7.5f);
    drawCone(-7.2f, -7.5f); drawCone(-3.0f, -7.6f); drawCone( 3.0f, -7.6f); drawCone( 7.2f, -7.5f);

    // --- Gapura start/finish ---
    drawStartGantry();

    // --- Tribune di sisi luar ---
    drawGrandstand(-20.0f, -19.5f, 0.0f);   // tribune kiri-selatan
    drawGrandstand( 14.0f, -19.5f, 0.0f);   // tribune kanan-selatan
    drawGrandstand( 27.0f,   3.0f, 90.0f);  // tribune kanan (side)
    drawGrandstand(-27.0f,   3.0f, -90.0f); // tribune kiri (side)

    // --- Pohon dekorasi sekeliling ---
    float treePosX[] = { -28.0f, -30.0f, -32.0f,  28.0f,  30.0f,  32.0f,
                          -15.0f,  15.0f,   0.0f,  -6.0f,   6.0f };
    float treePosZ[] = {   2.0f,  -4.0f,   6.0f,   2.0f,  -4.0f,   6.0f,
                          -22.0f, -22.0f,  -23.0f,  20.0f,  20.0f };
    float treeH[]    = {   3.5f,   4.2f,   3.0f,   3.8f,   3.2f,   4.0f,
                            3.5f,   3.7f,   4.2f,   3.0f,   3.5f };
    int numTrees = 11;
    for (int t = 0; t < numTrees; t++)
        drawTree(treePosX[t], treePosZ[t], treeH[t]);
}

// ==============================
// PEGUNUNGAN LATAR
// ==============================
void drawMountains()
{
    float f = nightMode ? 0.28f : 1.0f;

    // Range pegunungan belakang (utara)
    float rN = nightMode ? 0.15f : 0.17f;
    float gN = nightMode ? 0.25f : 0.36f;
    float bN = nightMode ? 0.20f : 0.20f;
    setMaterial(rN, gN, bN, 5.0f);
    glBegin(GL_TRIANGLES);
        glNormal3f(0, 1, 0.3f);
        glVertex3f(-36, 0, -26); glVertex3f(-26, 11, -26); glVertex3f(-15, 0, -26);
        glVertex3f(-19, 0, -26.5f); glVertex3f( -8,  8, -26.5f); glVertex3f( 3,   0, -26.5f);
        glVertex3f( -3, 0, -26); glVertex3f(  9, 10, -26); glVertex3f( 20,  0, -26);
        glVertex3f( 14, 0, -26.5f); glVertex3f( 24,  7, -26.5f); glVertex3f( 35,  0, -26.5f);
    glEnd();

    // Range selatan (lighter, snow caps jika siang)
    setMaterial(rN * 1.15f, gN * 1.1f, bN * 1.2f, 5.0f);
    glBegin(GL_TRIANGLES);
        glNormal3f(0, 1, -0.3f);
        glVertex3f(-32, 0, 26); glVertex3f(-22, 8, 26); glVertex3f(-12, 0, 26);
        glVertex3f( 10, 0, 26); glVertex3f( 20, 7, 26); glVertex3f( 30,  0, 26);
    glEnd();

    // Salju puncak (hanya siang)
    if (!nightMode) {
        setMaterial(0.95f, 0.96f, 0.98f, 20.0f);
        glBegin(GL_TRIANGLES);
            glVertex3f(-28.0f, 9.5f, -26); glVertex3f(-26.0f, 11.0f, -26); glVertex3f(-24.0f, 9.5f, -26);
            glVertex3f( 7.0f,  8.5f, -26); glVertex3f(  9.0f, 10.0f, -26); glVertex3f( 11.0f,  8.5f, -26);
        glEnd();
    }
}

// ==============================
// LANGIT MALAM (BINTANG)
// ==============================
void drawNightSky()
{
    if (!nightMode) return;

    glDisable(GL_LIGHTING);
    glPointSize(2.2f);
    glColor3f(0.95f, 0.95f, 1.0f);

    // Bintang tetap (pseudo-random berdasarkan index)
    glBegin(GL_POINTS);
    unsigned int s = 7919u; // seed tetap
    for (int i = 0; i < 180; i++) {
        s = s * 1664525u + 1013904223u;
        float bx = (float)(s % 2000) / 2000.0f * 80.0f - 40.0f;
        s = s * 1664525u + 1013904223u;
        float by = 8.0f + (float)(s % 1000) / 1000.0f * 22.0f;
        s = s * 1664525u + 1013904223u;
        float bz = (float)(s % 2000) / 2000.0f * 80.0f - 40.0f;
        glVertex3f(bx, by, bz);
    }
    glEnd();

    glPointSize(1.0f);
    glEnable(GL_LIGHTING);
}