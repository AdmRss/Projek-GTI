#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

// Kerb merah-putih horizontal/vertikal
void drawCurbHorizontal(float x1, float x2, float z, float depth);
void drawCurbVertical(float x, float z1, float z2, float width);

// Guard rail horizontal/vertikal
void drawGuardRailHorizontal(float x1, float x2, float z);
void drawGuardRailVertical(float x, float z1, float z2);

// Kerucut lalu lintas
void drawCone(float x, float z);

// Pola kotak-kotak untuk garis finish
void drawCheckeredLine(float minX, float maxX, float minZ, float maxZ);

// Marker checkpoint (garis finish + checkpoint biru neon)
void drawCheckpointMarkers();

// Gapura start/finish dengan pilar dan papan nama
void drawStartGantry();

// Tribune penonton
void drawGrandstand(float x, float z, float rotY);

// Pohon sederhana
void drawTree(float x, float z, float height);

// Track utama + environment
void drawTrack();

// Pegunungan latar belakang
void drawMountains();

// Langit malam (lingkaran bintang)
void drawNightSky();

#endif