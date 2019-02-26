Duktape.errCreate = function (err) {
	try {
		if (typeof err === 'object' &&
			typeof err.message !== 'undefined' &&
			typeof err.lineNumber === 'number') {
			err.message = err.message + ' (line ' + err.lineNumber + ')';
		}
	} catch (e) {
		// ignore; for cases such as where "message" is not writable etc
	}
	return err;
}

function arrayContains(arr, ele) {
	var index = arr.indexOf(ele);
	return index != -1;
}

function arrayLazyRemove(arr, ele) {
	var index = arr.indexOf(ele);
	if (index != -1) arr.splice(index, 1);
}

function arrayCount(arr, ele) {
	var count = 0;
	arr.forEach(function(element) {
		if (element == ele) count++;
	});

	return count;
}

function arrayRemove(arr, ele) {
	var index = arr.indexOf(ele);
	if (index == -1) print("Going to fail to remove this element from an array");
	arr.splice(index, 1);
}

function arrayContainsAny(haystack, needles) {
	for (var i = 0; i < haystack.length; i++) {
		for (var j = 0; j < needles.length; j++) {
			if (haystack[i] == needles[j]) return true;
		}
	}

	return false;
}

function forRange(start, end, fn) {
	var arr = [];
	for (var i = start; i < end; i++) arr.push(i);

	arr.forEach(fn);
}

var images = [];
var audios = [];
var choices = [];
var timers = [];
var tweens = [];
var backgrounds = [];
var keys = [];
var msgs = [];
var iconDatabase = [];
var doneStreamingFns = [];
var tempUpdateFunctions = [];
var updateFunctions = [];
var clearFunctions = [];

var mouseX = 0;
var mouseY = 0;
var mouseDown = false;
var mouseJustDown = false;
var mouseJustUp = false;
var mouseWheel = 0;
var time = 0;
var data = {};
var checkpointStr = "{}";
var exitDisabled = false;
var keyboardOpened = false;
var isMuted = false;
var tooltipShowing = false;
var lastInput = "";
var choicePage = 0;
var assetStreamsLeft = 0;

var queuedCommands = [];
var queueTimeLeft = 0;
var commandSkipped = false;
var currentCommand = null;

var BG_START_LAYER = 10;
var MAIN_TEXT_LAYER = 20;
var GRAPH_BG_LAYER = 30; //@todo Remove
var GRAPH_LINE_LAYER = 40;
var GRAPH_NODE_LAYER = 50;
var GRAPH_NODE_TEXT_LAYER = 55;
var DEFAULT_LAYER = 60;
var CHOICE_BUTTON_LAYER = 70;
var CHOICE_TEXT_LAYER = 80;
var CHOICE_BUTTON_UPPER_LAYER = 85;
var CHOICE_TEXT_UPPER_LAYER = 89;
var VN_TEXT_BG_LAYER = 90;
var VN_TEXT_LAYER = 95; //@cleanup Unused
var MSG_SPRITE_LAYER = 100;
var MSG_TEXT_LAYER = 110;
var TITLE_LAYER = 120;
var TOOLTIP_SPRITE_LAYER = 130;
var TOOLTIP_TEXT_LAYER = 140;

var choicesPerPage = 4;
var BUTTON_HEIGHT = 128;

var TOP = 1;
var BOTTOM = 2;
var LEFT = 3;
var RIGHT = 4;

var DEFAULT = 5;
var TILED = 6;
var CENTERED = 7;

var LOOPING = 8;
var PINGPONG = 9;
var PINGPONG_ONCE = 10;

function newImage() {
	var img;
	img = {
		id: -1,
		exists: true,
		x: 0,
		y: 0,
		width: 0,
		height: 0,
		rotation: 0,
		tint: 0x00000000,
		alpha: 1,
		scaleX: 1,
		scaleY: 1,
		skewX: 0,
		skewY: 0,
		smoothing: false,
		centerPivot: false,
		ignoreMouseRect: false,

		text: "", //@cleanup Consider removing this
		textWidth: 0,
		textHeight: 0,
		layer: DEFAULT_LAYER,
		inInputField: false,
		setText: function(text) {
			if (text === undefined) return;
			text = text.replace(/%/g, "%%");
			img.text = text;
			setImageText(img, text);
		},
		setFont: function(fontName) {
			setImageFont(img.id, fontName);
		},
		setWordWrapWidth: function(wordWrapWidth) {
			setWordWrapWidth_internal(img.id, wordWrapWidth);
		},

		justPressed: false,
		justReleased: false,
		pressing: false,
		justHovered: false,
		justUnHovered: false,
		hovering: false,
		hoveringTime: 0,
		draggable: false,
		dragPivotX: 0,
		dragPivotY: 0,
		onRelease: null,
		onHover: null,
		whileHovering: null,
		onUnHover: null,

		temp: true,
		destroy: function() {
			img.exists = false;
			destroyImage(img.id);
			var index = images.indexOf(img);
			images.splice(index, 1);
		},
		addChild: function(otherImg) {
			addChild_internal(img.id, otherImg.id);
		},

		currentFrame: 0,
		totalFrames: 0,
		gotoFrame: function(frame, andStop) {
			if (andStop === undefined) andStop = false;

			if (typeof frame === "string") {
				gotoFrameNamed(img.id, frame, andStop);
			} else {
				gotoFrameNum(img.id, frame, andStop);
			}
		},

		copyPixels: function(sx, sy, sw, sh, dx, dy) {
			copyPixels_internal(img.id, sx, sy, sw, sh, dx, dy);
		}
	};

	images.push(img);
	return img;
}

