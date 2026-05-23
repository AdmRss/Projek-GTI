#ifndef INPUT_UI_H
#define INPUT_UI_H

// extern = artinya variabel ini dideklarasikan di tempat lain (globals.h) tapi bisa dipakai di sini
extern bool normalKey[256]; // extern karena dideklarasikan di globals.h
extern bool specialKey[256]; // extern karena dideklarasikan di globals.h

void drawText();
void keyboardDown(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void specialDown(int key, int x, int y);
void specialUp(int key, int x, int y);
void mouseButton(int button, int state, int x, int y);
void mouseMotion(int x, int y);

#endif