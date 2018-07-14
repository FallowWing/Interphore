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
			dgb->info->setupEmpty(1024, 512);
		}
		dgb->info->layer = dgb->bottomLayer;
		dgb->info->tint = 0xFFFFFFFF;

		char buildDateChar[32] = {};
		getBuildDate(buildDateChar);
		dgb->info->setText("Built %s UTC\nActive Sprite: %d", buildDateChar, engine->activeSprites);
	}
	if (!dgb->active) {
		if (dgb->info) {
			dgb->info->destroy();
			dgb->info = NULL;
		}
	}

}
