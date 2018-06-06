var images = [];
var audios = [];
var choices = [];
var timers = [];
var tweens = [];
var backgrounds = [];
var keys = [];
var msgs = [];

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
var lastInput = "";
var choicePage = 0;

var queuedCommands = [];
var queueTimeLeft = 0;
var commandSkipped = false;
var currentCommand = null;

var BG_START_LAYER = 10;
var MAIN_TEXT_LAYER = 20;
var GRAPH_BG_LAYER = 30; //@todo Remove
var GRAPH_LINE_LAYER = 40;
var GRAPH_NODE_LAYER = 50;
var DEFAULT_LAYER = 60;
var CHOICE_BUTTON_LAYER = 70;
var CHOICE_TEXT_LAYER = 80;
var MSG_SPRITE_LAYER = 90;
var MSG_TEXT_LAYER = 100;
var TITLE_LAYER = 110;

var CHOICES_PER_PAGE = 4;
var BUTTON_HEIGHT = 256;

var TOP = 1;
var BOTTOM = 2;
var LEFT = 3;
var RIGHT = 4;

var DEFAULT = 5;
var TILED = 6;
var CENTERED = 7;

var LOOPING = 8;
var PINGPONG = 9;

function newImage() {
	var img;
	img = {
		id: -1,
		x: 0,
		y: 0,
		width: 0,
		height: 0,
		rotation: 0,
		tint: 0x00000000,
		alpha: 1,
		scaleX: 1,
		scaleY: 1,

		text: "", //@cleanup Consider removing this
		textWidth: 0,
		textHeight: 0,
		layer: DEFAULT_LAYER,
		inInputField: false,
		setText: function(text) {
			img.text = text;
			setImageText(img, text);
			if (isFlash && img.tint == 0) img.tint = 0xFFFFFFFF;
		},
		setFont: function(fontName) {
			setImageFont(img.id, fontName);
		},

		justPressed: false,
		justReleased: false,
		pressing: false,
		justHovered: false,
		justUnHovered: false,
		hovering: false,
		draggable: false,
		dragPivotX: 0,
		dragPivotY: 0,
		onRelease: null,
		onHover: null,
		onUnHover: null,

		temp: true,
		destroy: function() {
			destroyImage(img.id);
			var index = images.indexOf(img);
			images.splice(index, 1);
		},
		addChild: function(otherImg) {
			addChild_internal(img.id, otherImg.id);
		},

		totalFrames: 0,
		gotoFrame: function(frame) {
			if (typeof frame === "string") {
				gotoFrameNamed(img.id, frame);
			} else {
				gotoFrameNum(img.id, frame);
			}
		},

		copyPixels: function(sx, sy, sw, sh, dx, dy) {
			copyPixels_internal(img.id, sx, sy, sw, sh, dx, dy);
		}
	};

	images.push(img);
	return img;
}

function getImageProps(id, index) {
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
	var choice = {
		sprite: null,
		textField: null,
		result: result
	};

	var spr = add9SliceImage("img/writer/writerChoice.png", 256, BUTTON_HEIGHT, 5, 5, 10, 10);
	spr.temp = false;
	spr.layer = CHOICE_BUTTON_LAYER;
	spr.onHover = function() {
		playEffect("audio/ui/hoverChoiceButtons");
	}
	spr.onRelease = function() {
		playEffect("audio/ui/bestChoiceClick");
		if (typeof choice.result === "string") {
			gotoPassage(choice.result);
		} else {
			choice.result();
		}
	}

	var tf = addEmptyImage(spr.width, spr.height);
	spr.addChild(tf);
	tf.temp = false;
	tf.setFont("NunitoSans-Light_22");
	tf.setText(choiceText);
	tf.layer = CHOICE_TEXT_LAYER;
	tf.x = spr.width/2 - tf.textWidth/2;
	tf.y = spr.height/2 - tf.textHeight/2;

	if (config) {
		if (config.icons) {
			for (var i = 0; i < config.icons.length; i++) {
				var icon = addImage("writer/icon.png");
				icon.gotoFrame(config.icons[i]);
				icon.x = i * icon.width;
				icon.layer = CHOICE_TEXT_LAYER;
				spr.addChild(icon);
			}
		}
	}

	choice.sprite = spr;
	choice.textField = tf;

	choices.push(choice);
	return choice;
}

