#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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
    // Warna langit sesuai mode
    if (nightMode)
        glClearColor(0.04f, 0.05f, 0.10f, 1.0f);
    else
        glClearColor(0.55f, 0.74f, 0.92f, 1.0f); // biru langit siang

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update pencahayaan tiap frame sesuai nightMode
    updateLighting();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float rad = carAngle * M_PI / 180.0f;

    if (cameraMode == 0)
    {
        // Kamera isometrik tetap
        gluLookAt(0.0f, 24.0f, 26.0f,
                  0.0f,  0.0f,  0.0f,
                  0.0f,  1.0f,  0.0f);
    }
    else if (cameraMode == 1)
    {
        // Kamera chase (3rd person)
        float camX = carX + cosf(rad) * 9.5f;
        float camZ = carZ - sinf(rad) * 9.5f;
        gluLookAt(camX,  5.8f, camZ,
                  carX,  0.9f, carZ,
                  0.0f,  1.0f, 0.0f);
    }
    else
    {
        // Kamera top-down
        gluLookAt(0.0f, 35.0f, 0.1f,
                  0.0f,  0.0f, 0.0f,
                  0.0f,  0.0f, -1.0f);
    }

    // --- Bintang malam (digambar sebelum objek lain) ---
    drawNightSky();

    // --- World ---
    drawTrack();
    drawMountains();
    drawSkidMarks();
    drawSmoke();
    drawCar();

    // --- 2D Overlay (urutan penting: HUD → confetti → victory) ---
    drawText();
    drawConfettiOverlay();
    drawVictoryScreen();

    glutSwapBuffers();
}

void reshape(int w, int h)
{
    if (h == 0) h = 1;
    float ratio = (float)w / h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50.0f, ratio, 0.5f, 150.0f);
    glViewport(0, 0, w, h);
    glMatrixMode(GL_MODELVIEW);
}

// ==============================
// TIMER LOOP (~60 FPS)
// ==============================
void timer(int value)
{
    updateCar();
    smokeTime += 0.06f;
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

// ==============================
// MAIN
// ==============================
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1000, 550);
    glutCreateWindow("Pink Drift Racing – GTI B2");

    init();
    createTextures();
    resetScene();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(specialDown);
    glutSpecialUpFunc(specialUp);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);

    glutTimerFunc(0, timer, 0);
    glutMainLoop();
    return 0;
}