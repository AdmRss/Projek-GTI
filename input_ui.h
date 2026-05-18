#ifndef INPUT_UI_H
#define INPUT_UI_H

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

#endif