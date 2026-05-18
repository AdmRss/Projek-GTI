#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

// ==============================
// TRACK
// ==============================
void drawCurbHorizontal(float x1, float x2, float z, float depth)
{
    float seg = 1.2f;
    int count = (int)((x2 - x1) / seg);

    for (int i = 0; i < count; i++)
    {
        float cx = x1 + i * seg + seg * 0.5f;

        if (i % 2 == 0)
            setMaterial(0.95f, 0.08f, 0.08f);
        else
            setMaterial(0.98f, 0.98f, 0.98f);

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

    for (int i = 0; i < count; i++)
    {
        float cz = z1 + i * seg + seg * 0.5f;

        if (i % 2 == 0)
            setMaterial(0.95f, 0.08f, 0.08f);
        else
            setMaterial(0.98f, 0.98f, 0.98f);

        glPushMatrix();
        glTranslatef(x, 0.035f, cz);
        drawBox(width, 0.07f, seg);
        glPopMatrix();
    }
}

void drawGuardRailHorizontal(float x1, float x2, float z)
{
    setMaterial(0.72f, 0.74f, 0.76f, 50.0f);

    glPushMatrix();
    glTranslatef((x1 + x2) * 0.5f, 0.55f, z);
    drawBox(x2 - x1, 0.07f, 0.09f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef((x1 + x2) * 0.5f, 0.90f, z);
    drawBox(x2 - x1, 0.07f, 0.09f);
    glPopMatrix();

    for (float x = x1; x <= x2; x += 1.5f)
    {
        glPushMatrix();
        glTranslatef(x, 0.48f, z);
        drawBox(0.07f, 0.90f, 0.07f);
        glPopMatrix();
    }
}

void drawGuardRailVertical(float x, float z1, float z2)
{
    setMaterial(0.72f, 0.74f, 0.76f, 50.0f);

    glPushMatrix();
    glTranslatef(x, 0.55f, (z1 + z2) * 0.5f);
    drawBox(0.09f, 0.07f, z2 - z1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(x, 0.90f, (z1 + z2) * 0.5f);
    drawBox(0.09f, 0.07f, z2 - z1);
    glPopMatrix();

    for (float z = z1; z <= z2; z += 1.5f)
    {
        glPushMatrix();
        glTranslatef(x, 0.48f, z);
        drawBox(0.07f, 0.90f, 0.07f);
        glPopMatrix();
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

void drawCheckpointMarkers()
{
    glDisable(GL_LIGHTING);

    glColor3f(0.1f, 0.8f, 1.0f);

    // checkpoint kanan
    glBegin(GL_QUADS);
        glVertex3f(17.8f, 0.065f, -2.0f);
        glVertex3f(19.2f, 0.065f, -2.0f);
        glVertex3f(19.2f, 0.065f,  2.0f);
        glVertex3f(17.8f, 0.065f,  2.0f);
    glEnd();

    // checkpoint bawah
    glBegin(GL_QUADS);
        glVertex3f(-2.0f, 0.065f, -10.2f);
        glVertex3f( 2.0f, 0.065f, -10.2f);
        glVertex3f( 2.0f, 0.065f,  -8.8f);
        glVertex3f(-2.0f, 0.065f,  -8.8f);
    glEnd();

    // checkpoint kiri
    glBegin(GL_QUADS);
        glVertex3f(-19.2f, 0.065f, -2.0f);
        glVertex3f(-17.8f, 0.065f, -2.0f);
        glVertex3f(-17.8f, 0.065f,  2.0f);
        glVertex3f(-19.2f, 0.065f,  2.0f);
    glEnd();

    glEnable(GL_LIGHTING);
}

void drawTrack()
{
    // rumput
    setMaterial(0.33f, 0.54f, 0.26f);
    glPushMatrix();
    glTranslatef(0, -0.08f, 0);
    drawBox(76.0f, 0.12f, 56.0f);
    glPopMatrix();

    // aspal luar
    setMaterial(0.18f, 0.19f, 0.21f);
    glBegin(GL_QUADS);
        glNormal3f(0, 1, 0);
        glVertex3f(OUTER_LEFT,  0.0f, OUTER_BOTTOM);
        glVertex3f(OUTER_RIGHT, 0.0f, OUTER_BOTTOM);
        glVertex3f(OUTER_RIGHT, 0.0f, OUTER_TOP);
        glVertex3f(OUTER_LEFT,  0.0f, OUTER_TOP);
    glEnd();

    // island tengah
    setMaterial(0.31f, 0.53f, 0.23f);
    glBegin(GL_QUADS);
        glNormal3f(0, 1, 0);
        glVertex3f(INNER_LEFT,  0.02f, INNER_BOTTOM);
        glVertex3f(INNER_RIGHT, 0.02f, INNER_BOTTOM);
        glVertex3f(INNER_RIGHT, 0.02f, INNER_TOP);
        glVertex3f(INNER_LEFT,  0.02f, INNER_TOP);
    glEnd();

    // curb luar
    drawCurbHorizontal(-24.0f, 24.0f, -16.0f, 0.55f);
    drawCurbHorizontal(-24.0f, 24.0f,  16.0f, 0.55f);
    drawCurbVertical(-24.0f, -16.0f, 16.0f, 0.55f);
    drawCurbVertical( 24.0f, -16.0f, 16.0f, 0.55f);

    // curb dalam
    drawCurbHorizontal(-8.8f, 8.8f, -4.8f, 0.50f);
    drawCurbHorizontal(-8.8f, 8.8f,  4.8f, 0.50f);
    drawCurbVertical(-8.8f, -4.8f, 4.8f, 0.50f);
    drawCurbVertical( 8.8f, -4.8f, 4.8f, 0.50f);

    glDisable(GL_LIGHTING);

    // start finish
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if ((i + j) % 2 == 0)
                glColor3f(1.0f, 1.0f, 1.0f);
            else
                glColor3f(0.03f, 0.03f, 0.03f);

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

    // garis lane
    glColor3f(1.0f, 1.0f, 0.88f);
    for (int i = -8; i <= 8; i++)
    {
        glBegin(GL_QUADS);
            glVertex3f(i * 2.2f - 0.7f, 0.05f,  9.0f);
            glVertex3f(i * 2.2f + 0.7f, 0.05f,  9.0f);
            glVertex3f(i * 2.2f + 0.7f, 0.05f,  9.25f);
            glVertex3f(i * 2.2f - 0.7f, 0.05f,  9.25f);
        glEnd();

        glBegin(GL_QUADS);
            glVertex3f(i * 2.2f - 0.7f, 0.05f, -9.25f);
            glVertex3f(i * 2.2f + 0.7f, 0.05f, -9.25f);
            glVertex3f(i * 2.2f + 0.7f, 0.05f, -9.0f);
            glVertex3f(i * 2.2f - 0.7f, 0.05f, -9.0f);
        glEnd();
    }

    // skid mark dekoratif
    glColor3f(0.04f, 0.04f, 0.04f);
    glLineWidth(3.0f);

    for (int k = 0; k < 3; k++)
    {
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i < 90; i++)
        {
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

    // guard rail luar
    drawGuardRailHorizontal(-25.5f, 25.5f, -17.7f);
    drawGuardRailHorizontal(-25.5f, 25.5f,  17.7f);
    drawGuardRailVertical(-25.7f, -17.7f, 17.7f);
    drawGuardRailVertical( 25.7f, -17.7f, 17.7f);

    // guard rail dalam
    drawGuardRailHorizontal(-9.6f, 9.6f, -5.7f);
    drawGuardRailHorizontal(-9.6f, 9.6f,  5.7f);
    drawGuardRailVertical(-9.7f, -5.7f, 5.7f);
    drawGuardRailVertical( 9.7f, -5.7f, 5.7f);

    // cone
    drawCone(-7.0f,  7.8f);
    drawCone(-3.0f,  7.9f);
    drawCone( 3.0f,  7.9f);
    drawCone( 7.0f,  7.8f);

    drawCone(-7.0f, -7.8f);
    drawCone(-3.0f, -7.9f);
    drawCone( 3.0f, -7.9f);
    drawCone( 7.0f, -7.8f);
}

void drawMountains()
{
    setMaterial(0.17f, 0.36f, 0.20f);

    glBegin(GL_TRIANGLES);
        glNormal3f(0, 0, 1);

        glVertex3f(-34, 0, -25);
        glVertex3f(-25, 9, -25);
        glVertex3f(-16, 0, -25);

        glVertex3f(-18, 0, -25.5f);
        glVertex3f(-8,  7, -25.5f);
        glVertex3f(2,   0, -25.5f);

        glVertex3f(-2, 0, -25);
        glVertex3f(8,  8, -25);
        glVertex3f(18, 0, -25);

        glVertex3f(13, 0, -25.5f);
        glVertex3f(23, 6, -25.5f);
        glVertex3f(33, 0, -25.5f);
    glEnd();
}

#endif