#define SEMI_SOUND_NEW

#include "gameEngine.h"
#include "debugOverlay.cpp"
#include "textArea.cpp"
#include "game.cpp"

void updateMain();
void initMain();
void exitMain();
extern "C" void entryPoint();

#ifdef SEMI_WIN32
INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
#else
int main(int argc, char **argv) {
	platformArgC = argc;
	platformArgV = argv;
#endif
	entryPoint();
	return 0;
}

void entryPoint() {
	initEngine(initMain, updateMain);
}

void initMain() {
	engine->exitCallback = exitMain;

	initGame();
}

void updateMain() {
	updateGame();
}

void exitMain() {
	deinitGame();
}