function addImage(assetId) {
	var img = newImage();
	img.id = addImage_internal(assetId);
	img.totalFrames = getImageFrames(img.id);
	getImageSize(img.id, images.indexOf(img));
	return img;
}

function addCanvasImage(assetId, width, height) {
	var img = newImage();
	img.id = addCanvasImage_internal(assetId, width, height);
	img.width = width;
	img.height = height;
	img.totalFrames = getImageFrames(img.id);
	return img;
}

function add9SliceImage(assetId, width, height, x1, y1, x2, y2) {
	var img = newImage();
	img.id = add9SliceImage_internal(assetId, width, height, x1, y1, x2, y2);
	img.width = width;
	img.height = height;
	return img;
}

function addRectImage(width, height, colour) {
	var img = newImage();
	img.id = addRectImage_internal(width, height, colour);
	img.width = width;
	img.height = height;
	return img;
}

function addEmptyImage(width, height) {
	var img = newImage();
	img.id = addEmptyImage_internal(width, height);
	img.width = width;
	img.height = height;
	return img;
}

function addChoice(choiceText, result, config) {
	if (result === undefined) result = choiceText;

	var choice = {
		sprite: null,
		textField: null,
		result: result,
		enabled: true
	};

	var buttonWidth = (gameWidth - nextChoices.width - prevChoices.width)/choicesPerPage;

	var spr = add9SliceImage("img/writer/writerChoice.png", buttonWidth, BUTTON_HEIGHT, 5, 5, 10, 10);
	spr.temp = false;
	spr.centerPivot = true;

	spr.onRelease = function() {
		if (!choice.enabled) {
			playEffect("audio/ui/nope");
			return;
		}

		playEffect("audio/ui/bestChoiceClick");
		if (typeof choice.result === "string") {
			gotoPassage(choice.result);
		} else {
			choice.result();
		}
	}

	var tf = addEmptyImage(spr.width, spr.height);
	tf.wordWrapWidth = spr.width + 5;
	spr.addChild(tf);
	tf.temp = false;
	tf.setFont("NunitoSans-Light_22");
	tf.tint = 0xFFa5e3f2;
	tf.setText(choiceText);
	tf.x = spr.width/2 - tf.textWidth/2;
	tf.y = spr.height/2 - tf.textHeight/2;

	if (config) {
		if (config.icons) {
			config.icons.forEach(function(iconName, i) {
				var icon = addImage(iconDatabase[iconName]);
				if (isAndroid) icon.scaleX = icon.scaleY = 2;

				icon.x = i * icon.width * icon.scaleX + 2;
				icon.y = -icon.height * icon.scaleY - 2;
				icon.layer = CHOICE_TEXT_LAYER;
				spr.addChild(icon);

				icon.onRelease = function() {
					msg(config.icons[i]);
				}

				icon.onHover = function() {
					playEffect("audio/ui/hoverChoiceIcons");
				}

				icon.whileHovering = function() {
					showTooltip(config.icons[i]);
					icon.y = (-icon.height * icon.scaleY - 2) - 4;
				}

				icon.onUnHover = function() {
					icon.y = -icon.height * icon.scaleY - 2;
				}
			});
		}

		if (config.tooltip) {
			spr.whileHovering = function() {
				showTooltip(config.tooltip);
			}
		}

		if (config.enabled === undefined) config.enabled = true;
		choice.enabled = config.enabled;
	}

	choice.sprite = spr;
	choice.textField = tf;

	choices.push(choice);
	return choice;
}

function addInputField(title) {
	if (title === undefined) title = "Input:";
	inputTitle.setText(title);

	inputFieldBg.alpha = 1;
	inputField.inInputField = true;
	inputField.setText("");
}

function setImageText(img, text) {
	setImageText_internal(img.id, text);
	getTextSize(img.id, images.indexOf(img));
}

