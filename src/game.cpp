// THIS FILE IS A TEST RE-WRITE OF THE GAME, NOTHING IN HERE IS IN USE CURRENTLY

#define PASSAGE_MAX 1024
#define IMAGES_MAX 256
#define AUDIOS_MAX 256
#define STREAM_MAX 256
#define ASSETS_MAX 256

#define MAIN_TEXT_LAYER 20

void initGame(MintSprite *bgSpr);
void deinitGame();
void updateGame();

namespace Writer {
	void initWriter(MintSprite *bgSpr) { initGame(bgSpr); }
	void deinitWriter() { deinitGame(); }
	void updateWriter() { updateGame(); }
}

//
//
//         LEGACY END
//
//

enum GameState {
	STATE_NULL=0,
	STATE_PASSAGE,
	STATE_GRAPH
};

struct Passage {
	String *name;
	String *data;
};

struct Game {
	MintSprite *bg;
	GameState state;
	GameState nextState;

	float stateStartTime;
	bool firstOfState;
	bool lastOfState;
	MintSprite *root;

	/// Streaming
	char *streamNames[STREAM_MAX];
	int streamNamesNum;

	char *streamUrls[STREAM_MAX];
	int streamUrlsNum;

	bool isStreaming;
	int curStreamIndex;

	Asset *loadedAssets[ASSETS_MAX];

	/// Passage
	char mainTextStr[HUGE_STR];
	MintSprite *mainText;
	Passage *passages[PASSAGE_MAX];
	int passagesNum;

	MintSprite *images[IMAGES_MAX];
	Channel *audios[AUDIOS_MAX];

	/// Graph
};

void switchState(GameState state);
void changeState();
void updateState();

void runMod(char *serialData);
void msg(const char *str, ...);

duk_ret_t append(duk_context *ctx);
duk_ret_t setMainText(duk_context *ctx);
duk_ret_t submitPassage(duk_context *ctx);
duk_ret_t streamAsset(duk_context *ctx);
void assetStreamed(char *serialData);
duk_ret_t execAsset(duk_context *ctx);

duk_ret_t gotoPassage(duk_context *ctx);
duk_ret_t saveGame(duk_context *ctx);
duk_ret_t loadGame(duk_context *ctx);
void gameLoaded(char *data);

/// Images
duk_ret_t addImage(duk_context *ctx);
duk_ret_t addCanvasImage(duk_context *ctx);
duk_ret_t addRectImage(duk_context *ctx);
duk_ret_t add9SliceImage(duk_context *ctx);
duk_ret_t addEmptyImage(duk_context *ctx);
duk_ret_t setImageProps(duk_context *ctx);
duk_ret_t setImageText(duk_context *ctx);
duk_ret_t getImageSize(duk_context *ctx);
duk_ret_t getTextSize(duk_context *ctx);
duk_ret_t getImageFrames(duk_context *ctx);
duk_ret_t getImageProps(duk_context *ctx);
duk_ret_t destroyImage(duk_context *ctx);
duk_ret_t addChild(duk_context *ctx);
duk_ret_t gotoFrameNamed(duk_context *ctx);
duk_ret_t gotoFrameNum(duk_context *ctx);
duk_ret_t copyPixels(duk_context *ctx);

duk_ret_t getTextureWidth(duk_context *ctx);
duk_ret_t getTextureHeight(duk_context *ctx);

/// Audio
duk_ret_t playAudio(duk_context *ctx);
duk_ret_t destroyAudio(duk_context *ctx);
duk_ret_t setAudioFlags(duk_context *ctx);

Game *game = NULL;
char tempBytes[Megabytes(2)];

