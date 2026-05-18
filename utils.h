#ifndef UTILS_H
#define UTILS_H

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

#endif