#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

// Deklarasi semua fungsi environment
void drawCurbHorizontal(float x1, float x2, float z, float depth);
void drawCurbVertical(float x, float z1, float z2, float width);
void drawGuardRailHorizontal(float x1, float x2, float z);
void drawGuardRailVertical(float x, float z1, float z2);
void drawCone(float x, float z);
void drawCheckeredLine(float minX, float maxX, float minZ, float maxZ);
void drawCheckpointMarkers();
void drawTrack();
void drawMountains();

#endif