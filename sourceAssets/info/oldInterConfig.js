function arrayShift(arr) {
	var ret = arr[0];
	for (var i = 1; i < arr.length; i++) {
		arr[i-1] = arr[i];
	}

	arr.splice(arr.length-1);

	return ret;
}

var images = [];
var timers = [];
var data = {};
var dataStr;
var lastInput = null;
var imageInstanceNumber = 0;
var gameWidth = 1280; //@incomplte Don't hard code
var gameHeight = 720; //@incomplte Don't hard code

var CENTER = "CENTER";
var TOP = "TOP";
var BOTTOM = "BOTTOM";
var LEFT = "LEFT";
var RIGHT = "RIGHT";

var NORMAL = 0;
var TILED = 1;
var CENTERED = 2;

var print = ffi("void print(char *)");
var submitPassage = ffi("void submitPassage(char *)");
var submitImage = ffi("void submitImage(char *)");
var exitMod = ffi("void exitMod()");
var clear = ffi("void clear()");

var gotoPassage_internal = ffi("void gotoPassage(char *)");
function gotoPassage(passageName, clearScreen) {
	if (clearScreen !== true) clear();
	gotoPassage_internal(passageName);
}

var append_internal = ffi("void append(char *)");
function append(data) { 
	if (data !== undefined) {
		if (typeof data === "string") {
			append_internal(data);
		} else {
			append_internal(JSON.stringify(data));
		}
	}
}

var getImageWidth = ffi("int getImageWidth(char *)");
var getImageHeight = ffi("int getImageHeight(char *)");
var getImageX = ffi("float getImageX(char *)");
var getImageY = ffi("float getImageY(char *)");

var addRectImage_internal = ffi("void addRectImage(char *, int, int, int)");
function addRectImage(width, height, colour) {
	var img = newImage();
	addRectImage_internal(img.name, width, height, colour);
	img.width = width;
	img.height = height;

	return img;
}

var addImage_internal = ffi("void addImage(char *, char *)");
function addImage(path, name) {
	if (name === undefined) name = "img"+(imageInstanceNumber++);

	addImage_internal(path, name);

	var img = newImage();
	img.name = name;
	img.width = getImageWidth(name);
	img.height = getImageHeight(name);

	return img;
}

function newImage() {
	var img = {
		name: "img"+(imageInstanceNumber++),
		x: 0,
		y: 0,
		width: 0,
		height: 0,
		rotation: 0,
		tint: 0x000000,
		alpha: 1,
		scaleX: 1,
		scaleY: 1,
		onReleaseOnceDepricated: null
	};

	images.push(img);

	return img;
}

var removeImage_internal = ffi("void removeImage(char *)");
function removeImage(name) {
	removeImage_internal(name);

	for (var i = 0; i < images.length; i++) {
		if (images[i].name === name) {
			images.splice(i, 1);
			return;
		}
	}

}

function removeAllImages() {
	while (images.length > 0) removeImage(images[0].name);
}

// let myArr = [10, 20, 25];
// for (let val in myArr) print("Value is: "+val);

function getImageByName(name) {
	// for (let curImg in images) {
	// 	if (curImg.name === name) return curImg;
	// }
	for (var i = 0; i < images.length; i++) {
		if (images[i].name === name) return images[i];
	}

	return null;
}

var alignImage_internal = ffi("void alignImage(char *, char *)");
function alignImage(imgName, dir) {
	if (!dir) dir = CENTER;
	alignImage_internal(imgName, dir);

	var img = getImageByName(imgName);
	img.x = getImageX(imgName);
	img.y = getImageY(imgName);
}

var moveImage = ffi("void moveImage(char *, float, float)");
var scaleImage = ffi("void scaleImage(char *, float, float)");
var rotateImage = ffi("void rotateImage(char *, float)");
var tintImage = ffi("void tintImage(char *, int)");

var submitAudio = ffi("void submitAudio(char *)");

var setAudioLooping = ffi("void setAudioLooping(char *, int)");
var playAudio_internal = ffi("void playAudio(char *, char *)");
function playAudio(path, name, config) {
	playAudio_internal(path, name);

	if (config) {
		if (config.looping) {
			config.looping = config.looping === false ? 0 : 1;
			setAudioLooping(name, config.looping);
		}
	}
}

