#include "input_ui.h" // buat fungsi-fungsi input dan UI
#include "car_graphics.h" // buat fungsi gambar mobil sama efek visual
#include "environment.h" // buat fungsi gambar track, gunung, dll
#include "utils.h" // buat fungsi utilitas seperti setMaterial, drawBox, dll
#include "globals.h" // buat deklarasi variabel global seperti posisi mobil, kecepatan, skor, dll
#include <GL/glut.h> // buat fungsi OpenGL
#include <math.h> // buat fungsi matematika seperti sin, cos, dll
#include <cstdio> // buat fungsi sprintf untuk format teks 


// ==============================
// TEXT & UI SLIDER
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

    // Teks instruksi diupdate ke WASD
    glRasterPos2f(30, 490);
    const char* info = "W/S = speed   A/D = steer   SPACE = drift   C = camera   N = day/night   0 = reset";
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

    // --- MENGGAMBAR SLIDER UI ---
    // Background Slider (Abu-abu gelap)
    glColor3f(0.15f, 0.15f, 0.15f);
    glBegin(GL_QUADS);
        glVertex2f(800, 500); glVertex2f(950, 500);
        glVertex2f(950, 510); glVertex2f(800, 510);
    glEnd();

    // Kalkulasi Posisi Handle (Rentang speed: 0.5x sampai 2.0x)
    float t = (speedMultiplier - 0.5f) / 1.5f; 
    if (t < 0) t = 0; if (t > 1) t = 1;
    float handleX = 800 + t * 150;

    // Handle Slider (Kuning Emas)
    glColor3f(1.0f, 0.8f, 0.0f);
    glBegin(GL_QUADS);
        glVertex2f(handleX - 5, 495); glVertex2f(handleX + 5, 495);
        glVertex2f(handleX + 5, 515); glVertex2f(handleX - 5, 515);
    glEnd();

    // Teks Slider
    char slText[64];
    sprintf(slText, "Speed/Handling: %.1fx", speedMultiplier);
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(800, 525);
    for(int i = 0; slText[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, slText[i]);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_LIGHTING);
}

// ==============================
// KEYBOARD INPUT
// ==============================
void keyboardDown(unsigned char key, int x, int y)
{
    normalKey[key] = true;

    switch (key)
    {
        case '0': resetScene(); break;
        case 'c': case 'C':
            cameraMode++;
            if (cameraMode > 2) cameraMode = 0;
            break;
        case 'n': case 'N': nightMode = !nightMode; break;
        case 27: exit(0); break;
    }
    glutPostRedisplay();
}

void keyboardUp(unsigned char key, int x, int y) { normalKey[key] = false; }
void specialDown(int key, int x, int y) { specialKey[key] = true; }
void specialUp(int key, int x, int y) { specialKey[key] = false; }

// ==============================
// MOUSE INPUT (UNTUK DRAG SLIDER)
// ==============================
void mouseButton(int button, int state, int x, int y)
{
    // Konversi koordinat asli layar ke skala Ortho 1000x550
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    float orthoX = (float)x / w * 1000.0f;
    float orthoY = 550.0f - ((float)y / h * 550.0f);

    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            // Cek apakah klik berada di dalam area hitbox slider
            if (orthoX >= 780 && orthoX <= 970 && orthoY >= 480 && orthoY <= 530)
            {
                isDraggingSlider = true;
                float t = (orthoX - 800.0f) / 150.0f;
                if (t < 0.0f) t = 0.0f;
                if (t > 1.0f) t = 1.0f;
                speedMultiplier = 0.5f + t * 1.5f;
            }
        }
        else if (state == GLUT_UP)
        {
            isDraggingSlider = false;
        }
    }
}

void mouseMotion(int x, int y)
{
    if (isDraggingSlider)
    {
        int w = glutGet(GLUT_WINDOW_WIDTH);
        float orthoX = (float)x / w * 1000.0f;

        float t = (orthoX - 800.0f) / 150.0f;
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;
        speedMultiplier = 0.5f + t * 1.5f; // Set nilai 0.5x sampai 2.0x
    }
}