void initGame(MintSprite *bgSpr) {
	///
	//@incomplete Copy sound tweaks
	///

	getTextureAsset("NunitoSans-Light_22")->level = 3;
	getTextureAsset("NunitoSans-Light_30")->level = 3;
	getTextureAsset("NunitoSans-Light_38")->level = 3;

	if (engine->platform == PLAT_ANDROID) {
		strcpy(engine->spriteData.defaultFont, "OpenSans-Regular_40");
	} else {
		strcpy(engine->spriteData.defaultFont, "OpenSans-Regular_20");
	}

	initJs();

	char buf[1024];
	sprintf(buf, "var gameWidth = %d;\n var gameHeight = %d;\n", engine->width, engine->height);
	runJs(buf);

	addJsFunction("submitPassage", submitPassage, 1);
	addJsFunction("streamAsset", streamAsset, 2);
	addJsFunction("execAsset", execAsset, 1);

	addJsFunction("append", append, 1);
	addJsFunction("setMainText", setMainText, 1);
	addJsFunction("gotoPassage_internal", gotoPassage, 1);
	addJsFunction("addImage_internal", addImage, 1);
	addJsFunction("addCanvasImage_internal", addCanvasImage, 3);
	addJsFunction("addRectImage_internal", addRectImage, 3);
	addJsFunction("add9SliceImage_internal", add9SliceImage, 7);
	addJsFunction("addEmptyImage_internal", addEmptyImage, 2);
	addJsFunction("setImageProps", setImageProps, 9);
	addJsFunction("setImageText_internal", setImageText, 2);
	addJsFunction("getImageSize", getImageSize, 2);
	addJsFunction("getTextSize", getTextSize, 2);
	addJsFunction("getImageFrames", getImageFrames, 1);
	addJsFunction("getImageProps_internal", getImageProps, 2);
	addJsFunction("destroyImage", destroyImage, 1);
	addJsFunction("addChild_internal", addChild, 2);
	addJsFunction("gotoFrameNamed", gotoFrameNamed, 2);
	addJsFunction("gotoFrameNum", gotoFrameNum, 2);
	addJsFunction("copyPixels_internal", copyPixels, 7);
	addJsFunction("getTextureWidth_internal", getTextureWidth, 1);
	addJsFunction("getTextureHeight_internal", getTextureHeight, 1);

	addJsFunction("playAudio_internal", playAudio, 1);
	addJsFunction("destroyAudio", destroyAudio, 1);
	addJsFunction("setAudioFlags", setAudioFlags, 3);

	addJsFunction("saveGame_internal", saveGame, 1);
	addJsFunction("loadGame_internal", loadGame, 0);

	// if (streq(name, "addNotif")) return (void *)addNotif;

	// if (streq(name, "gotoBrowser")) return (void *)gotoBrowser;
	// if (streq(name, "loadModFromDisk")) return (void *)loadModFromDisk;

	game = (Game *)zalloc(sizeof(Game));

	///
	//@incomplete Setup mod repo
	///

	char *initCode = (char *)getAsset("info/interConfig.js")->data;
	runJs(initCode);

	// char *tempCode = (char *)getAsset("info/scratch.phore")->data;
	char *tempCode = (char *)getAsset("info/main.phore")->data;
	runMod(tempCode);

	switchState(STATE_PASSAGE);
}

void deinitGame() {
	printf("deinited\n");
}

void switchState(GameState state) {
	if (game->nextState) return;

	game->nextState = state;

	if (game->state == STATE_NULL) changeState();
}

void changeState() {
	GameState oldState = game->state;

	game->firstOfState = false;
	game->lastOfState = true;
	updateState();

	if (game->root) game->root->destroy();
	game->root = createMintSprite();
	game->root->setupContainer(engine->width, engine->height);
	game->stateStartTime = engine->time;

	game->firstOfState = true;
	game->lastOfState = false;
	game->state = game->nextState;
	updateState();

	game->firstOfState = false;
	game->lastOfState = false;

	game->nextState = STATE_NULL;
}

void updateGame() {
	updateState();
}