function addInputField() {
	inputField.alpha = 1;
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
			var index = audios.indexOf(audio);
			if (index != -1) audios.splice(index, 1);
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
	var audio = newAudio();
	audio.id = playEffect_internal(assetId);
	audios.push(audio);
	return audio;
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

function clear() {
	setMainText("");
	choicePage = 0;
	exitButton.alpha = exitDisabled ? 0 : 1; //@todo This should probably happen instantly
	lastInput = inputField.text;
	inputField.inInputField = false;
	inputField.alpha = 0;

	var imagesToRemove = [];
	for (var i = 0; i < images.length; i++) if (images[i].temp) imagesToRemove.push(images[i]);
	for (var i = 0; i < imagesToRemove.length; i++) imagesToRemove[i].destroy();

	for (var i = 0; i < choices.length; i++) {
		choices[i].sprite.destroy();
		choices[i].textField.destroy();
	}

	choices = [];
}

function gotoPassage(passageName) {
	clear();
	gotoPassage_internal(passageName);
}

function timer(delay, onComplete) {
	var tim;
	tim = {
		delay: delay,
		timeLeft: delay,
		onComplete: onComplete
	};

	timers.push(tim);
}

function rnd() { return Math.random(); }
function rndInt(min, max) { return round(rndFloat(min, max)); }
function rndFloat(min, max) { return min + rnd() * (max - min); }
function floor(num) { return Math.floor(num); }
function round(num) { return Math.round(num); }

function saveCheckpoint() {
	checkpointStr = JSON.stringify(data);
}

function saveGame() {
	msg("Game Saved");
	saveGame_internal(checkpointStr);
}

function loadGame() {
	msg("Game Loaded");
	loadGame_internal();
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
		tween(bgSprite, 0.5, {alpha: 1});

		var bg = {
			sprite: bgSprite,
			type: bgType,
			bobX: 0,
			bobY: 0,
		};

		backgrounds[bgNum] = bg;
	}

	if (backgrounds[bgNum]) {
		tween(backgrounds[bgNum].sprite, 0.5, {alpha: 0}, {onComplete: function() {
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

function setBackgroundBob(bgNum, bobX, bobY) {
	//@incomplete Remove this
	// print("This is going away, don't call this!");
	backgrounds[bgNum].bobX = bobX;
	backgrounds[bgNum].bobY = bobY;
}

function tween(src, time, params, config) {
	if (!config) config = {};
	if (config.ease == undefined) config.ease = LINEAR;
	if (config.reversed == undefined) config.reversed = false;

	var startParams = {};
	for (key in params) startParams[key] = src[key];

	var tw;
	tw = {
		source: src,
		totalTime: time,
		elapsed: 0,
		startParams: startParams,
		params: params,
		config: config
	};

	tweens.push(tw);
	return tw;
}

function setTitle(text) {
	tween(titleTf, 0.25, {alpha: 0}, {onComplete: function() {
		titleTf.setText(text);
		tween(titleTf, 0.25, {alpha: 1});
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

function enableExit() {
	exitDisabled = false;
	exitButton.alpha = 1;
}

function disableExit() {
	exitDisabled = true;
	exitButton.alpha = 0;
}

function msg(str) {
	var tf = addEmptyImage(256, 256);
	tf.temp = false;
	tf.layer = MSG_TEXT_LAYER;
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
		timeShown: 0,
	};

	msgs.push(message);
	return message;
}

function __update() {
	var elapsed = 1/60;

	/// Misc
	inputField.x = gameWidth/2 - inputField.textWidth/2;

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
	if (keys[32] == KEY_JUST_PRESSED) commandSkipped = true;
	if (currentCommand && currentCommand.skippable && commandSkipped) queueTimeLeft = 0;
	commandSkipped = false;

	queueTimeLeft -= 1/60;
	if (queueTimeLeft <= 0) {
		if (queuedCommands.length > 0) {
			var command = queuedCommands.shift();

			if (command.type === "title") setTitle(command.content);
			if (command.type === "addChoice") addChoice(command.choiceText, command.result);
			if (command.type === "call") command.func();

			currentCommand = command;
			queueTimeLeft = command.addedTime;
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
	var tweensToRemove = [];
	tweens.forEach(function(tw) {
		var perc = tw.elapsed / tw.totalTime;
		if (tw.config.reversed) perc = 1 - perc;
		perc = tweenEase(perc, tw.config.ease);

		if ((perc >= 1 && !tw.config.reversed) || (perc <= 0 && tw.config.reversed)) {
			if (tw.config.type == LOOPING) {
				tw.elapsed = 0;
			} else if (tw.config.type == PINGPONG) {
				tw.elapsed = 0;
				tw.config.reversed = !tw.config.reversed;
			} else {
				if (tw.config.onComplete) tw.config.onComplete();
				tweensToRemove.push(tw);
			}
		}
		tw.elapsed += elapsed;

		for (key in tw.params) {
			var min = tw.startParams[key];
			var max = tw.params[key];
			tw.source[key] = min + (max - min) * perc;
		}
	});

	tweensToRemove.forEach(function(tw) {
		var index = tweens.indexOf(tw);
		if (index != -1) tweens.splice(index, 1);
	});

	/// Timers
	var timersToRemove = [];
	for (var i = 0; i < timers.length; i++) {
		var tim = timers[i];
		tim.delay -= elapsed;
		if (tim.delay <= 0) {
			tim.onComplete();
			timersToRemove.push(tim);
		}
	}

	for (var i = 0; i < timersToRemove.length; i++) {
		var index = timers.indexOf(timersToRemove[i]);
		if (index != -1) timers.splice(index, 1);
	}

	/// Choices
	prevChoices.alpha = nextChoices.alpha = choices.length > CHOICES_PER_PAGE ? 1 : 0;

	for (var i = 0; i < choices.length; i++) choices[i].sprite.y = gameHeight;

	var minChoice = choicePage * CHOICES_PER_PAGE;
	var maxChoice = minChoice + 4;
	if (maxChoice > choices.length) maxChoice = choices.length;
	var choiceIndexOnPage = 0;
	for (var i = minChoice; i < maxChoice; i++) {
		var choice = choices[i];
		var spr = choice.sprite;
		var tf = choice.textField;

		var choicesWidth = choice.sprite.width * CHOICES_PER_PAGE;
		var choicesOff = gameWidth/2 - choicesWidth/2;

		spr.x = spr.width * choiceIndexOnPage + choicesOff;
		spr.y = gameHeight - spr.height;
		if (spr.hovering) spr.y -= 5;

		choiceIndexOnPage++;
	}

	/// Images
	images.forEach(function(img) {
		var data = getImageProps_internal(img.id);
		img.justPressed = data[0];
		img.justReleased = data[1];
		img.pressing = data[2];
		img.justHovered = data[3];
		img.justUnHovered = data[4];
		img.hovering = data[5];

		if (img.justReleased && img.onRelease) img.onRelease();
		if (img.justHovered && img.onHover) img.onHover();
		if (img.justUnHovered && img.onUnHover) img.onUnHover();

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
		if (img.alpha < 0) img.alpha = 0;
		if (img.alpha > 1) img.alpha = 1;

		if (img.inInputField) {
			var keyWasPressed = false;
			for (var j = 65; j < 90; j++) {
				if (keys[j] == KEY_JUST_PRESSED) {
					var ch = String.fromCharCode(j);
					if (keys[KEY_SHIFT] == KEY_RELEASED || keys[KEY_SHIFT] == KEY_JUST_RELEASED) ch = ch.toLowerCase();
					img.text += ch;
					img.setText(img.text);
				}
			}

			if (keys[32] == KEY_JUST_RELEASED) {
				img.text += " ";
				img.setText(img.text);
			}
		}

		setImageProps(img.id, img.x, img.y, img.scaleX, img.scaleY, img.alpha, img.rotation, img.tint, img.layer);
	});

	/// Audios
	for (var i = 0; i < audios.length; i++) {
		var audio = audios[i];
		setAudioFlags(audio.id, audio.looping, audio.volume);
	}
}

for (var i = 0; i < 500; i++) keys[i] = KEY_RELEASED;

execAsset("info/nodeGraph.phore");

var nextChoices = add9SliceImage("img/writer/writerChoice.png", 128, BUTTON_HEIGHT, 5, 5, 10, 10);
nextChoices.temp = false;
nextChoices.x = gameWidth - nextChoices.width;
nextChoices.y = gameHeight - nextChoices.height;
nextChoices.onRelease = function() {
	if (nextChoices.alpha != 1) return;
	var totalPages = choices.length / CHOICES_PER_PAGE;
	if (choicePage >= totalPages-1) return;
	choicePage++;
}

var nextArrow = addImage("choiceArrow.png");
nextArrow.temp = false;
nextChoices.addChild(nextArrow);
nextArrow.x = nextChoices.width/2 - nextArrow.width/2;
nextArrow.y = nextChoices.height/2 - nextArrow.height/2;

var prevChoices = add9SliceImage("img/writer/writerChoice.png", 128, BUTTON_HEIGHT, 5, 5, 10, 10);
prevChoices.y = gameHeight - prevChoices.height;
prevChoices.temp = false;
prevChoices.onRelease = function() {
	if (prevChoices.alpha != 1) return;
	if (choicePage <= 0) return;
	choicePage--;
}

var prevArrow = addImage("choiceArrow.png");
prevArrow.temp = false;
prevChoices.addChild(prevArrow);
prevArrow.scaleX = -1;
prevArrow.x = prevChoices.width/2 - prevArrow.width/2 + prevArrow.width;
prevArrow.y = prevChoices.height/2 - prevArrow.height/2;

var exitButton = addImage("writer/exit.png");
exitButton.temp = false;
exitButton.scaleX = exitButton.scaleY = 2;
exitButton.x = gameWidth - exitButton.width*exitButton.scaleX - 16;
exitButton.y = 16;
exitButton.onRelease = function() {
	if (exitButton.alpha != 1) return;
	gotoMap(false);
}

var titleBg = addRectImage(gameWidth, 50, 0x000000);
titleBg.temp = false;
titleBg.y = gameHeight * 0.55;
titleBg.layer = TITLE_LAYER;

var titleTf = addEmptyImage(titleBg.width, titleBg.height);
titleTf.temp = false;
titleTf.setFont("NunitoSans-Light_22");
titleTf.layer = TITLE_LAYER;
titleBg.addChild(titleTf);

var inputField = addEmptyImage(gameWidth, 50);
inputField.y = gameHeight * 0.45;
inputField.temp = false;
inputField.alpha = 0;
