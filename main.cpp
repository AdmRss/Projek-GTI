#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ==============================
// URUTAN MODUL (Unity Build)
// ==============================
#include "globals.h"
#include "utils.h"
#include "environment.h"
#include "car_graphics.h"
#include "physics.h"
#include "input_ui.h"

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
// MAIN LOOP & TIMER
// ==============================
void timer(int value)
{
    updateCar();
    smokeTime += 0.06f;

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

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

    // Mendaftarkan kontrol Mouse untuk Slider
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);

    glutTimerFunc(0, timer, 0);

    glutMainLoop();
    return 0;
}