function newAudio() { /// You must push into audios manually
	var audio;
	audio = {
		id: -1,
		name: "",
		looping: false,
		volume: 1,
		destroy: function() {
			destroyAudio(audio.id);
			for (;;) {
				var index = audios.indexOf(audio);
				if (index == -1) break;
				if (index != -1) audios.splice(index, 1);
			}
		}
	};

	return audio;
}

function playMusic(assetId) {
	var audio = newAudio();
	audio.id = playMusic_internal(assetId);
	audio.looping = true;
	audios.push(audio);
	return audio;
}

function playEffect(assetId) {
	var audio = playMusic(assetId);
	audio.looping = false;
	return audio;

	// var audio = newAudio();
	// audio.id = playEffect_internal(assetId);
	// audios.push(audio);
	// return audio;
}

function getAudio(audioName) {
	for (var i = 0; i < audios.length; i++) {
		var audio = audios[i];
		if (audio.name == audioName) return audio;
	}

	var dummyAudio = newAudio();
	dummyAudio.destroy = function() {};
	return dummyAudio;
}

function getAudioById(id) {
	for (var i = 0; i < audios.length; i++) {
		var audio = audios[i];
		if (audio.id == id) return audio;
	}
}

function clearText() {
	setMainText("");
}

function clearChoices() {
	choicePage = 0;
	keyboardOpened = false;
	exitButton.alpha = exitDisabled ? 0 : 1; //@todo This should probably happen instantly
	lastInput = inputField.text;
	inputField.inInputField = false;
	inputFieldBg.alpha = 0;

	for (var i = 0; i < choices.length; i++) {
		choices[i].sprite.destroy();
		choices[i].textField.destroy();
	}

	choices = [];
}

function clear() {
	clearFunctions.forEach(function(fn) {
		fn();
	});
	clearFunctions = [];
	tempUpdateFunctions = [];

	clearText();
	clearChoices();
	choicesPerPage = 4;

	var imagesToRemove = [];
	for (var i = 0; i < images.length; i++) if (images[i].temp) imagesToRemove.push(images[i]);
	for (var i = 0; i < imagesToRemove.length; i++) imagesToRemove[i].destroy();
}

function gotoPassage(passageName) {
	clear();
	gotoPassage_internal(passageName);
}

function timer(delay, onComplete, loopCount) {
	if (loopCount === undefined) loopCount = 1;
	var tim;
	tim = {
		delay: delay,
		timeLeft: delay,
		onComplete: onComplete,
		loopCount: loopCount,
		destroy: function() {
			var index = timers.indexOf(tim);
			if (index != -1) timers.splice(index, 1);
		}
	};

	timers.push(tim);

	return tim;
}

function rnd() { return rnd_internal(); }
function rndInt(min, max) { return round(rndFloat(min, max)); }
function rndFloat(min, max) { return min + rnd() * (max - min); }
function floor(num) { return Math.floor(num); }
function round(num) { return Math.round(num); }
function lerp(perc, min, max) { return lerp_internal(perc, min, max); }
function norm(value, min, max) { return norm_internal(value, min, max); }
function map(value, sourceMin, sourceMax, destMin, destMax, ease) { return map_internal(value, sourceMin, sourceMax, destMin, destMax, ease === undefined ? LINEAR : ease); }
function clampMap(value, sourceMin, sourceMax, destMin, destMax, ease) { return clampMap_internal(value, sourceMin, sourceMax, destMin, destMax, ease === undefined ? LINEAR : ease); }

function clamp(value, min, max) {
	if (value > max) value = max;
	if (value < min) value = min;
	return value;
}


function saveCheckpoint() {
	checkpointStr = JSON.stringify(data);
}

function saveGame() {
	msg("Game Saved");
	playEffect("audio/ui/rewards/checkmark");
	saveGame_internal(checkpointStr);
}

function loadGame() {
	msg("Loading game...");

	//Legacy world maps
	if (data.metCart) {
		worldMap.unlockEntry("Femme Ren's");
		worldMap.unlockEntry("Moving In");
		worldMap.unlockEntry("Yellow Pond");
		worldMap.unlockEntry("Visit L.L.L.");
		worldMap.unlockEntry("Wet Transmission");
		worldMap.unlockEntry("Archiepelago");
	}

	if (data.heardTransmission) {
		worldMap.unlockEntry("Wet Cartography");
	}

	if (data.metRen) {
		worldMap.unlockEntry("Bullying Lone");
	}

	if (data.leftMynt) {
		worldMap.unlockEntry("Marshmelon Mess");
		worldMap.unlockEntry("Olitippo's Stream");
		worldMap.unlockEntry("Foodland");
		worldMap.unlockEntry("Mynt's Day Off");
	}

	if (data.metPatch) {
		worldMap.unlockEntry("Indigo Pond");
	}

	loadGame_internal();
}