var stopAudio = ffi("void stopAudio(char *)");

var moveImagePx = ffi("void moveImagePx(char *, float, float)");
var permanentImage = ffi("void permanentImage(char *)");

var submitNode = ffi("void submitNode(char *, char *)");
var attachNode = ffi("void attachNode(char *, char *, char *)");

var getTime = ffi("float getTime()");
var addButtonIcon = ffi("void addButtonIcon(char *, char *)");

var setBackground_internal = ffi("void setBackground(int, char *, int)");
function setBackground(bgNum, assetId, type) {
	if (type === undefined) type = NORMAL;

	setBackground_internal(bgNum, assetId, type);
}

var addNotif = ffi("void addNotif(char *, char *)");
var gotoMap = ffi("void gotoMap()");
var rnd = ffi("float rnd()");
var floor = ffi("int floor(float)");
var round = ffi("int round(float)");

function rndInt(min, max) {
	return round(rndFloat(min, max));
}

function rndFloat(min, max) {
	return min + rnd() * (max - min);
}

var addChoice_internal = ffi("void addChoice(char *, void (*)(userdata), userdata");
function addChoice(text, dest, config) {

	if (dest === undefined) dest = text;
	if (typeof dest === "string") {
		addChoice_internal(text, gotoPassage, dest);
	} else {
		addChoice_internal(text, dest, null);
	}

	if (config) {
		if (config.icons) {
			for (var i = 0; i < config.icons.length; i++) {
				addButtonIcon(text, config.icons[i]);
			}
		}
	}
}

var timer_internal = ffi("int timer(float, void (*)(userdata), userdata)");
function timer(delay, onComplete) {
	return timer_internal(delay, onComplete, null);
}

var setNodeLocked = ffi("void setNodeLocked(char *, char *)");

var streamAsset = ffi("void streamAsset(char *, char *)");
var execAsset = ffi("void execAsset(char *)");
var setTitle = ffi("void setTitle(char *)");

var setBackgroundBob = ffi("void setBackgroundBob(int, float, float);");
var resetBackgroundMode = ffi("void resetBackgroundMode(int);");

var addInputField = ffi("void addInputField();");
var clearNodes = ffi("void clearNodes();");

var enableExit = ffi("void enableExit();");
var disableExit = ffi("void disableExit();");
var gotoBrowser = ffi("void gotoBrowser();");
var loadModFromDisk = ffi("void loadModFromDisk();");

var isImageReleasedOnce = ffi("bool isImageReleasedOnce(char *);");

var queuedCommands = [];
var queueTimeLeft = 0;
var commandSkipped = false;
var currentCommand = null;

var updateFunctions = [];
function __update() {
	// print("I was called\n");

	if (currentCommand && currentCommand.skippable === true && commandSkipped) queueTimeLeft = 0;
	commandSkipped = false;

	queueTimeLeft -= 1/60;
	if (queueTimeLeft <= 0) {
		if (queuedCommands.length > 0) {
			var command = arrayShift(queuedCommands);

			if (command.type === "title") setTitle(command.content);
			if (command.type === "addChoice") addChoice(command.choiceText, command.result);
			if (command.type === "call") command.func();

			currentCommand = command;
			queueTimeLeft = command.addedTime;
		}
	}

	for (var i = 0; i < updateFunctions.length; i++) {
		if (updateFunctions[i]) updateFunctions[i]();
	}

	for (var i = 0; i < images.length; i++) {
		var img = images[i];
		moveImagePx(img.name, img.x, img.y);
		rotateImage(img.name, img.rotation);
		scaleImage(img.name, img.scaleX, img.scaleY);
		tintImage(img.name, img.tint);

		var justRel = isImageReleasedOnce(img.name);
		if (justRel && img.onReleaseOnceDepricated !== null) {
			img.onReleaseOnceDepricated();
		}
	}
}

function queueTitle(str) {
	var command = {};
	command.type = "title";
	command.content = str;
	command.addedTime = str.length/40*3 + 0.5;
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
// var prev = 0;
// function testOnFrame() {
// 	var cur = getTime();
// 	print("Elapsed: "+(cur-prev));
// 	prev = cur;
// }

// updateFunctions.push(testOnFrame);
print("Base js loaded\n");
