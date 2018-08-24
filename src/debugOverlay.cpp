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

		bool isDebug = false;
		bool isEarly = false;
		bool isInternal = false;
#ifdef SEMI_DEBUG
		isDebug = true;
#endif
#ifdef SEMI_EARLY
		isEarly = true;
#endif
#ifdef SEMI_INTERNAL
		isInternal = true;
#endif

		dgb->info->setText("Built %s UTC\nDev version: %d\nEarly version: %d\nInternal version: %d\nActive Sprite: %d\n", buildDateChar, isDebug, isEarly, isInternal, engine->activeSprites);
	}
	if (!dgb->active) {
		if (dgb->info) {
			dgb->info->destroy();
			dgb->info = NULL;
		}
	}

}