void updateState() {
	char buf[1024];
	sprintf(
		buf,
		"time = %f;"
		"mouseX = %d;"
		"mouseY = %d;"
		"mouseJustDown = %d;"
		"mouseJustUp = %d;"
		"mouseDown = %d;"
		"__update();",
		engine->time,
		engine->mouseX,
		engine->mouseY,
		engine->leftMouseJustPressed,
		engine->leftMouseJustReleased,
		engine->leftMousePressed
		);
	runJs(buf);

	{ /// Update streaming
		if (!game->isStreaming && game->streamNamesNum > game->curStreamIndex) {
			game->isStreaming = true;
			platformLoadFromUrl(game->streamUrls[game->curStreamIndex], assetStreamed);
		}

		if (game->curStreamIndex == game->streamUrlsNum) {
			game->streamUrlsNum = 0;
			game->streamNamesNum = 0;
			game->curStreamIndex = 0;
		}
	}

	{ /// Check for ended audios
		for (int i = 0; i < AUDIOS_MAX; i++) {
			Channel *channel = game->audios[i];
			if (!channel) continue;
			if (!channel->exists) {
				game->audios[i] = NULL;
				char buf[1024];
				sprintf(buf, "audios[%d].exists = false;", i);
				runJs(buf);
			}
		}
	}

	if (game->state == STATE_PASSAGE) {
		if (game->firstOfState) {
		}

		if (game->lastOfState) {
			return;
		}

		if (!game->mainText) {
			game->mainText = createMintSprite();
			game->mainText->setupEmpty(engine->width - 64, 2048);
		}
		game->mainText->x = engine->width/2 - game->mainText->width/2;
		game->mainText->y = 20;
		game->mainText->layer = MAIN_TEXT_LAYER;
		game->mainText->setText(game->mainTextStr);
	}

	if (game->state == STATE_GRAPH) {
		if (game->firstOfState) {
		}

		if (game->lastOfState) {
			return;
		}
	}
}

void runMod(char *serialData) {
	// printf("Loaded data: %s\n", serialData);
	char *inputData = (char *)zalloc(SERIAL_SIZE);

	String *realData = newString();
	realData->append("var __passage = \"\";\n");
	realData->append("var __image = \"\";\n");
	realData->append("var __audio = \"\";\n");

	int serialLen = strlen(serialData);
	int inputLen = 0;
	for (int i = 0; i < serialLen; i++)
		if (serialData[i] != '\r')
			inputData[inputLen++] = serialData[i];

	const char *lineStart = inputData;
	bool inPassage = false;
	bool inImages = false;
	bool inAudio = false;
	for (int i = 0;; i++) {
		const char *lineEnd = strstr(lineStart, "\n");
		if (!lineEnd) {
			if (strlen(lineStart) == 0) break;
			else lineEnd = lineStart+strlen(lineStart);
		}

		char *line = tempBytes;
		line[0] = '\0';
		memcpy(line, lineStart, lineEnd-lineStart);
		line[lineEnd-lineStart] = '\0';

		if (strstr(line, "START_IMAGES")) {
			inImages = true;
			realData->append("var __image = \"\";\n");
		} else if (strstr(line, "END_IMAGES")) {
			inImages = false;
			realData->append("print(\"Submitting images is deprecated\");");
		} else if (strstr(line, "START_AUDIO")) {
			inAudio = true;
			realData->append("__audio = \"\";");
		} else if (strstr(line, "END_AUDIO")) {
			inAudio = false;
			realData->append("print(\"Submitting audio is deprecated\");");
		} else if (strstr(line, "START_PASSAGES")) {
			inPassage = true;
			realData->append("__passage = \"\";");
		} else if (strstr(line, "END_PASSAGES")) {
			realData->append("submitPassage(__passage);");
			inPassage = false;
		} else if (strstr(line, "---")) {
			if (inPassage) {
				realData->append("submitPassage(__passage);\n__passage = \"\";");
			} else if (inImages) {
			realData->append("print(\"Submitting images is deprecated\");");
			} else if (inAudio) {
			realData->append("print(\"Submitting audio is deprecated\");");
			}
		} else if (inPassage) {
			realData->append("__passage += \"");

			if (strstr(line, "\"")) {
				for (int lineIndex = 0; lineIndex < strlen(line); lineIndex++) {
					if (line[lineIndex] == '"') {
						realData->append("\\\"");
					} else {
						char buf[2] = {};
						buf[0] = line[lineIndex];
						realData->append(buf);
					}
				}
			} else {
				realData->append(line);
			}
			realData->append("\\n\";");
		} else if (inImages) {
			realData->append("__image += \"");
			realData->append(line);
			realData->append("\n\";");
		} else if (inAudio) {
			realData->append("__audio += \"");
			realData->append(line);
			realData->append("\n\";");
		} else {
			realData->append(line);
		}
		realData->append("\n");

		lineStart = lineEnd+1;
	}

	// printf("Gonna exec:\n%s\n", realData);
	// exit(1);

	Free(inputData);

	// printf("Final: %s\n", realData->cStr);
	runJs(realData->cStr);
	realData->destroy();
}

