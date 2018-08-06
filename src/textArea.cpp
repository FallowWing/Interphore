struct TextArea {
	bool exists;
	MintSprite *sprite;

	CharRenderDef defs[HUGE_STR];
	int defsNum;
	int textWidth;
	int textHeight;

	void setText(const char *text);
};

void initTextArea(TextArea *area, int width, int height) {
	memset(area, 0, sizeof(TextArea));
	area->exists = true;

	// area->ref = createMintSprite();
	// area->ref->setupEmpty(width, height);
	// area->ref->visible = false;

	area->sprite = createMintSprite();
	area->sprite->setupEmpty(width, height);
}

void TextArea::setText(const char *text) {
	TextArea *area = this;

	// FormatRegion regions[REGION_LIMIT];
	// int regionsNum;

	// char outText[HUGE_STR];

	// parseText(text, outText, regions, &regionsNum);

	// getTextRects(outText, area->sprite->width, NULL, regions, regionsNum, area->defs, &area->defsNum, &area->textWidth, &area->textHeight);
}
