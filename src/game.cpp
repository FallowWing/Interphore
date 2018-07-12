#define PASSAGE_MAX 1024
#define IMAGES_MAX 256
#define AUDIOS_MAX 256
#define STREAM_MAX 256
#define ASSETS_MAX 256

#define BUTTON_HEIGHT 128

#define MAIN_TEXT_LAYER 20

#define PROFILE_JS_UPDATE 0
#define PROFILE_GAME_UPDATE 1

struct Passage {
	char *name;
	char *data;
};

struct Game {
	Profiler profiler;
	DebugOverlay debugOverlay;

	MintSprite *bg;

	MintSprite *root;

	/// Streaming
	char *streamNames[STREAM_MAX];
	int streamNamesNum;

	char *streamUrls[STREAM_MAX];
	int streamUrlsNum;

	bool isStreaming;
	int curStreamIndex;

	Asset *loadedAssets[ASSETS_MAX];

	char mainTextStr[HUGE_STR];
	MintSprite *mainText;
	Passage *passages[PASSAGE_MAX];
	int passagesNum;

	MintSprite *images[IMAGES_MAX];
	Channel *audios[AUDIOS_MAX];
};

void initGame();
void deinitGame();
void updateGame();

void runMod(char *serialData);
void msg(const char *str, ...);
void jsError(const char *message);

duk_ret_t append(duk_context *ctx);
duk_ret_t setMainText(duk_context *ctx);
duk_ret_t addPassage(duk_context *ctx);
duk_ret_t streamAsset(duk_context *ctx);
void assetStreamed(char *serialData);
duk_ret_t execAsset(duk_context *ctx);

duk_ret_t gotoPassage(duk_context *ctx);
duk_ret_t saveGame(duk_context *ctx);
duk_ret_t loadGame(duk_context *ctx);
void gameLoaded(char *data);
duk_ret_t loadMod(duk_context *ctx);
void modLoaded(char *data);

duk_ret_t interTweenEase(duk_context *ctx);
duk_ret_t setFontTag(duk_context *ctx);
duk_ret_t streamEmbeddedTexture(duk_context *ctx);
duk_ret_t openSoftwareKeyboard(duk_context *ctx);
duk_ret_t interRnd(duk_context *ctx);

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
duk_ret_t setImageFont(duk_context *ctx);

duk_ret_t getTextureWidth(duk_context *ctx);
duk_ret_t getTextureHeight(duk_context *ctx);
duk_ret_t setWordWrapWidth(duk_context *ctx);

/// Audio
duk_ret_t addSoundTweakJs(duk_context *ctx);
duk_ret_t playMusic(duk_context *ctx);
duk_ret_t playEffect(duk_context *ctx);
duk_ret_t destroyAudio(duk_context *ctx);
duk_ret_t setAudioFlags(duk_context *ctx);
duk_ret_t setMasterVolume(duk_context *ctx);

Game *game = NULL;
char tempBytes[Megabytes(2)];

