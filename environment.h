#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

// ==============================
// TRACK & ENVIRONMENT
// ==============================
void drawCurbHorizontal(float x1, float x2, float z, float depth)
{
    float seg = 1.2f;
    int count = (int)((x2 - x1) / seg);
    for (int i = 0; i < count; i++) {
        float cx = x1 + i * seg + seg * 0.5f;
        if (i % 2 == 0) setMaterial(0.95f, 0.08f, 0.08f);
        else setMaterial(0.98f, 0.98f, 0.98f);
        glPushMatrix();
        glTranslatef(cx, 0.035f, z);
        drawBox(seg, 0.07f, depth);
        glPopMatrix();
    }
}

void drawCurbVertical(float x, float z1, float z2, float width)
{
    float seg = 1.2f;
    int count = (int)((z2 - z1) / seg);
    for (int i = 0; i < count; i++) {
        float cz = z1 + i * seg + seg * 0.5f;
        if (i % 2 == 0) setMaterial(0.95f, 0.08f, 0.08f);
        else setMaterial(0.98f, 0.98f, 0.98f);
        glPushMatrix();
        glTranslatef(x, 0.035f, cz);
        drawBox(width, 0.07f, seg);
        glPopMatrix();
    }
}

void drawGuardRailHorizontal(float x1, float x2, float z)
{
    setMaterial(0.72f, 0.74f, 0.76f, 50.0f);
    glPushMatrix(); glTranslatef((x1 + x2) * 0.5f, 0.55f, z); drawBox(x2 - x1, 0.07f, 0.09f); glPopMatrix();
    glPushMatrix(); glTranslatef((x1 + x2) * 0.5f, 0.90f, z); drawBox(x2 - x1, 0.07f, 0.09f); glPopMatrix();
    for (float x = x1; x <= x2; x += 1.5f) {
        glPushMatrix(); glTranslatef(x, 0.48f, z); drawBox(0.07f, 0.90f, 0.07f); glPopMatrix();
    }
}

void drawGuardRailVertical(float x, float z1, float z2)
{
    setMaterial(0.72f, 0.74f, 0.76f, 50.0f);
    glPushMatrix(); glTranslatef(x, 0.55f, (z1 + z2) * 0.5f); drawBox(0.09f, 0.07f, z2 - z1); glPopMatrix();
    glPushMatrix(); glTranslatef(x, 0.90f, (z1 + z2) * 0.5f); drawBox(0.09f, 0.07f, z2 - z1); glPopMatrix();
    for (float z = z1; z <= z2; z += 1.5f) {
        glPushMatrix(); glTranslatef(x, 0.48f, z); drawBox(0.07f, 0.90f, 0.07f); glPopMatrix();
    }
}