function loadMod() {
	msg("Loading mod...");
	loadMod_internal();
}

function pointDistance(x1, y1, x2, y2) { return Math.sqrt((Math.pow(x2-x1, 2))+(Math.pow(y2-y1, 2))); }

function setBackground(bgNum, assetId, bgType) {
	if (!bgType) bgType = DEFAULT;

	if (bgNum > 10) {
		//@robustness Silent fail
		print("No more than 10 bgs");
		return;
	}

	function createBg() {
		if (assetId == "") return;
		var bgSprite;
		if (bgType == DEFAULT) {
			bgSprite = addImage(assetId);
		} else if (bgType == TILED) {
			bgSprite = addCanvasImage(assetId, gameWidth, gameHeight);
			var imgW = getTextureWidth_internal(assetId);
			var imgH = getTextureHeight_internal(assetId);
			var rows = (gameWidth / imgW) + 1;
			var cols = (gameHeight / imgH) + 1;

			for (var row = 0; row < rows; row++) {
				for (var col = 0; col < cols; col++) {
					bgSprite.copyPixels(0, 0, imgW, imgH, row * imgW, col * imgH);
				}
			}
		} else if (bgType == CENTERED) {
			bgSprite = addCanvasImage(assetId, gameWidth, gameHeight);
			var imgW = getTextureWidth_internal(assetId);
			var imgH = getTextureHeight_internal(assetId);

			bgSprite.copyPixels(0, 0, imgW, imgH, (gameWidth - imgW)/2, (gameHeight - imgH)/2);
		}
		bgSprite.temp = false;
		bgSprite.layer = BG_START_LAYER + bgNum;
		bgSprite.alpha = 0;
		tween(bgSprite, 0.2, {alpha: 1});

		var bg = {
			sprite: bgSprite,
			type: bgType,
			bobX: 0,
			bobY: 0,
		};

		backgrounds[bgNum] = bg;
	}

	if (backgrounds[bgNum]) {
		tween(backgrounds[bgNum].sprite, 0.2, {alpha: 0}, {onComplete: function() {
			if (backgrounds[bgNum]) backgrounds[bgNum].sprite.destroy();
			backgrounds[bgNum] = null;
			createBg();
		}});
	} else {
		createBg();
	}
}

function resetBackgroundMode(bgNum) {
	if (backgrounds.length <= bgNum) return;
	if (!backgrounds[bgNum]) return;

	backgrounds[bgNum].bobX = 0;
	backgrounds[bgNum].bobY = 0;
}

function tween(src, time, params, config) {
	if (!config) config = {};
	if (config.ease === undefined) config.ease = LINEAR;
	if (config.reversed === undefined) config.reversed = false;
	if (config.startDelay === undefined) config.startDelay = 0;

	var startParams = {};
	for (key in params) startParams[key] = src[key];

	var tw;
	tw = {
		source: src,
		totalTime: time,
		elapsed: -config.startDelay,
		startParams: startParams,
		params: params,
		config: config,
		reset: function() {
			tw.elapsed = 0;
			for (key in tw.params) {
				tw.source[key] = tw.startParams[key];
			}

			if (tweens.indexOf(tw) == -1) tweens.push(tw);
		},
		cancel: function() {
			var index = tweens.indexOf(tw);
			if (index > 0) tweens.splice(tw, 1);
		}
	};

	tweens.push(tw);
	return tw;
}

function cancelTweens(source) {
	var tweensToRemove = [];
	tweens.forEach(function(tw) {
		if (tw.source == source) tweensToRemove.push(tw);
	});

	tweensToRemove.forEach(function(tw) {
		arrayRemove(tweens, tw);
	});
}

function setTitle(text) {
	tween(titleTf, 0.12, {scaleY: 0}, {ease: QUINT_OUT});
	tween(titleTf, 0.1, {alpha: 0}, {onComplete: function() {
		titleTf.setText(text);
		tween(titleTf, 0.1, {alpha: 1});
		tween(titleTf, 0.12, {scaleY: 1}, {ease: QUINT_OUT});
		playEffect("audio/ui/myntTalk");
	}});
}

function queueTitle(str) {
	var command = {};
	command.type = "title";
	command.content = str;
	command.addedTime = str.length/40*3 + 0.5;
	if (command.addedTime < 0.25) command.addedTime = 0.25;
	command.skippable = true;
	queuedCommands.push(command);
}

function queueAddChoice(choiceText, result) {
	if (result === undefined) result = choiceText;

	var command = {};
	command.type = "addChoice";
	command.choiceText = choiceText;
	command.result = result;
	command.addedTime = 0;
	queuedCommands.push(command);
}

function queueCall(func) {
	var command = {};
	command.type = "call";
	command.func = func;
	command.addedTime = 0;
	queuedCommands.push(command);
}