void msg(const char *str, ...) {
	char buffer[HUGE_STR];
	va_list argptr;
	va_start(argptr, str);
	vsprintf(buffer, str, argptr);
	va_end(argptr);

	printf("msg: %s\n", buffer);
}

duk_ret_t append(duk_context *ctx) {
	duk_int_t type = duk_get_type(ctx, -1);
	if (type == DUK_TYPE_STRING) {
		const char *data = duk_get_string(ctx, -1);
		// printf("Appending |%s|\n", data);

		String *str = newString();
		str->set(data);

		String **lines;
		int linesNum;
		str->split("\n", &lines, &linesNum);
		str->destroy();

		for (int i = 0; i < linesNum; i++) {
			// printf("Line is |%s|\n", lines[i]->cStr);
			String *line = lines[i];
			if (line->charAt(0) == '[') {
				line->pop();
				line->unshift();
				String *label;
				String *result;

				int barIndex = line->indexOf("|");
				if (barIndex != -1) {
					label = line->subStrAbs(0, barIndex);
					result = line->subStrAbs(barIndex+1, line->length);
				} else {
					label = line->clone();
					result = line->clone();
				}

				String *code = newString();
				code->set("addChoice(\"");
				code->append(label->cStr);
				code->append("\", \"");
				code->append(result->cStr);
				code->append("\");");
				// printf("Gonna run %s\n", code->cStr);
				runJs(code->cStr);
				code->destroy();
				label->destroy();
				result->destroy();
			} else {
				strcat(game->mainTextStr, line->cStr);
				if (i < linesNum-1) strcat(game->mainTextStr, "\n");
			}
			line->destroy();
		}
		Free(lines);

	} else if (type == DUK_TYPE_NUMBER) {
		double num = duk_get_number(ctx, -1);
		char buf[64];
		if (num == round(num)) {
			sprintf(buf, "%.0f", num);
		} else {
			sprintf(buf, "%f", num);
		}
		strcat(game->mainTextStr, buf);
	}

	return 0;
}

duk_ret_t submitPassage(duk_context *ctx) {
	const char *str = duk_get_string(ctx, -1);
	String *data = newString();
	data->set(str);
	// printf("\n\n\nPassage:\n%s\n", data->cStr);

	int colonPos = data->indexOf(":");
	int nameEndPos = data->indexOf("\n", colonPos);
	String *name = data->subStrAbs(colonPos+1, nameEndPos);

	String *jsData = newString();
	int curPos = nameEndPos;
	bool isCode = false;
	bool appendNextCode = true;
	for (;;) {
		int nextPos = data->indexOf("`", curPos);

		if (nextPos == -1) nextPos = data->length-1;

		String *segment = data->subStrAbs(curPos, nextPos);
		// printf("Seg (code: %d): %s\n", isCode, segment->cStr);
		if (!isCode) {
			if (segment->getLastChar() == '!') {
				appendNextCode = false;
				segment->pop(1);
			}
			jsData->append("append(\"");

			String *fixedSeg = segment->replace("\n", "\\n");
			segment->destroy();
			segment = fixedSeg;
			//@incompelte Parse addChoice alias ([myTitle|myPass])

			jsData->append(segment->cStr);
			jsData->append("\");");
		} else {
			if (appendNextCode) {
				jsData->append("append(");
				if (segment->getLastChar() == ';') segment->pop(1);
				jsData->append(segment->cStr);
				jsData->append(");");
			} else {
				jsData->append(segment->cStr);
			}

			appendNextCode = true;
		}
		jsData->append("\n");
		segment->destroy();

		curPos = nextPos + 1;
		isCode = !isCode;
		if (curPos >= data->length-1) break;
	}

	Passage *passage = (Passage *)zalloc(sizeof(Passage));
	passage->name = name;
	passage->data = jsData;
	// printf("Code: %s\n", jsData->cStr);

	bool addNew = true;
	for (int i = 0; i < game->passagesNum; i++) {
		if (game->passages[i]->name->equals(passage->name)) {
			game->passages[i]->name->destroy();
			game->passages[i]->data->destroy();
			Free(game->passages[i]);
			game->passages[i] = passage;
			addNew = false;
		}
	}

	if (addNew) game->passages[game->passagesNum++] = passage;

	data->destroy();
	// exit(0);
	return 0;
}