void drawCone(float x, float z)
{
    glPushMatrix();
    glTranslatef(x, 0.02f, z);
    setMaterial(1.0f, 0.35f, 0.05f);
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    glutSolidCone(0.27f, 0.75f, 28, 16);
    glPopMatrix();
    setMaterial(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glTranslatef(0, 0.23f, 0);
    drawBox(0.43f, 0.07f, 0.43f);
    glPopMatrix();
    glPopMatrix();
}

// FUNGSI BANTUAN: Untuk menggambar pola catur pada Garis Finish
void drawCheckeredLine(float minX, float maxX, float minZ, float maxZ)
{
    float size = 0.8f; // Ukuran kotak catur
    int xCount = (int)((maxX - minX) / size + 0.1f);
    int zCount = (int)((maxZ - minZ) / size + 0.1f);

    glBegin(GL_QUADS);
    for (int i = 0; i < xCount; i++) {
        for (int j = 0; j < zCount; j++) {
            if ((i + j) % 2 == 0) glColor3f(0.95f, 0.95f, 0.95f); // Putih
            else glColor3f(0.05f, 0.05f, 0.05f); // Hitam

            float x1 = minX + i * size;
            float x2 = x1 + size;
            float z1 = minZ + j * size;
            float z2 = z1 + size;

            glVertex3f(x1, 0.065f, z1);
            glVertex3f(x2, 0.065f, z1);
            glVertex3f(x2, 0.065f, z2);
            glVertex3f(x1, 0.065f, z2);
        }
    }
    glEnd();
}

// Kombinasi Finish Line (Catur) dan Checkpoint Progress (Biru Neon)
void drawCheckpointMarkers()
{
    glDisable(GL_LIGHTING);

    // ==========================================
    // 1. GARIS FINISH (Checkpoint 2 - Bawah)
    // ==========================================
    // Digambar lebih dulu tanpa blending transparan
    drawCheckeredLine(-0.8f, 0.8f, -16.0f, -4.8f);

    // ==========================================
    // 2. CHECKPOINT PROGRESS (0, 1, dan 3)
    // ==========================================
    // Menggunakan warna biru neon transparan hologram
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.1f, 0.8f, 1.0f, 0.45f);

    // Checkpoint 0 (Atas)
    glBegin(GL_QUADS);
        glVertex3f(-0.8f, 0.065f,  4.8f);
        glVertex3f( 0.8f, 0.065f,  4.8f);
        glVertex3f( 0.8f, 0.065f, 16.0f);
        glVertex3f(-0.8f, 0.065f, 16.0f);
    glEnd();

    // Checkpoint 1 (Kanan)
    glBegin(GL_QUADS);
        glVertex3f( 8.8f, 0.065f, -0.8f);
        glVertex3f(24.0f, 0.065f, -0.8f);
        glVertex3f(24.0f, 0.065f,  0.8f);
        glVertex3f( 8.8f, 0.065f,  0.8f);
    glEnd();

    // Checkpoint 3 (Kiri)
    glBegin(GL_QUADS);
        glVertex3f(-24.0f, 0.065f, -0.8f);
        glVertex3f( -8.8f, 0.065f, -0.8f);
        glVertex3f( -8.8f, 0.065f,  0.8f);
        glVertex3f(-24.0f, 0.065f,  0.8f);
    glEnd();

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

void drawTrack()
{
    setMaterial(0.33f, 0.54f, 0.26f);
    glPushMatrix(); glTranslatef(0, -0.08f, 0); drawBox(76.0f, 0.12f, 56.0f); glPopMatrix();

    setMaterial(0.18f, 0.19f, 0.21f);
    glBegin(GL_QUADS);
        glNormal3f(0, 1, 0);
        glVertex3f(OUTER_LEFT,  0.0f, OUTER_BOTTOM);
        glVertex3f(OUTER_RIGHT, 0.0f, OUTER_BOTTOM);
        glVertex3f(OUTER_RIGHT, 0.0f, OUTER_TOP);
        glVertex3f(OUTER_LEFT,  0.0f, OUTER_TOP);
    glEnd();

    setMaterial(0.31f, 0.53f, 0.23f);
    glBegin(GL_QUADS);
        glNormal3f(0, 1, 0);
        glVertex3f(INNER_LEFT,  0.02f, INNER_BOTTOM);
        glVertex3f(INNER_RIGHT, 0.02f, INNER_BOTTOM);
        glVertex3f(INNER_RIGHT, 0.02f, INNER_TOP);
        glVertex3f(INNER_LEFT,  0.02f, INNER_TOP);
    glEnd();

    drawCurbHorizontal(-24.0f, 24.0f, -16.0f, 0.55f);
    drawCurbHorizontal(-24.0f, 24.0f,  16.0f, 0.55f);
    drawCurbVertical(-24.0f, -16.0f, 16.0f, 0.55f);
    drawCurbVertical( 24.0f, -16.0f, 16.0f, 0.55f);

    drawCurbHorizontal(-8.8f, 8.8f, -4.8f, 0.50f);
    drawCurbHorizontal(-8.8f, 8.8f,  4.8f, 0.50f);
    drawCurbVertical(-8.8f, -4.8f, 4.8f, 0.50f);
    drawCurbVertical( 8.8f, -4.8f, 4.8f, 0.50f);

    glDisable(GL_LIGHTING);

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 4; j++) {
            if ((i + j) % 2 == 0) glColor3f(1.0f, 1.0f, 1.0f);
            else glColor3f(0.03f, 0.03f, 0.03f);

            float x1 = -15.0f + i * 0.70f;
            float x2 = x1 + 0.70f;
            float z1 = 11.7f + j * 0.35f;
            float z2 = z1 + 0.35f;

            glBegin(GL_QUADS);
                glVertex3f(x1, 0.055f, z1);
                glVertex3f(x2, 0.055f, z1);
                glVertex3f(x2, 0.055f, z2);
                glVertex3f(x1, 0.055f, z2);
            glEnd();
        }
    }

    glColor3f(1.0f, 1.0f, 0.88f);
    for (int i = -8; i <= 8; i++) {
        glBegin(GL_QUADS);
            glVertex3f(i * 2.2f - 0.7f, 0.05f,  9.0f); glVertex3f(i * 2.2f + 0.7f, 0.05f,  9.0f);
            glVertex3f(i * 2.2f + 0.7f, 0.05f,  9.25f); glVertex3f(i * 2.2f - 0.7f, 0.05f,  9.25f);
        glEnd();
        glBegin(GL_QUADS);
            glVertex3f(i * 2.2f - 0.7f, 0.05f, -9.25f); glVertex3f(i * 2.2f + 0.7f, 0.05f, -9.25f);
            glVertex3f(i * 2.2f + 0.7f, 0.05f, -9.0f); glVertex3f(i * 2.2f - 0.7f, 0.05f, -9.0f);
        glEnd();
    }

    glColor3f(0.04f, 0.04f, 0.04f);
    glLineWidth(3.0f);
    for (int k = 0; k < 3; k++) {
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i < 90; i++) {
            float t = i / 89.0f;
            float x = cos(t * M_PI * 1.7f) * 11.5f;
            float z = sin(t * M_PI * 1.7f) * 7.0f + k * 0.35f;
            glVertex3f(x, 0.06f, z);
        }
        glEnd();
    }
    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);

    drawCheckpointMarkers();

    drawGuardRailHorizontal(-25.5f, 25.5f, -17.7f); drawGuardRailHorizontal(-25.5f, 25.5f,  17.7f);
    drawGuardRailVertical(-25.7f, -17.7f, 17.7f); drawGuardRailVertical( 25.7f, -17.7f, 17.7f);

    drawGuardRailHorizontal(-9.6f, 9.6f, -5.7f); drawGuardRailHorizontal(-9.6f, 9.6f,  5.7f);
    drawGuardRailVertical(-9.7f, -5.7f, 5.7f); drawGuardRailVertical( 9.7f, -5.7f, 5.7f);

    drawCone(-7.0f,  7.8f); drawCone(-3.0f,  7.9f); drawCone( 3.0f,  7.9f); drawCone( 7.0f,  7.8f);
    drawCone(-7.0f, -7.8f); drawCone(-3.0f, -7.9f); drawCone( 3.0f, -7.9f); drawCone( 7.0f, -7.8f);
}

void drawMountains()
{
    setMaterial(0.17f, 0.36f, 0.20f);
    glBegin(GL_TRIANGLES);
        glNormal3f(0, 0, 1);
        glVertex3f(-34, 0, -25); glVertex3f(-25, 9, -25); glVertex3f(-16, 0, -25);
        glVertex3f(-18, 0, -25.5f); glVertex3f(-8,  7, -25.5f); glVertex3f(2,   0, -25.5f);
        glVertex3f(-2, 0, -25); glVertex3f(8,  8, -25); glVertex3f(18, 0, -25);
        glVertex3f(13, 0, -25.5f); glVertex3f(23, 6, -25.5f); glVertex3f(33, 0, -25.5f);
    glEnd();
}

#endif