function queueDelay(amount) {
	var command = {};
	command.type = "delay";
	command.skippable = true;
	command.addedTime = amount;
	queuedCommands.push(command);
}

function queueSkippableDelay(amount) {
	var command = {};
	command.type = "delay";
	command.addedTime = amount;
	command.skippable = true;
	queuedCommands.push(command);
}

function queuePause() {
	var command = {};
	command.type = "pause";
	command.skippable = true;
	queuedCommands.push(command);
}

function queueStop() {
	var command = {};
	command.type = "stop";
	queuedCommands.push(command);
}

function skipCurrentCommand() {
	queueTimeLeft = 0;
}

function enableExit() {
	exitDisabled = false;
	exitButton.alpha = 1;
}

function disableExit() {
	exitDisabled = true;
	exitButton.alpha = 0;
}

function msg(str, config) {
	if (config === undefined) config = {};
	if (config.smallFont === undefined) config.smallFont = false;
	if (config.hugeTexture === undefined) config.hugeTexture = false;
	if (config.extraTime === undefined) config.extraTime = 0;

	if (config.silent === undefined) playEffect("audio/ui/msg");

	var tf;

	if (config.hugeTexture) {
		tf = addEmptyImage(1024, 2048);
	} else {
		tf = addEmptyImage(256, 512);
	}

	if (config.smallFont) tf.setFont("NunitoSans-Light_22");
	tf.temp = false;
	tf.layer = MSG_TEXT_LAYER;
	tf.tint = 0xFFFFFFFF;
	tf.setText(str);

	var spr = add9SliceImage("img/writer/writerChoice.png", tf.textWidth + 32, tf.textHeight + 32, 5, 5, 10, 10);
	spr.temp = false;
	spr.layer = MSG_SPRITE_LAYER;
	spr.addChild(tf);

	spr.x = gameWidth - spr.width - 16;
	spr.y = gameHeight - BUTTON_HEIGHT;
	spr.alpha = 0;

	tf.x = spr.width/2 - tf.textWidth/2;
	tf.y = spr.height/2 - tf.textHeight/2;

	var message;
	message = {
		sprite: spr,
		textField: tf,
		timeShown: -config.extraTime,
	};

	msgs.push(message);
	return message;
}

function rawAppend(data) {
	append_internal(data);
}

function append(data) {
	if (typeof data !== "string") data = String(data);
	var newStr = "";

	var lines = data.split("\n");
	lines.forEach(function(line, i) {
		if (line.charAt(0) == "[") {
			var barIndex = line.indexOf("|");
			var closingBraceIndex = line.indexOf("]");
			var choiceName = "";
			var choiceDest = "";
			if (barIndex != -1) {
				choiceName = line.substring(1, barIndex);
				choiceDest = line.substring(barIndex+1, closingBraceIndex);
			} else {
				choiceName = line.substring(1, closingBraceIndex);
				choiceDest = choiceName;
			}

			var choiceParams = {};

			line.substring(closingBraceIndex+1, line.length).split("#").forEach(function(param) {
				param = param.trim();
				if (param.length == 0) return;
				if (!choiceParams.icons) choiceParams.icons = [];
				choiceParams.icons.push(param);
			});

			addChoice(choiceName, choiceDest, choiceParams);
		} else {
			newStr += line;
			if (i < lines.length-1) newStr += "\n";
		}
	});

	newStr = newStr.replace(/%/g, "%%"); // We won't need this once mainText is in js
	rawAppend(newStr);
}

function submitPassage(str) {
	var code = "";

	str = str.replace(/\r/g, "");
	while (str.charAt(0) == "\n") str = str.substr(1, str.length);

	var lines = str.split("\n");
	var name = lines.shift().replace(/^\s+|\s+$/g, "");
	name = name.substr(1, name.length);
	// print("Got name: "+name);

	var preCode = lines.join("\n");
	lines = preCode.split("`");

	var inCode = false;
	var noAppendNextCode = false;
	lines.forEach(function(line, i) {
		if (!inCode) {
			line = line.replace(/\"/g, "\\\"");
			if (line.charAt(line.length-1) == "!") {
				noAppendNextCode = true;
				line = line.substr(0, line.length-1);
			}
			line = line.replace(/\n/g, "\\n");
			code += "append(\""+line+"\");\n";
		} else {
			if (noAppendNextCode) {
				code += line;
			} else {
				if (line.charAt(line.length-1) == ";") line = line.substr(0, line.length-1);
			code += "append("+line+");\n";
			}
			noAppendNextCode = false;
		}
		inCode = !inCode;
	});
	
	// print("Got code:\n"+code);
	addPassage(name, code);
}