duk_ret_t streamAsset(duk_context *ctx) {
	const char *url = duk_get_string(ctx, -1);
	const char *assetName = duk_get_string(ctx, -2);

	game->streamNames[game->streamNamesNum++] = stringClone(assetName);
	game->streamUrls[game->streamUrlsNum++] = stringClone(url);

	return 0;
}

void assetStreamed(char *serialData) {
	const char *name = game->streamNames[game->curStreamIndex];
	const char *url = game->streamUrls[game->curStreamIndex];

	addAsset(name, serialData, platformLoadedStringSize);

	for (int i = 0; i < ASSETS_MAX; i++) {
		if (!game->loadedAssets[i]) {
			game->loadedAssets[i] = getAsset(name);
			break;
		}
	}

	if (stringEndsWith(name, ".phore") || stringEndsWith(name, ".js")) runMod((char *)getAsset(name)->data);

	game->curStreamIndex++;
	game->isStreaming = false;

	Free((void *)name);
	Free((void *)url);
}

duk_ret_t execAsset(duk_context *ctx) {
	const char *assetId = duk_get_string(ctx, -1);

	runMod((char *)getAsset(assetId)->data);

	return 0;
}

duk_ret_t gotoPassage(duk_context *ctx) {
	const char *passageName = duk_get_string(ctx, -1);
	// printf("Going to %s\n", passageName);

	// if (game->state == STATE_MENU) changeState(STATE_MOD);

	// game->scrollAmount = 0;
	// game->passageStartTime = engine->time;
	// game->mainText->alpha = 0;

	// printf("Passages %d\n", game->passagesNum);
	for (int i = 0; i < game->passagesNum; i++) {
		Passage *passage = game->passages[i];
		// printf("Checking passage %s\n", passage->name);
		if (streq(passage->name->cStr, passageName)) {
			runJs(passage->data->cStr);
			return 0;
		}
	}

	msg("Failed to find passage %s\n", passageName);
	return 0;
}

duk_ret_t saveGame(duk_context *ctx) {
	const char *data = duk_get_string(ctx, -1);
	msg("Game saved!");
	platformSaveToDisk(data);
	return 0;
}

duk_ret_t loadGame(duk_context *ctx) {
	platformLoadFromDisk(gameLoaded);
	return 0;
}

void gameLoaded(char *data) {
	// printf("Loaded: %s\n", data);

	if (!streq(data, "none") && !streq(data, "(null)")) {
		msg("Game loaded!");
		char *buf = (char *)Malloc(strlen(data) + 1024);
		sprintf(buf, "checkpointStr = '%s'; data = JSON.parse(checkpointStr);", data);
		// printf("Running: %s\n", jsCommand);
		runJs(buf);
		Free(buf);
	} else {
		msg("No save game found");
	}

	Free(data);
}

duk_ret_t setMainText(duk_context *ctx) {
	const char *text = duk_get_string(ctx, -1);
	strcpy(game->mainTextStr, text);
	return 0;
}

//
//
//         IMAGES START
//
//

duk_ret_t addImage(duk_context *ctx) {
	const char *assetId = duk_get_string(ctx, -1);

	int slot;
	for (slot = 0; slot < IMAGES_MAX; slot++) if (!game->images[slot]) break;

	if (slot >= IMAGES_MAX) {
		msg("Too many images");
		duk_push_int(ctx, -1);
		return 1;
	}

	MintSprite *spr = createMintSprite(assetId);
	game->images[slot] = spr;

	duk_push_int(ctx, slot);
	return 1;
}

duk_ret_t addCanvasImage(duk_context *ctx) {
	int height = duk_get_number(ctx, -1);
	int width = duk_get_number(ctx, -2);
	const char *assetId = duk_get_string(ctx, -3);

	int slot;
	for (slot = 0; slot < IMAGES_MAX; slot++) if (!game->images[slot]) break;

	if (slot >= IMAGES_MAX) {
		msg("Too many images");
		duk_push_int(ctx, -1);
		return 1;
	}

	MintSprite *spr = createMintSprite();
	spr->setupCanvas(assetId, width, height);
	game->images[slot] = spr;

	duk_push_int(ctx, slot);
	return 1;
}

