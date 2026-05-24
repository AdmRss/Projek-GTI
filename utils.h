#ifndef UTILS_H
#define UTILS_H

#include <GL/glut.h>

// Menggambar kotak solid berskala
void drawBox(float sx, float sy, float sz);

// Mengatur material OpenGL (warna + shininess)
void setMaterial(float r, float g, float b, float shininess = 30.0f);

// Cek posisi dalam rectangle
bool isInsideRect(float x, float z, float left, float right, float bottom, float top);

// Cek apakah posisi berada di dalam lintasan yang valid
bool isOnTrack(float x, float z);

// Clamp posisi ke dalam track dan kembalikan normal dinding yang tersentuh
// nx/nz: normal dinding (0 jika tidak menabrak)
bool clampToTrack(float &x, float &z, float &nx, float &nz);

// Reset skid marks
void resetSkidMarks();

// Reset seluruh scene ke posisi awal
void resetScene();

// Reset hanya race (posisi + timer, highscore tetap)
void restartRace();

// Inisialisasi OpenGL dasar
void init();

// Update pencahayaan berdasarkan nightMode (dipanggil tiap frame)
void updateLighting();

// Buat tekstur prosedural dan simpan ke texAsphalt, texGrass, texConcrete
void createTextures();

// Spawn partikel confetti di layar
void spawnConfetti();

// Update posisi confetti tiap frame
void updateConfetti();

#endif