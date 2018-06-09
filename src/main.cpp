#define SEMI_SOUND_NEW

#include "gameEngine.h"
#include "debugOverlay.cpp"
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

#if 0
	MintSprite *bg = createMintSprite();
	bg->setupRect(1, 1, 0x000000);
	bg->alpha = 0;
	bg->scale(engine->width, engine->height);

	MintSprite *spr = createMintSprite();
	spr->setupContainer(engine->width, engine->height);
#else
	MintSprite *spr = createMintSprite();
	spr->setupRect(engine->width, engine->height, 0x000000);
#endif
	Writer::initWriter(spr);
}

void updateMain() {
	Writer::updateWriter();
}

void exitMain() {
	Writer::deinitWriter();
}