duk_ret_t addRectImage(duk_context *ctx) {
	int colour = duk_get_number(ctx, -1);
	int height = duk_get_number(ctx, -2);
	int width = duk_get_number(ctx, -3);

	int slot;
	for (slot = 0; slot < IMAGES_MAX; slot++) if (!game->images[slot]) break;

	if (slot >= IMAGES_MAX) {
		msg("Too many images");
		duk_push_int(ctx, -1);
		return 1;
	}

	MintSprite *spr = createMintSprite();
	spr->setupRect(width, height, colour);
	game->images[slot] = spr;

	duk_push_int(ctx, slot);
	return 1;
}

duk_ret_t add9SliceImage(duk_context *ctx) {
	int y2 = duk_get_number(ctx, -1);
	int x2 = duk_get_number(ctx, -2);
	int y1 = duk_get_number(ctx, -3);
	int x1 = duk_get_number(ctx, -4);
	int height = duk_get_number(ctx, -5);
	int width = duk_get_number(ctx, -6);
	const char *assetId = duk_get_string(ctx, -7);

	int slot;
	for (slot = 0; slot < IMAGES_MAX; slot++) if (!game->images[slot]) break;

	if (slot >= IMAGES_MAX) {
		msg("Too many images");
		duk_push_int(ctx, -1);
		return 1;
	}

	MintSprite *spr = createMintSprite();
	spr->setup9Slice(assetId, width, height, x1, y1, x2, y2);
	game->images[slot] = spr;

	duk_push_int(ctx, slot);
	return 1;
}

duk_ret_t addEmptyImage(duk_context *ctx) {
	int height = duk_get_number(ctx, -1);
	int width = duk_get_number(ctx, -2);

	int slot;
	for (slot = 0; slot < IMAGES_MAX; slot++) if (!game->images[slot]) break;

	if (slot >= IMAGES_MAX) {
		msg("Too many images");
		duk_push_int(ctx, -1);
		return 1;
	}

	MintSprite *spr = createMintSprite();
	spr->setupEmpty(width, height);
	game->images[slot] = spr;

	duk_push_int(ctx, slot);
	return 1;
}

duk_ret_t setImageProps(duk_context *ctx) {
	int layer = duk_get_number(ctx, -1);
	int tint = duk_get_number(ctx, -2);
	double rotation = duk_get_number(ctx, -3);
	double alpha = duk_get_number(ctx, -4);
	double scaleY = duk_get_number(ctx, -5);
	double scaleX = duk_get_number(ctx, -6);
	double y = duk_get_number(ctx, -7);
	double x = duk_get_number(ctx, -8);
	int id = duk_get_number(ctx, -9);

	MintSprite *img = game->images[id];
	img->x = x;
	img->y = y;
	img->scaleX = scaleX;
	img->scaleY = scaleY;
	img->alpha = alpha;
	img->rotation = rotation;
	img->tint = tint;
	img->layer = layer;

	return 0;
}

duk_ret_t setImageText(duk_context *ctx) {
	const char *text = duk_get_string(ctx, -1);
	int id = duk_get_number(ctx, -2);

	MintSprite *img = game->images[id];
	img->setText(text);

	return 0;
}

duk_ret_t getImageSize(duk_context *ctx) {
	int arrayIndex = duk_get_number(ctx, -1);
	int id = duk_get_number(ctx, -2);
	MintSprite *img = game->images[id];

	char buf[1024];
	sprintf(buf, "images[%d].width = %d;\nimages[%d].height = %d;\n", arrayIndex, img->width, arrayIndex, img->height);
	runJs(buf);

	return 0;
}

duk_ret_t getTextSize(duk_context *ctx) {
	int arrayIndex = duk_get_number(ctx, -1);
	int id = duk_get_number(ctx, -2);
	MintSprite *img = game->images[id];

	char buf[1024];
	sprintf(buf, "images[%d].textWidth = %d;\nimages[%d].textHeight = %d;\n", arrayIndex, img->textWidth, arrayIndex, img->textHeight);
	runJs(buf);

	return 0;
}

