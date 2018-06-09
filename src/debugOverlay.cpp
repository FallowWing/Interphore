struct DebugOverlay {
	bool active;
	int bottomLayer;

	MintSprite *versionTf;

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
		if (!dgb->versionTf) {
			dgb->versionTf = createMintSprite();
			dgb->versionTf->setupEmpty(512, 256);
			dgb->versionTf->layer = dgb->bottomLayer;
			dgb->versionTf->setText("Build %d\n", engine->commitCount);
		}
	}
	if (!dgb->active) {
		if (dgb->versionTf) {
			dgb->versionTf->destroy();
			dgb->versionTf = NULL;
		}
	}

}
