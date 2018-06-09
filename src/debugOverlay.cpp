struct DebugOverlay {
	bool active;
	int bottomLayer;

	MintSprite *info;

	void update();
};

void initDebugOverlay(DebugOverlay *newOverlay) {
	memset(newOverlay, 0, sizeof(DebugOverlay));
	newOverlay->bottomLayer = 1000;
}

void DebugOverlay::update() {
	DebugOverlay *dgb = this;

	if (keyIsJustReleased('`')) dgb->active = !dgb->active;

	if (dgb->active) {
		if (!dgb->info) {
			dgb->info = createMintSprite();
			dgb->info->setupEmpty(512, 512);
			dgb->info->layer = dgb->bottomLayer;
		}
			dgb->info->setText("Build %d\nActive Sprite: %d", engine->commitCount, engine->activeSprites);
	}
	if (!dgb->active) {
		if (dgb->info) {
			dgb->info->destroy();
			dgb->info = NULL;
		}
	}

}
