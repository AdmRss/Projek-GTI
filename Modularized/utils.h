#ifndef UTILS_H
#define UTILS_H

void drawBox(float sx, float sy, float sz);
void setMaterial(float r, float g, float b, float shininess = 30.0f);
bool isInsideRect(float x, float z, float left, float right, float bottom, float top);
bool isOnTrack(float x, float z);
void resetSkidMarks();
void resetScene();
void init();
#endif