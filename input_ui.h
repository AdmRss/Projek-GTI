#ifndef INPUT_UI_H
#define INPUT_UI_H

// --- Helper: render teks di koordinat ortho ---
void renderString(float x, float y, void* font, const char* str);

// --- HUD & Overlay UI ---
void drawText();
void drawVictoryScreen();
void drawConfettiOverlay();

// --- Input handlers ---
void keyboardDown(unsigned char key, int x, int y);
void keyboardUp  (unsigned char key, int x, int y);
void specialDown (int key, int x, int y);
void specialUp   (int key, int x, int y);

// --- Mouse ---
void mouseButton(int button, int state, int x, int y);
void mouseMotion (int x, int y);

#endif