void initGame() {
	windowsDiskLoadPath = stringClone("currentMod.phore");

	getTextureAsset("NunitoSans-Light_22")->level = 3;
	getTextureAsset("NunitoSans-Light_30")->level = 3;
	getTextureAsset("NunitoSans-Light_26")->level = 3;

	strcpy(engine->spriteData.defaultFont, "OpenSans-Regular_40");

	initJs();
	jsErrorFn = jsError;

	addJsFunction("addPassage", addPassage, 2);
	addJsFunction("streamAsset", streamAsset, 2);
	addJsFunction("execAsset", execAsset, 1);

	addJsFunction("append_internal", append, 1);
	addJsFunction("setMainText", setMainText, 1);
	addJsFunction("gotoPassage_internal", gotoPassage, 1);
	addJsFunction("tweenEase", interTweenEase, 2);
	addJsFunction("setFontTag", setFontTag, 2);
	addJsFunction("streamEmbeddedTexture", streamEmbeddedTexture, 1);
	addJsFunction("openSoftwareKeyboard", openSoftwareKeyboard, 0);
	addJsFunction("rnd_internal", interRnd, 0);

	addJsFunction("addImage_internal", addImage, 1);
	addJsFunction("addCanvasImage_internal", addCanvasImage, 3);
	addJsFunction("addRectImage_internal", addRectImage, 3);
	addJsFunction("add9SliceImage_internal", add9SliceImage, 7);
	addJsFunction("addEmptyImage_internal", addEmptyImage, 2);
	addJsFunction("setImageProps", setImageProps, 11);
	addJsFunction("setImageText_internal", setImageText, 2);
	addJsFunction("getImageSize", getImageSize, 2);
	addJsFunction("getTextSize", getTextSize, 2);
	addJsFunction("getImageFrames", getImageFrames, 1);
	addJsFunction("getImageProps_internal", getImageProps, 1);
	addJsFunction("destroyImage", destroyImage, 1);
	addJsFunction("addChild_internal", addChild, 2);
	addJsFunction("gotoFrameNamed", gotoFrameNamed, 2);
	addJsFunction("gotoFrameNum", gotoFrameNum, 2);
	addJsFunction("copyPixels_internal", copyPixels, 7);
	addJsFunction("setImageFont", setImageFont, 2);
	addJsFunction("getTextureWidth_internal", getTextureWidth, 1);
	addJsFunction("getTextureHeight_internal", getTextureHeight, 1);
	addJsFunction("setWordWrapWidth_internal", setWordWrapWidth, 2);

	addJsFunction("addSoundTweak", addSoundTweakJs, 2);
	addJsFunction("playMusic_internal", playMusic, 1);
	addJsFunction("playEffect_internal", playEffect, 1);
	addJsFunction("destroyAudio", destroyAudio, 1);
	addJsFunction("setAudioFlags", setAudioFlags, 3);
	addJsFunction("setMasterVolume", setMasterVolume, 1);

	addJsFunction("saveGame_internal", saveGame, 1);
	addJsFunction("loadGame_internal", loadGame, 0);
	addJsFunction("loadMod_internal", loadMod, 0);

	game = (Game *)zalloc(sizeof(Game));
	game->bg = createMintSprite();
	game->bg->setupRect(engine->width, engine->height, 0x000000);

	initProfiler(&game->profiler);
	initDebugOverlay(&game->debugOverlay);

	char *initCode = (char *)getAsset("info/interConfig.js")->data;
	runJs(initCode);

	// char *tempCode = (char *)getAsset("info/basic.phore")->data;
	char *tempCode = (char *)getAsset("info/main.phore")->data;
	runMod(tempCode);

#ifdef FAST_SCRATCH
	runJs("gotoPassage(\"scratchModStart\");");
#endif

	game->root = createMintSprite();
	game->root->setupContainer(engine->width, engine->height);

// #ifdef SEMI_FLASH
// 	const char *modNamePrefix = "modName=";

// 	char autoRunMod[PATH_LIMIT] = {};

// 	char *curUrl = flashGetUrl();
// 	char *modNamePos = strstr(curUrl, modNamePrefix);
// 	if (modNamePos) {
// 		modNamePos += strlen(modNamePrefix);
// 		strcpy(autoRunMod, modNamePos);
// 	}
// 	Free(curUrl);

// 	printf("cur url is %s\n", curUrl);
// 	printf("auto run mod is %s\n", autoRunMod);
// 	if (autoRunMod[0] != '\0') runMod((char *)getAsset(autoRunMod)->data);
// #endif
}

void deinitGame() {
	for (int i = 0; i < IMAGES_MAX; i++) {
		MintSprite *img = game->images[i];
		if (img) img->destroy();
	}
	game->bg->destroy();

	for (int i = 0; i < game->passagesNum; i++) {
		Passage *passage = game->passages[i];
		Free(passage->name);
		Free(passage->data);
		Free(passage);
	}

	deinitJs();
	Free(game);

	// stb_leakcheck_dumpmem();
	printf("deinited\n");
}