function whenDoneStreaming(fn) {
	doneStreamingFns.push(fn);
}

function registerIcon(iconName, iconPath) {
	iconDatabase[iconName] = iconPath;
}

function showTooltip(str) {
	var rebuildBg = false;
	if (str != tooltipTf.text) rebuildBg = true;

	tooltipTf.setText(str);

	if (rebuildBg) {
		if (tooltipBg) tooltipBg.destroy();
		tooltipBg = add9SliceImage("img/writer/writerChoice.png", tooltipTf.textWidth + 16, tooltipTf.textHeight + 16, 5, 5, 10, 10);
		tooltipBg.temp = false;
		tooltipBg.layer = TOOLTIP_SPRITE_LAYER;
		tooltipTf.addChild(tooltipBg);
	}

	tooltipShowing = true;
}

function keyJustPressed(keyCode) {
	return keys[keyCode] == KEY_JUST_PRESSED;
}

function getUrl(url) {
	getUrl_internal(url);
}

function hookTempUpdate(fn) {
	tempUpdateFunctions.push(fn);
}

function hookUpdate(fn) {
	updateFunctions.push(fn);
}

function __update() {
	try {
		realUpdate();
	} catch (e) {
		print(e.stack);
		msg(String(e));
		msg(String(e.stack), {smallFont: true, hugeTexture: true, extraTime: 20});
	}
}

