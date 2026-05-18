#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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
};

const int MAX_SKID = 600;
SkidMark skidMarks[MAX_SKID];
int skidIndex = 0;
int skidFrame = 0;

// ==============================
// UTIL
// ==============================
void drawBox(float sx, float sy, float sz)
{
    glPushMatrix();
    glScalef(sx, sy, sz);
    glutSolidCube(1.0);
    glPopMatrix();
}

void setMaterial(float r, float g, float b, float shininess = 30.0f)
{
    GLfloat diffuse[]  = { r, g, b, 1.0f };
    GLfloat ambient[]  = { r * 0.35f, g * 0.35f, b * 0.35f, 1.0f };
    GLfloat specular[] = { 0.38f, 0.38f, 0.38f, 1.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

bool isInsideRect(float x, float z, float left, float right, float bottom, float top)
{
    return (x > left && x < right && z > bottom && z < top);
}

bool isOnTrack(float x, float z)
{
    bool insideOuter = isInsideRect(
        x, z,
        OUTER_LEFT + TRACK_MARGIN,
        OUTER_RIGHT - TRACK_MARGIN,
        OUTER_BOTTOM + TRACK_MARGIN,
        OUTER_TOP - TRACK_MARGIN
    );

    bool insideInner = isInsideRect(
        x, z,
        INNER_LEFT - TRACK_MARGIN,
        INNER_RIGHT + TRACK_MARGIN,
        INNER_BOTTOM - TRACK_MARGIN,
        INNER_TOP + TRACK_MARGIN
    );

    return insideOuter && !insideInner;
}

void resetSkidMarks()
{
    for (int i = 0; i < MAX_SKID; i++)
    {
        skidMarks[i].active = false;
        skidMarks[i].x = 0.0f;
        skidMarks[i].z = 0.0f;
        skidMarks[i].angle = 0.0f;
    }

    skidIndex = 0;
    skidFrame = 0;
}

void resetScene()
{
    carX = -18.0f;
    carZ = 10.0f;
    carAngle = 180.0f;
    carSpeed = 0.0f;

    wheelRot = 0.0f;
    smokeTime = 0.0f;

    steerVisualAngle = 0.0f;
    carYawVel = 0.0f;
    driftSlipAngle = 0.0f;

    isDrifting = false;

    driftScore = 0.0f;
    comboMultiplier = 1.0f;
    driftFrames = 0;

    lapCount = 0;
    nextCheckpoint = 0;
    lapStarted = false;
    checkpointCooldown = 0;

    resetSkidMarks();
}

// ==============================
// INIT
// ==============================
void init()
{
    glClearColor(0.56f, 0.74f, 0.56f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    GLfloat lightPos[] = { -7.0f, 13.0f, 9.0f, 1.0f };
    GLfloat lightAmb[] = { 0.35f, 0.35f, 0.35f, 1.0f };
    GLfloat lightDif[] = { 0.90f, 0.90f, 0.90f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDif);
}

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

// ==============================
// SKID MARK DINAMIS
// ==============================
void addSkidMark(float localZ)
{
    float rad = carAngle * M_PI / 180.0f;

    float rearLocalX = 1.25f;

    float worldX = carX + rearLocalX * cos(rad) + localZ * sin(rad);
    float worldZ = carZ - rearLocalX * sin(rad) + localZ * cos(rad);

    skidMarks[skidIndex].x = worldX;
    skidMarks[skidIndex].z = worldZ;
    skidMarks[skidIndex].angle = carAngle;
    skidMarks[skidIndex].active = true;

    skidIndex++;
    if (skidIndex >= MAX_SKID)
        skidIndex = 0;
}

void updateSkidMarks()
{
    if (isDrifting)
    {
        skidFrame++;

        if (skidFrame % 3 == 0)
        {
            addSkidMark(0.72f);
            addSkidMark(-0.72f);
        }
    }
}

void drawSkidMarks()
{
    glDisable(GL_LIGHTING);
    glColor3f(0.015f, 0.015f, 0.015f);

    for (int i = 0; i < MAX_SKID; i++)
    {
        if (!skidMarks[i].active)
            continue;

        glPushMatrix();
        glTranslatef(skidMarks[i].x, 0.075f, skidMarks[i].z);
        glRotatef(skidMarks[i].angle, 0, 1, 0);
        drawBox(0.16f, 0.01f, 0.55f);
        glPopMatrix();
    }

    glEnable(GL_LIGHTING);
}

// ==============================
// MOBIL
// ==============================
void drawSmoke()
{
    if (!isDrifting && fabs(carSpeed) < 0.10f)
        return;

    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushMatrix();
    glTranslatef(carX, 0.18f, carZ);
    glRotatef(carAngle, 0, 1, 0);

    int smokeCount = isDrifting ? 22 : 7;

    for (int i = 0; i < smokeCount; i++)
    {
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

    if (steerable)
        glRotatef(steerVisualAngle, 0, 1, 0);

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
    for (int i = 0; i < 8; i++)
    {
        float a = i * 2.0f * M_PI / 8.0f;
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(cos(a) * 0.20f, sin(a) * 0.20f, 0.0f);
    }
    glEnd();

    glEnable(GL_LIGHTING);
    glPopMatrix();
}

void drawCarBody()
{
    bool braking = specialKey[GLUT_KEY_DOWN];

    // body utama
    setMaterial(1.0f, 0.02f, 0.55f, 65.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.38f, 0.0f);
    drawBox(3.95f, 0.52f, 1.60f);
    glPopMatrix();

    // bumper depan
    setMaterial(0.95f, 0.01f, 0.48f, 55.0f);
    glPushMatrix();
    glTranslatef(-2.06f, 0.25f, 0.0f);
    drawBox(0.22f, 0.33f, 1.68f);
    glPopMatrix();

    // bumper belakang
    glPushMatrix();
    glTranslatef(2.06f, 0.27f, 0.0f);
    drawBox(0.22f, 0.34f, 1.64f);
    glPopMatrix();

    // side skirt
    setMaterial(0.04f, 0.04f, 0.05f, 45.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.15f, 0.90f);
    drawBox(3.75f, 0.16f, 0.12f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 0.15f, -0.90f);
    drawBox(3.75f, 0.16f, 0.12f);
    glPopMatrix();

    // kap depan hitam
    setMaterial(0.02f, 0.02f, 0.025f, 70.0f);
    glPushMatrix();
    glTranslatef(-1.02f, 0.66f, 0.0f);
    drawBox(1.38f, 0.08f, 1.28f);
    glPopMatrix();

    // ventilasi kap
    setMaterial(0.0f, 0.0f, 0.0f, 25.0f);
    glPushMatrix();
    glTranslatef(-1.05f, 0.72f, -0.28f);
    drawBox(0.46f, 0.03f, 0.12f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1.05f, 0.72f, 0.28f);
    drawBox(0.46f, 0.03f, 0.12f);
    glPopMatrix();

    // bagasi
    setMaterial(1.0f, 0.02f, 0.55f, 65.0f);
    glPushMatrix();
    glTranslatef(1.20f, 0.64f, 0.0f);
    drawBox(0.92f, 0.12f, 1.40f);
    glPopMatrix();

    // kabin
    glPushMatrix();
    glTranslatef(0.26f, 0.90f, 0.0f);
    drawBox(1.82f, 0.38f, 1.28f);
    glPopMatrix();

    // atap
    glPushMatrix();
    glTranslatef(0.34f, 1.17f, 0.0f);
    drawBox(1.10f, 0.12f, 1.12f);
    glPopMatrix();

    // kaca biru muda
    setMaterial(0.42f, 0.83f, 1.0f, 95.0f);

    glPushMatrix();
    glTranslatef(-0.55f, 1.01f, 0.0f); 
    glRotatef(-35.0f, 0, 0, 1);
    drawBox(0.12f, 0.65f, 1.25f); 
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.92f, 1.01f, 0.0f);
    glRotatef(30.0f, 0, 0, 1);
    drawBox(0.12f, 0.52f, 1.15f);
    glPopMatrix();

    for (int s = -1; s <= 1; s += 2)
    {
        glPushMatrix();
        glTranslatef(0.18f, 1.00f, s * 0.67f);
        drawBox(1.12f, 0.27f, 0.045f);
        glPopMatrix();
    }

    // pilar hitam
    setMaterial(0.06f, 0.06f, 0.07f, 40.0f);
    for (int s = -1; s <= 1; s += 2)
    {
        glPushMatrix();
        glTranslatef(-0.28f, 1.00f, s * 0.69f);
        drawBox(0.05f, 0.36f, 0.05f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0.34f, 1.00f, s * 0.69f);
        drawBox(0.05f, 0.36f, 0.05f);
        glPopMatrix();
    }

    // grille depan
    setMaterial(0.02f, 0.02f, 0.02f, 20.0f);
    glPushMatrix();
    glTranslatef(-2.16f, 0.35f, 0.0f);
    drawBox(0.05f, 0.22f, 0.74f);
    glPopMatrix();

    // lampu depan
    if (nightMode)
        setMaterial(1.0f, 1.0f, 0.55f, 120.0f);
    else
        setMaterial(0.96f, 0.88f, 0.42f, 95.0f);

    glPushMatrix();
    glTranslatef(-2.12f, 0.40f, -0.54f);
    drawBox(0.07f, 0.18f, 0.30f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2.12f, 0.40f, 0.54f);
    drawBox(0.07f, 0.18f, 0.30f);
    glPopMatrix();

    // lampu rem
    if (braking)
        setMaterial(1.0f, 0.0f, 0.0f, 120.0f);
    else
        setMaterial(0.75f, 0.02f, 0.02f, 80.0f);

    glPushMatrix();
    glTranslatef(2.12f, 0.38f, -0.50f);
    drawBox(0.07f, 0.18f, 0.30f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2.12f, 0.38f, 0.50f);
    drawBox(0.07f, 0.18f, 0.30f);
    glPopMatrix();

    // diffuser
    setMaterial(0.02f, 0.02f, 0.025f, 40.0f);
    glPushMatrix();
    glTranslatef(2.18f, 0.16f, 0.0f);
    drawBox(0.10f, 0.18f, 1.08f);
    glPopMatrix();

    // knalpot
    setMaterial(0.78f, 0.78f, 0.80f, 100.0f);
    glPushMatrix();
    glTranslatef(2.27f, 0.18f, -0.34f);
    glRotatef(90, 0, 1, 0);
    glutSolidTorus(0.025f, 0.08f, 14, 24);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2.27f, 0.18f, 0.34f);
    glRotatef(90, 0, 1, 0);
    glutSolidTorus(0.025f, 0.08f, 14, 24);
    glPopMatrix();

    // spoiler
    setMaterial(1.0f, 0.02f, 0.55f, 70.0f);
    glPushMatrix();
    glTranslatef(1.74f, 1.08f, 0.0f);
    drawBox(0.16f, 0.11f, 1.82f);
    glPopMatrix();

    setMaterial(0.04f, 0.04f, 0.05f, 45.0f);
    glPushMatrix();
    glTranslatef(1.65f, 0.87f, -0.60f);
    drawBox(0.07f, 0.42f, 0.07f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.65f, 0.87f, 0.60f);
    drawBox(0.07f, 0.42f, 0.07f);
    glPopMatrix();

    // stripe putih
    setMaterial(1.0f, 1.0f, 1.0f, 50.0f);
    glPushMatrix();
    glTranslatef(0.05f, 0.67f, 0.82f);
    drawBox(2.75f, 0.07f, 0.035f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.05f, 0.67f, -0.82f);
    drawBox(2.75f, 0.07f, 0.035f);
    glPopMatrix();
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

// ==============================
// CHECKPOINT / LAP
// ==============================
int getCheckpointZone()
{
    if (carZ > 8.2f && carX > -20.0f && carX < 20.0f)
        return 0; // atas / start

    if (carX > 17.5f && carZ > -12.0f && carZ < 12.0f)
        return 1; // kanan

    if (carZ < -8.2f && carX > -20.0f && carX < 20.0f)
        return 2; // bawah

    if (carX < -17.5f && carZ > -12.0f && carZ < 12.0f)
        return 3; // kiri

    return -1;
}

void updateLapSystem()
{
    if (checkpointCooldown > 0)
        checkpointCooldown--;

    int zone = getCheckpointZone();

    if (zone == nextCheckpoint && checkpointCooldown == 0)
    {
        if (nextCheckpoint == 0)
        {
            if (lapStarted)
                lapCount++;
            else
                lapStarted = true;
        }

        nextCheckpoint++;
        if (nextCheckpoint > 3)
            nextCheckpoint = 0;

        checkpointCooldown = 45;
    }
}

// ==============================
// TEXT
// ==============================
void drawText()
{
    glDisable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1000, 0, 550);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    if (isDrifting)
    {
        glColor3f(1.0f, 0.2f, 0.8f);
        glRasterPos2f(30, 515);
        const char* text = "SLOW DRIFT MODE!";
        for (int i = 0; text[i] != '\0'; i++)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    }

    glColor3f(1.0f, 1.0f, 1.0f);

    glRasterPos2f(30, 490);
    const char* info = "UP/DOWN = speed   LEFT/RIGHT = steer   SPACE = drift   C = camera   N = day/night   0 = reset";
    for (int i = 0; info[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, info[i]);

    int speedKmh = (int)(fabs(carSpeed) * 650.0f);

    char text1[160];
    sprintf(text1, "Speed: %d km/h   Drift Score: %.0f   Combo: x%.1f", speedKmh, driftScore, comboMultiplier);

    glRasterPos2f(30, 465);
    for (int i = 0; text1[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, text1[i]);

    char text2[160];
    sprintf(text2, "Lap: %d   Next Checkpoint: %d   Camera Mode: %d   Mode: %s",
            lapCount, nextCheckpoint + 1, cameraMode + 1, nightMode ? "Night" : "Day");

    glRasterPos2f(30, 440);
    for (int i = 0; text2[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, text2[i]);

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_LIGHTING);
}

// ==============================
// DISPLAY
// ==============================
void display()
{
    if (nightMode)
        glClearColor(0.04f, 0.06f, 0.10f, 1.0f);
    else
        glClearColor(0.56f, 0.74f, 0.56f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float rad = carAngle * M_PI / 180.0f;

    if (cameraMode == 0)
    {
        gluLookAt(
            0.0f, 24.0f, 26.0f,
            0.0f,  0.0f,  0.0f,
            0.0f,  1.0f,  0.0f
        );
    }
    else if (cameraMode == 1)
    {
        float camX = carX + cos(rad) * 9.5f;
        float camY = 5.8f;
        float camZ = carZ - sin(rad) * 9.5f;

        gluLookAt(
            camX, camY, camZ,
            carX, 0.9f, carZ,
            0.0f, 1.0f, 0.0f
        );
    }
    else
    {
        gluLookAt(
            0.0f, 35.0f, 0.1f,
            0.0f,  0.0f, 0.0f,
            0.0f,  0.0f, -1.0f
        );
    }

    drawTrack();
    drawMountains();
    drawSkidMarks();
    drawSmoke();
    drawCar();
    drawText();

    glutSwapBuffers();
}

void reshape(int w, int h)
{
    if (h == 0)
        h = 1;

    float ratio = (float)w / h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50.0f, ratio, 1.0f, 100.0f);

    glViewport(0, 0, w, h);
    glMatrixMode(GL_MODELVIEW);
}

// ==============================
// INPUT
// ==============================
void keyboardDown(unsigned char key, int x, int y)
{
    normalKey[key] = true;

    switch (key)
    {
        case '0':
            resetScene();
            break;

        case 'c':
        case 'C':
            cameraMode++;
            if (cameraMode > 2)
                cameraMode = 0;
            break;

        case 'n':
        case 'N':
            nightMode = !nightMode;
            break;

        case 27:
            exit(0);
            break;
    }

    glutPostRedisplay();
}

void keyboardUp(unsigned char key, int x, int y)
{
    normalKey[key] = false;
}

void specialDown(int key, int x, int y)
{
    specialKey[key] = true;
}

void specialUp(int key, int x, int y)
{
    specialKey[key] = false;
}

// ==============================
// UPDATE MOBIL
// ==============================
void updateCar()
{
    const float maxForwardSpeed = 0.23f; // Top speed sedikit dinaikkan
    const float maxReverseSpeed = -0.08f;
    const float acceleration    = 0.0055f;
    const float brakePower      = 0.0075f;
    const float rollingFriction = 0.985f;

    if (specialKey[GLUT_KEY_UP]) {
        carSpeed += acceleration;
        if (carSpeed > maxForwardSpeed) carSpeed = maxForwardSpeed;
    } else if (specialKey[GLUT_KEY_DOWN]) {
        carSpeed -= brakePower;
        if (carSpeed < maxReverseSpeed) carSpeed = maxReverseSpeed;
    } else {
        carSpeed *= rollingFriction;
        if (fabs(carSpeed) < 0.001f) carSpeed = 0.0f;
    }

    float steerInput = 0.0f;
    if (specialKey[GLUT_KEY_LEFT])  steerInput =  1.0f;
    if (specialKey[GLUT_KEY_RIGHT]) steerInput = -1.0f;

    bool handbrake = normalKey[' '];
    float speedRatio = fabs(carSpeed) / maxForwardSpeed;
    if (speedRatio > 1.0f) speedRatio = 1.0f;

    // Logika Drifting
    bool canDrift = (fabs(carSpeed) > 0.06f);
    if (handbrake && canDrift) {
        isDrifting = true;
    } else if (!handbrake) {
        isDrifting = false; 
    }

    // Sistem Combo
    if (isDrifting) {
        driftFrames++;
        if (driftFrames > 240) comboMultiplier = 4.0f;
        else if (driftFrames > 160) comboMultiplier = 3.0f;
        else if (driftFrames > 80) comboMultiplier = 2.0f;
        else comboMultiplier = 1.0f;
    } else {
        driftFrames = 0;
        comboMultiplier = 1.0f;
    }

    // --- HANDLING & STEERING ---
    float targetSteerVisual = steerInput * 25.0f;
    steerVisualAngle += (targetSteerVisual - steerVisualAngle) * 0.15f;

    float turnPower = steerInput * (0.3f + speedRatio * 0.45f);
    if (carSpeed < 0.0f) turnPower = -turnPower * 0.65f;

    carYawVel += turnPower;
    carYawVel *= isDrifting ? 0.95f : 0.82f; // Saat drift, handling lebih licin (mudah oversteer)
    carAngle += carYawVel;

    // --- Lateral Inertia (Physics buat drift) ---
    static float lateralSpeed = 0.0f;

    if (isDrifting) {
        // Gaya sentrifugal melempar mobil ke samping berlawanan arah belok
        lateralSpeed += (carYawVel * speedRatio * 0.022f);
        // Batasi batas maksimal ngesot agar tidak melayang
        if (lateralSpeed >  0.085f) lateralSpeed =  0.085f;
        if (lateralSpeed < -0.085f) lateralSpeed = -0.085f;
    }

    // Friksi ban (Grip)
    float grip = isDrifting ? 0.985f : 0.85f; // Licin saat spasi ditahan, langsung nge-grip saat dilepas
    lateralSpeed *= grip;

    // Efek visual kemiringan bodi saat drift
    float targetSlip = isDrifting ? (lateralSpeed * -160.0f) : 0.0f;
    driftSlipAngle += (targetSlip - driftSlipAngle) * 0.12f;
    if (fabs(driftSlipAngle) < 0.1f) driftSlipAngle = 0.0f;

    // --- KALKULASI VEKTOR LINTASAN ---
    float headingRad = carAngle * M_PI / 180.0f; // Arah moncong mobil menghadap
    
    float forwardX = -cos(headingRad);
    float forwardZ =  sin(headingRad);
    float sideX = sin(headingRad);
    float sideZ = cos(headingRad);

    // Gerak Maju + Gerak Menyamping (Inertia) digabung!
    float nextX = carX + (forwardX * carSpeed) + (sideX * lateralSpeed);
    float nextZ = carZ + (forwardZ * carSpeed) + (sideZ * lateralSpeed);

    if (isDrifting) {
        carSpeed *= 0.995f; // Tenaga mesin berkurang saat ngesot
        driftScore += (fabs(carSpeed) * 6.0f + fabs(lateralSpeed) * 25.0f) * comboMultiplier;
    }

    // --- COLLISION (Deteksi Tabrakan) ---
    bool hitWall = false;

    if (isOnTrack(nextX, nextZ)) {
        carX = nextX;
        carZ = nextZ;
    } else if (isOnTrack(nextX, carZ)) {
        carX = nextX;
        carSpeed *= -0.2f; carYawVel *= 0.2f; lateralSpeed *= -0.5f; // Pantulan tembok
        hitWall = true;
    } else if (isOnTrack(carX, nextZ)) {
        carZ = nextZ;
        carSpeed *= -0.2f; carYawVel *= 0.2f; lateralSpeed *= -0.5f;
        hitWall = true;
    } else {
        carSpeed *= -0.25f; carYawVel *= 0.2f; lateralSpeed *= -0.5f;
        hitWall = true;
    }

    if (hitWall) {
        driftFrames = 0; comboMultiplier = 1.0f; isDrifting = false;
        driftSlipAngle *= 0.35f;
    }

    wheelRot += carSpeed * 220.0f;
    if (wheelRot > 360.0f) wheelRot -= 360.0f;
    if (wheelRot < -360.0f) wheelRot += 360.0f;

    updateSkidMarks();
    updateLapSystem();
}

// ==============================
// MAIN
// ==============================
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(1000, 550);
    glutCreateWindow("Pink Drift Racing Track - GTI B2");

    init();
    resetScene();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);

    glutSpecialFunc(specialDown);
    glutSpecialUpFunc(specialUp);

    glutTimerFunc(0, timer, 0);

    glutMainLoop();
    return 0;
}