duk_ret_t getImageFrames(duk_context *ctx) {
	int id = duk_get_number(ctx, -1);
	MintSprite *img = game->images[id];
	duk_push_int(ctx, img->framesNum);
	return 1;
}

duk_ret_t getImageProps(duk_context *ctx) {
	int arrayIndex = duk_get_number(ctx, -1);
	int id = duk_get_number(ctx, -2);
	MintSprite *img = game->images[id];

	char *data = (char *)duk_push_buffer(ctx, sizeof(char) * 7, false);
	data[0] = img->justPressed;
	data[1] = img->justReleased;
	data[2] = img->pressing;
	data[3] = img->justHovered;
	data[4] = img->justUnHovered;
	data[5] = img->hovering;

	return 1;
}

duk_ret_t destroyImage(duk_context *ctx) {
	int id = duk_get_number(ctx, -1);
	MintSprite *img = game->images[id];
	game->images[id] = NULL;
	img->destroy();

	return 0;
}

duk_ret_t addChild(duk_context *ctx) {
	int otherId = duk_get_number(ctx, -1);
	int id = duk_get_number(ctx, -2);

	MintSprite *img = game->images[id];
	MintSprite *otherImg = game->images[otherId];
	img->addChild(otherImg);

	return 0;
}

duk_ret_t gotoFrameNamed(duk_context *ctx) {
	const char *frameName = duk_get_string(ctx, -1);
	int id = duk_get_number(ctx, -2);

	MintSprite *img = game->images[id];
	img->gotoFrame(frameName);
	img->playing = false;

	return 0;

}

duk_ret_t gotoFrameNum(duk_context *ctx) {
	int frameNum = duk_get_number(ctx, -1);
	int id = duk_get_number(ctx, -2);

	MintSprite *img = game->images[id];
	img->gotoFrame(frameNum);
	img->playing = false;

	return 0;
}

duk_ret_t copyPixels(duk_context *ctx) {
	int dy = duk_get_number(ctx, -1);
	int dx = duk_get_number(ctx, -2);
	int sh = duk_get_number(ctx, -3);
	int sw = duk_get_number(ctx, -4);
	int sy = duk_get_number(ctx, -5);
	int sx = duk_get_number(ctx, -6);
	int id = duk_get_number(ctx, -7);

	MintSprite *img = game->images[id];
	img->copyPixels(sx, sy, sw, sh, dx, dy);

	return 0;
}

duk_ret_t getTextureWidth(duk_context *ctx) {
	const char *assetId = duk_get_string(ctx, -1);
	Asset *textureAsset = getTextureAsset(assetId);
	duk_push_int(ctx, textureAsset->width);
	return 1;
}

duk_ret_t getTextureHeight(duk_context *ctx) {
	const char *assetId = duk_get_string(ctx, -1);
	Asset *textureAsset = getTextureAsset(assetId);
	duk_push_int(ctx, textureAsset->height);
	return 1;
}
//
//
//         IMAGES END
//
//


//
//
//         AUDIO START
//
//

duk_ret_t playAudio(duk_context *ctx) {
	const char *assetId = duk_get_string(ctx, -1);

	int slot;
	for (slot = 0; slot < AUDIOS_MAX; slot++) if (!game->audios[slot]) break;

	if (slot >= AUDIOS_MAX) {
		msg("Too many audios");
		duk_push_int(ctx, -1);
		return 1;
	}

	Channel *channel = playEffect(assetId);
	game->audios[slot] = channel;

	duk_push_int(ctx, slot);

	return 1;
}

duk_ret_t destroyAudio(duk_context *ctx) {
	int id = duk_get_number(ctx, -1);

	if (!game->audios[id]) return 0;

	game->audios[id]->destroy();
	game->audios[id] = NULL;

	return 0;
}

duk_ret_t setAudioFlags(duk_context *ctx) {
	int volume = duk_get_number(ctx, -1);
	int looping = duk_get_boolean(ctx, -2);
	int id = duk_get_number(ctx, -3);

	Channel *channel = game->audios[id];
	channel->looping = looping;
	channel->userVolume = volume;
	return 0;
}

//
//
//         AUDIO END
//
//
