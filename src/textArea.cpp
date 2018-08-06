struct TextArea {
	bool exists;
	MintSprite *ref;
	MintSprite *sprite;
};

void initTextArea(TextArea *area, int width, int height) {
	memset(area, 0, sizeof(TextArea));
	area->exists = true;

	area->ref = createMintSprite();
	area->ref->setupEmpty(width, height);
	// area->ref->visible = false;

	area->sprite = createMintSprite();
	area->sprite->setupEmpty(width, height);
}