void updateGame() {
	game->profiler.startProfile(PROFILE_JS_UPDATE);

	char buf[1024];
	sprintf(
		buf,
		"time = %f;"
		"mouseX = %d;"
		"mouseY = %d;"
		"mouseJustDown = %d;"
		"mouseJustUp = %d;"
		"mouseDown = %d;"
		"mouseWheel = %d;"
		"assetStreamsLeft = %d;"
		"__update();",
		engine->time,
		engine->mouseX,
		engine->mouseY,
		engine->leftMouseJustPressed,
		engine->leftMouseJustReleased,
		engine->leftMousePressed,
		platformMouseWheel,
		game->streamUrlsNum - game->curStreamIndex
	);
	runJs(buf);

	game->profiler.endProfile(PROFILE_JS_UPDATE);
	game->profiler.startProfile(PROFILE_GAME_UPDATE);

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
				sprintf(buf, "getAudioById(%d).destroy();", i);
				runJs(buf);
			}
		}
	}

	if (!game->mainText) {
		game->mainText = createMintSprite();
		game->mainText->setupEmpty(engine->width - 64, 2048);
		game->mainText->clipRect.setTo(0, 0, engine->width, engine->height - BUTTON_HEIGHT - 16);
		strcpy(game->mainText->defaultFont, "NunitoSans-Light_26");
		game->mainText->tint = 0xFFdff9ff;
	}

	int viewHeight = engine->height - BUTTON_HEIGHT - 16;
	float newY = game->mainText->y;
	int minY = -game->mainText->textHeight + viewHeight;
	int maxY = 20;

	if (game->mainText->textHeight > viewHeight) {
		if (game->mainText->holding) newY = engine->mouseY - game->mainText->holdPivot.y;
		if (platformMouseWheel < 0) newY -= 20;
		if (platformMouseWheel > 0) newY += 20;
		newY = Clamp(newY, minY, maxY);
	}

	game->mainText->x = engine->width/2 - game->mainText->width/2;
	game->mainText->y = newY;
	game->mainText->layer = MAIN_TEXT_LAYER;
	game->mainText->setText(game->mainTextStr);

	game->profiler.endProfile(PROFILE_GAME_UPDATE);

	if (keyIsJustReleased('`')) {
		printf("JS: %0.2f CPP: %0.2f\n", game->profiler.getAverage(PROFILE_JS_UPDATE), game->profiler.getAverage(PROFILE_GAME_UPDATE));
	}

	game->debugOverlay.update();
}