function realUpdate() {
	/// Misc
	var elapsed = 1/60;

	/// Callbacks
	while (doneStreamingFns.length > 0 && assetStreamsLeft == 0) doneStreamingFns.shift()();
	tempUpdateFunctions.forEach(function(fn) {
		fn();
	});

	updateFunctions.forEach(function(fn) {
		fn();
	});

	/// Image mouse events
	images.forEach(function(img) {
		var data = getImageProps_internal(img.id);
		img.justPressed = data[0];
		img.justReleased = data[1];
		img.pressing = data[2];
		img.justHovered = data[3];
		img.justUnHovered = data[4];
		img.hovering = data[5];
		img.currentFrame = data[6];

		if (img.justReleased && img.onRelease) img.onRelease();
		if (img.justHovered && img.onHover) img.onHover();
		if (img.hovering && img.whileHovering) img.whileHovering();
		if (img.justUnHovered && img.onUnHover) img.onUnHover();
	});

	/// Tooltips
	if (tooltipShowing) { 
		tooltipTf.alpha += 0.05;
		tooltipBg.x = tooltipTf.textWidth/2 - tooltipBg.width/2;
		tooltipBg.y = tooltipTf.textHeight/2 - tooltipBg.height/2;

		tooltipTf.x = mouseX;
		tooltipTf.y = mouseY;

		if (tooltipTf.x + tooltipBg.x + tooltipBg.width > gameWidth) tooltipTf.x -= tooltipBg.width;
		if (tooltipTf.y + tooltipBg.y + tooltipBg.height > gameHeight) tooltipTf.y -= tooltipBg.height;
	} else {
		tooltipTf.alpha -= 0.05;
	}
	tooltipShowing = false;

	/// Input field
	if (keyboardOpened) inputFieldBg.y = 20
	else inputFieldBg.y = gameHeight - BUTTON_HEIGHT - inputFieldBg.height - 32;

	inputField.x = inputFieldBg.width/2 - inputField.textWidth/2;
	inputField.y = inputFieldBg.height/2 - inputField.textHeight/2;
	if (isAndroid && inputField.justReleased && inputFieldBg.alpha == 1) {
		keyboardOpened = true;
		openSoftwareKeyboard();
	}

	inputTitle.x = inputField.x - inputTitle.textWidth - 16;
	inputTitle.y = inputFieldBg.height/2 - inputTitle.textHeight/2;

	inputCarrot.x = inputField.x + inputField.textWidth;
	inputCarrot.y = inputField.y + inputField.textHeight/2 - inputCarrot.height/2;
	inputCarrot.alpha -= 0.02;
	if (inputCarrot.alpha <= 0) inputCarrot.alpha = 1;

	/// Msgs
	var msgsToDestroy = [];
	var msgPad = 16;

	var moveMsgsUp = false;
	for (var i = 0; i < msgs.length; i++)
		if (msgs[i].sprite.y + msgs[i].sprite.height > gameHeight - BUTTON_HEIGHT)
			moveMsgsUp = true;

	for (var i = 0; i < msgs.length; i++) {
		var msg = msgs[i];
		var spr = msgs[i].sprite;

		msg.timeShown += elapsed;
		if (spr.justReleased) msg.timeShown = 99;

		if (msg.timeShown > 5) {
			msg.sprite.alpha -= 0.2;
			if (msg.sprite.alpha <= 0) msgsToDestroy.push(msg);
		} else {
			msg.sprite.alpha += 0.2;
		}

		if (i == 0) {
			if (moveMsgsUp) spr.y -= 5;
		} else {
			var prevSpr = msgs[i-1].sprite;
			var destY = prevSpr.y + prevSpr.height + msgPad;

			if (spr.y < destY) spr.y = destY;

			spr.y -= 5;
		}
	}

	for (var i = 0; i < msgsToDestroy.length; i++) {
		var msg = msgsToDestroy[i];
		msg.sprite.destroy();
		msg.textField.destroy();
		var index = msgs.indexOf(msg);
		msgs.splice(index, 1);
	}

	/// Command queue
	if (mouseJustDown || keys[32] == KEY_JUST_PRESSED) commandSkipped = true;
	if (currentCommand && currentCommand.skippable && commandSkipped) queueTimeLeft = 0;
	commandSkipped = false;

	queueTimeLeft -= 1/60;
	if (queueTimeLeft <= 0) {
		if (queuedCommands.length > 0) {
			var command = queuedCommands.shift();

			if (command.type === "title") setTitle(command.content);
			if (command.type === "addChoice") addChoice(command.choiceText, command.result);
			if (command.type === "call") command.func();
			if (command.type === "delay") {
				// Nothing
			}
			if (command.type === "pause") command.addedTime = 99999;
			if (command.type === "stop") command.addedTime = 99999;

			currentCommand = command;
			queueTimeLeft = command.addedTime;

			if (queuedCommands.length > 0 && queuedCommands[0].type == "addChoice") queueTimeLeft = 0;
		}
	}

	/// Title
	if (titleTf.text == "") {
		titleBg.alpha -= 0.05;
	} else {
		titleBg.alpha += 0.05;
		titleTf.x = titleBg.width/2 - titleTf.textWidth/2;
		titleTf.y = titleBg.height/2 - titleTf.textHeight/2;
	}

	/// Backgrounds
	backgrounds.forEach(function(bg) {
		if (!bg) return;
		bg.sprite.x = Math.sin(time * bg.bobX) * 30;
		bg.sprite.y = Math.sin(time * bg.bobY) * 30;
	});

	/// Tweens
	// var start = performance.now();
	var tweensToRemove = [];
	if (false) {
		iterTweens_internal(tweens);
		tweens.forEach(function(tw) {
			if (tw.source.exists === false) {
				tweensToRemove.push(tw);
				return;
			}

			var perc = tw.elapsed / tw.totalTime;
			if ((perc >= 1 && !tw.config.reversed) || (perc <= 0 && tw.config.reversed)) {
				if (tw.config.type == LOOPING) {
					tw.elapsed = 0;
				} else if (tw.config.type == PINGPONG) {
					tw.elapsed = 0;
					tw.config.reversed = !tw.config.reversed;
				} else if (tw.config.type == PINGPONG_ONCE) {
					tw.elapsed = 0;
					tw.config.reversed = !tw.config.reversed;
					tw.config.type = null;
				} else {
					if (tw.config.onComplete) tw.config.onComplete();
					tweensToRemove.push(tw);
				}
			}
		});

	} else {
		tweens.forEach(function(tw) {
			if (tw.source.exists === false) {
				tweensToRemove.push(tw);
				return;
			}

			if (tw.elapsed < 0) {
				tw.elapsed += elapsed;
				return;
			}

			var perc = tw.elapsed / tw.totalTime;
			if (tw.config.reversed) perc = 1 - perc;
			var realPerc = perc;
			perc = tweenEase(perc, tw.config.ease);
			perc = round(perc * 1000) / 1000;

			for (key in tw.params) {
				var min = tw.startParams[key];
				var max = tw.params[key];
				tw.source[key] = min + (max - min) * perc;
			}
			tw.elapsed += elapsed;

			if ((realPerc >= 1 && !tw.config.reversed) || (realPerc <= 0 && tw.config.reversed)) {
				if (tw.config.type == LOOPING) {
					tw.elapsed = 0;
				} else if (tw.config.type == PINGPONG) {
					tw.elapsed = 0;
					tw.config.reversed = !tw.config.reversed;
				} else if (tw.config.type == PINGPONG_ONCE) {
					tw.elapsed = 0;
					tw.config.reversed = !tw.config.reversed;
					tw.config.type = null;
				} else {
					if (tw.config.onComplete) tw.config.onComplete();
					tweensToRemove.push(tw);
				}
			}
		});
	}

	tweensToRemove.forEach(function(tw) {
		var index = tweens.indexOf(tw);
		if (index != -1) tweens.splice(index, 1);
	});
	// var end = performance.now();
	// print("start: "+start+" end: "+end+" sub: "+(end-start));

	/// Timers
	var timersToRemove = [];
	for (var i = 0; i < timers.length; i++) {
		var tim = timers[i];
		tim.timeLeft -= elapsed;
		if (tim.timeLeft <= 0) {
			tim.onComplete();
			tim.loopCount--;
			if (tim.loopCount == 0) {
				timersToRemove.push(tim);
			} else {
				tim.timeLeft = tim.delay;
			}
		}
	}

	for (var i = 0; i < timersToRemove.length; i++) {
		var index = timers.indexOf(timersToRemove[i]);
		if (index != -1) timers.splice(index, 1);
	}

	/// Choices
	prevChoices.alpha = nextChoices.alpha = choices.length > choicesPerPage ? 1 : 0;

	for (var i = 0; i < choices.length; i++) choices[i].sprite.y = gameHeight + 256; // Go way down there!

	var minChoice = choicePage * choicesPerPage;
	var maxChoice = minChoice + choicesPerPage;
	if (maxChoice > choices.length) maxChoice = choices.length;
	var choiceIndexOnPage = 0;
	for (var i = minChoice; i < maxChoice; i++) {
		var choice = choices[i];
		var spr = choice.sprite;
		var tf = choice.textField;

		var choicesWidth = choice.sprite.width * choicesPerPage;
		var choicesOff = gameWidth/2 - choicesWidth/2;

		spr.x = spr.width * choiceIndexOnPage + choicesOff;
		spr.y = gameHeight - spr.height;
		spr.scaleX = spr.scaleY = 1;
		spr.alpha = choice.enabled ? 1 : 0.5;
		spr.layer = CHOICE_BUTTON_LAYER;
		tf.layer = CHOICE_TEXT_LAYER;
		spr.smoothing = true;
		tf.smoothing = true;
		if (spr.justHovered) playEffect("audio/ui/hoverChoiceButtons");
		if (spr.hovering) {
			var moveUpPerc = spr.hoveringTime / 0.3;
			if (moveUpPerc > 1) moveUpPerc = 1;
			moveUpPerc = tweenEase(moveUpPerc, ELASTIC_OUT);

			spr.y -= moveUpPerc * 10;
			spr.scaleX += moveUpPerc*0.2;
			spr.scaleY += moveUpPerc*0.2;

			spr.layer = CHOICE_BUTTON_UPPER_LAYER;
			tf.layer = CHOICE_TEXT_UPPER_LAYER;
		}
		choiceIndexOnPage++;
	}

	if (prevChoices.hovering) prevChoices.y = gameHeight - prevChoices.height + 5
	else prevChoices.y = gameHeight - prevChoices.height;

	if (nextChoices.hovering) nextChoices.y = gameHeight - nextChoices.height + 5
	else nextChoices.y = gameHeight - nextChoices.height;

	if (nextChoices.justHovered || prevChoices.justHovered) playEffect("audio/ui/hoverChoiceButtons");

	/// Images
	images.forEach(function(img) {
		if (img.draggable) {
			if (img.justPressed) {
				img.dragPivotX = mouseX - img.x;
				img.dragPivotY = mouseY - img.y;
			}
			if (img.pressing) {
				img.x = mouseX - img.dragPivotX;
				img.y = mouseY - img.dragPivotY;
			}
		}
		img.alpha = clamp(img.alpha, 0, 1);
		img.skewX = clamp(img.skewX, -1, 1);
		img.skewY = clamp(img.skewY, -1, 1);

		if (img.inInputField) {
			img.x = round(img.x);
			img.y = round(img.y);

			if (keys[32] == KEY_JUST_RELEASED) img.setText(img.text + " ");
			if (keys[KEY_BACKSPACE] == KEY_JUST_RELEASED) img.setText(img.text.substring(0, img.text.length-1));

			for (var j = 65; j <= 90; j++) {
				if (keys[j] == KEY_JUST_PRESSED) {
					var ch = String.fromCharCode(j);
					if (keys[KEY_SHIFT] == KEY_RELEASED || keys[KEY_SHIFT] == KEY_JUST_RELEASED) ch = ch.toLowerCase();
					img.text += ch;
					img.setText(img.text);
				}
			}
		}

		if (img.hovering) {
			img.hoveringTime += 1/60.0;
		} else {
			img.hoveringTime = 0;
		}

		setImageProps(img.id, img.x, img.y, img.scaleX, img.scaleY, img.skewX, img.skewY, img.alpha, img.rotation, img.tint, img.layer, img.smoothing, img.centerPivot, img.ignoreMouseRect);
	});

	/// Audios
	for (var i = 0; i < audios.length; i++) {
		var audio = audios[i];
		setAudioFlags(audio.id, audio.looping, audio.volume);
	}
}

for (var i = 0; i < 500; i++) keys[i] = KEY_RELEASED;

execAsset("interStart.phore");
