#define TEXT_AREA_MODES_LIMIT 16

enum TextAreaMode {
	TEXT_MODE_NONE,
	TEXT_MODE_JIGGLE,
	TEXT_MODE_ZOOM_OUT,
};

struct TextArea {
	bool exists;
	MintSprite *sprite;

	CharRenderDef defs[HUGE_STR];
	int defsNum;
	int textWidth;
	int textHeight;
	const char *defaultFont;
	int fontColour;

	TextAreaMode modes[TEXT_AREA_MODES_LIMIT];
	int modesNum;
	float modeStartTime;

	int jiggleX;
	int jiggleY;
	float zoomTime;

	void resize(int width, int height);
	void setFont(const char *fontName);
	void setText(const char *text);
	void addMode(TextAreaMode mode);
	void resetModes();
	void update();
};

void initTextArea(TextArea *area) {
	memset(area, 0, sizeof(TextArea));
	area->exists = true;
	area->fontColour = 0xFFFFFFFF;
}

void TextArea::resize(int width, int height) {
	TextArea *area = this;

	if (area->sprite) area->sprite->destroy();
	area->sprite = createMintSprite();
	area->sprite->setupEmpty(width, height);
}

void TextArea::setFont(const char *fontName) {
	TextArea *area = this;

	Asset *fontAsset = getBitmapFontAsset(fontName);
	if (!fontAsset) {
		printf("Font named %s not found\n", fontName);
		return;
	}

	area->defaultFont = fontAsset->name;
}

void TextArea::addMode(TextAreaMode mode) {
	TextArea *area = this;
	area->modes[area->modesNum++] = mode;

	if (mode == TEXT_MODE_JIGGLE) {
		area->jiggleX = 0;
		area->jiggleY = 0;
	} else if (mode == TEXT_MODE_ZOOM_OUT) {
		area->zoomTime = 1;
	}
}

void TextArea::resetModes() {
	TextArea *area = this;
	area->modesNum = 0;
}

void TextArea::setText(const char *text) {
	TextArea *area = this;

	FormatRegion regions[REGION_LIMIT];
	int regionsNum;
	char outText[HUGE_STR] = {};

	parseText(text, outText, regions, &regionsNum);
	getTextRects(outText, area->sprite->width, area->defaultFont, regions, regionsNum, area->defs, &area->defsNum, &area->textWidth, &area->textHeight);

	area->modeStartTime = engine->time;
}

void TextArea::update() {
	TextArea *area = this;
	MintSprite *sprite = area->sprite;

	sprite->clear();
	sprite->tint = area->fontColour;
	for (int i = 0; i < area->defsNum; i++) {
		CharRenderDef *def = &area->defs[i];
		if (def->glyph == ' ') continue;

		Rect sourceRect = def->sourceRect;
		Point destPoint = def->destPoint;
		Point scale = {1, 1};

		for (int modeI = 0; modeI < area->modesNum; modeI++) {
			TextAreaMode mode = area->modes[modeI];
			if (mode == TEXT_MODE_JIGGLE) {
				destPoint.x += rndInt(-area->jiggleX, area->jiggleX);
				destPoint.y += rndInt(-area->jiggleY, area->jiggleY);
			} else if (mode == TEXT_MODE_ZOOM_OUT) {
				float charPerc = mathClampMap(engine->time, area->modeStartTime, area->modeStartTime + zoomTime, 0, area->defsNum);
				if (charPerc > i+1) {
					continue;
				} else if (charPerc < i) {
					scale.x = 0;
					scale.y = 0;
				} else {
					float perc = charPerc - i;

					float scaleFactor = mathLerp(perc, 3, 1);
					scale.x = scaleFactor;
					scale.y = scaleFactor;

					float sizeDiffX = sourceRect.width*scale.x - sourceRect.width;
					float sizeDiffY = sourceRect.height*scale.y - sourceRect.height;
					destPoint.x -= sizeDiffX/2;
					destPoint.y -= sizeDiffY/2;
				}
			}
		}

		sprite->drawPixelsFromAsset(def->sourceTextureAsset, sourceRect.x, sourceRect.y, sourceRect.width, sourceRect.height, destPoint.x, destPoint.y, scale.x, scale.y);
	}
}