void runMod(char *serialData) {
	// printf("Loaded data: %s\n", serialData);
	char *inputData = (char *)zalloc(SERIAL_SIZE);

	String *realData = newString(4096);
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
					} else if (line[lineIndex] == '\\') {
						realData->append("\\\\");
					} else {
						realData->appendChar(line[lineIndex]);
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

void jsError(const char *message) {
	printf("Got js Error: %s\n", message);

	char *buf = (char *)Malloc(strlen(message) + 1024);
	sprintf(buf, "msg(\"Error: %s\", {smallFont: true, hugeTexture: true, extraTime: 20});", message);
	runJs(buf);
	Free(buf);
}

duk_ret_t append(duk_context *ctx) {
	const char *data = duk_get_string(ctx, -1);
	strcat(game->mainTextStr, data);

	return 0;
}

duk_ret_t addPassage(duk_context *ctx) {
	const char *data = duk_get_string(ctx, -1);
	const char *name = duk_get_string(ctx, -2);

	Passage *passage = (Passage *)zalloc(sizeof(Passage));
	passage->name = stringClone(name);
	passage->data = stringClone(data);
	// printf("Passage |%s| added, contains:\n%s\n", name, data);

	bool addNew = true;
	for (int i = 0; i < game->passagesNum; i++) {
		if (streq(game->passages[i]->name, passage->name)) {
			Free(game->passages[i]->name);
			Free(game->passages[i]->data);
			Free(game->passages[i]);
			game->passages[i] = passage;
			addNew = false;
		}
	}

	if (addNew) game->passages[game->passagesNum++] = passage;

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

	// printf("Asset done streaming: %s\n", name);
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
		if (streq(passage->name, passageName)) {
			// printf("Running passage |%s|\n", passage->data);
			runJs(passage->data);
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
		// printf("Running: %s\n", buf);
		runJs(buf);
		Free(buf);
	} else {
		msg("No save game found");
	}

	Free(data);
}

duk_ret_t loadMod(duk_context *ctx) {
	platformLoadFromDisk(modLoaded);
	return 0;
}

void modLoaded(char *data) {
	// printf("Loaded: %s\n", data);

	if (!streq(data, "none") && !streq(data, "(null)")) {
		msg("Mod loaded!");
		String *str = newString(2048);
		str->append("try {\n");
		str->append(data);
		str->append("\n} catch (e) { msg(String(e.stack), {smallFont: true, hugeTexture: true, extraTime: 20}); }\n");
		runMod(str->cStr);
		str->destroy();
	} else {
		msg("No mod found");
	}

	Free(data);
}


duk_ret_t setMainText(duk_context *ctx) {
	const char *text = duk_get_string(ctx, -1);
	strcpy(game->mainTextStr, text);
	if (game->mainText) game->mainText->y = 20;
	return 0;
}

duk_ret_t interTweenEase(duk_context *ctx) {
	int easeType = duk_get_int(ctx, -1);
	float perc = duk_get_number(ctx, -2);

	duk_push_number(ctx, tweenEase(perc, (Ease)easeType));

	return 1;
}

duk_ret_t setFontTag(duk_context *ctx) {
	const char *fontName = duk_get_string(ctx, -1);
	const char *tag = duk_get_string(ctx, -2);

	engine->spriteData.tagMap->setString(tag, fontName);

	return 0;
}

duk_ret_t streamEmbeddedTexture(duk_context *ctx) {
	const char *assetId = duk_get_string(ctx, -1);

	const char *textures[1] = {assetId};

	streamTextures(textures, 1, 1);

	return 0;
}

duk_ret_t openSoftwareKeyboard(duk_context *ctx) {
	displayKeyboard(true);

	return 0;
}

duk_ret_t interRnd(duk_context *ctx) {
	duk_push_number(ctx, rnd());
	return 1;
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
	int centerPivot = duk_get_boolean(ctx, -1);
	int smoothing = duk_get_boolean(ctx, -2);
	int layer = duk_get_int(ctx, -3);
	int tint = duk_get_uint(ctx, -4);
	double rotation = duk_get_number(ctx, -5);
	double alpha = duk_get_number(ctx, -6);
	double scaleY = duk_get_number(ctx, -7);
	double scaleX = duk_get_number(ctx, -8);
	double y = duk_get_number(ctx, -9);
	double x = duk_get_number(ctx, -10);
	int id = duk_get_number(ctx, -11);

	MintSprite *img = game->images[id];
	if (!img) return 0;

	img->x = x;
	img->y = y;
	img->scaleX = scaleX;
	img->scaleY = scaleY;
	img->alpha = alpha;
	img->rotation = rotation;
	img->tint = tint;
	img->layer = layer;
	img->smoothing = smoothing;
	img->centerPivot = centerPivot;

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
	int id = duk_get_number(ctx, -1);
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

duk_ret_t setImageFont(duk_context *ctx) {
	const char *fontName = duk_get_string(ctx, -1);
	int id = duk_get_int(ctx, -2);

	MintSprite *img = game->images[id];
	strcpy(img->defaultFont, fontName);

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

duk_ret_t setWordWrapWidth(duk_context *ctx) {
	int wordWrapWidth = duk_get_int(ctx, -1);
	int id = duk_get_int(ctx, -2);

	MintSprite *img = game->images[id];
	img->wordWrapWidth = wordWrapWidth;
	return 0;
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

duk_ret_t addSoundTweakJs(duk_context *ctx) {
	float volume = duk_get_number(ctx, -1);
	const char *assetId = duk_get_string(ctx, -2);

	addSoundTweak(assetId, volume);

	return 0;
}

duk_ret_t playMusic(duk_context *ctx) {
	const char *assetId = duk_get_string(ctx, -1);

	int slot;
	for (slot = 0; slot < AUDIOS_MAX; slot++) if (!game->audios[slot]) break;

	if (slot >= AUDIOS_MAX) {
		msg("Too many audios");
		duk_push_int(ctx, -1);
		return 1;
	}

	Channel *channel = playMusic(assetId);
	game->audios[slot] = channel;

	duk_push_int(ctx, slot);

	return 1;
}

duk_ret_t playEffect(duk_context *ctx) {
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

duk_ret_t setMasterVolume(duk_context *ctx) {
	float volume = duk_get_number(ctx, -1);

	engine->soundData.masterVolume = volume;

	return 0;
}

//
//
//         AUDIO END
//
//
