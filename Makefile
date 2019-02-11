ASSET_PATH := sourceAssets
EXTRA_DEFINES := 
# EXTRA_DEFINES += -DFAST_SCRATCH
GAME_NAME := Interphore

NEEDED_FONTS := "Espresso-Dolce 22" \
									"Espresso-Dolce 30" \
									"Espresso-Dolce 38" \
									"Espresso-Dolce 44" \
									"OpenSans-Regular 20" \
									"OpenSans-Regular 30" \
									"OpenSans-Regular 35" \
									"OpenSans-Regular 40" \
									"NunitoSans-Light 22" \
									"NunitoSans-Bold 22" \
									"NunitoSans-Italic 22" \
									"NunitoSans-Light 26" \
									"NunitoSans-Bold 26" \
									"NunitoSans-Italic 26" \
									"NunitoSans-Light 30" \
									"NunitoSans-Bold 30" \
									"NunitoSans-Italic 30" \

CPP_TOOLS := ..
CPP_TOOLS_WIN := ..

# If you don't have the Makefile.secret file, comment the line below and fill in the args underneath
include ../other/Makefile.secret

# Needed to build the Flash version
# CPP_TOOLS_ABS := /c/Dropbox/cpp # The Cygwin path to the directory that contains the engine and game directories
# CPP_TOOLS_ABS_WIN := C:\Dropbox\cpp # The Windows path to the directory that contains the engine and game directories
# CROSSBRIDGE_PATH = /d/_tools/_sdks/crossbridge
# JAVA_MAKE_PATH := /d/_tools/_sdks/jdk8u144_x64
# AIR_MAKE_PATH := /d/_tools/_sdks/air
#
# Needed to build the Android version
# ANDROID_SDK_PATH := /d/_tools/_sdks/android-sdk
# ANT_PATH := /d/_tools/_sdks/ant
# PYTHON3 := /c/Users/$$USERNAME/AppData/Local/Programs/Python/Python36/python.exe
#
# Currently unnecessary
# EMSCRIPTEN_DIR := /d/_tools/_sdks/emsdk-portable/emscripten/1.37.21
# EMSCRIPTEN_DIR_WIN := D:\_tools/_sdks/emsdk-portable/emscripten/1.37.21
# FLASH_SO_DIR := /d/Users/$$USERNAME/AppData/Roaming/Macromedia/Flash\ Player/\#SharedObjects/6U22UWBZ/localhost/Dropbox/cpp
# PARAPHORE_COM_PATH := /d/paraphore.com

GAME_WIDTH=1280
GAME_HEIGHT=720
SCREEN_ORIENTATION := landscape

all:
	$(MAKE) defaultAll

fast:
	cp sourceAssets/info/*.phore bin/assets/info
	cp sourceAssets/info/*.js bin/assets/info
	cp sourceAssets/shader/* bin/assets/shader
	make r

exportAssets:
	# bash $(CPP_TOOLS)/engine/buildSystem/exportAssets.sh ../writerExportedAssets/img assets/img

resetSite:
	if [[ ! -d "$(PARAPHORE_COM_PATH)" ]]; then \
		git clone git@github.com:FallowWing/paraphore.com.git $(PARAPHORE_COM_PATH) --depth=1 --recurse; \
		fi
	
	cd $(PARAPHORE_COM_PATH) && \
		git fetch origin && \
		git reset --hard origin/master && \
		git pull

shipInterNewEarlyDir:
	$(MAKE) resetSite
	newDirName=$(PARAPHORE_COM_PATH)/play/early/`date | md5sum -- `; \
												newDirName=$${newDirName:0:-3}; \
												rm -rf $(PARAPHORE_COM_PATH)/play/early/*; \
												mkdir -p $$newDirName;
	$(MAKE) shipDir SHIP_DIR="$(PARAPHORE_COM_PATH)/play/early"
	
	$(MAKE) shipInterEarly
	dirName=`ls -d $(PARAPHORE_COM_PATH)/play/early/*`; \
									echo New url is $$dirName;

shipInterNewDevDir:
	$(MAKE) resetSite
	newDirName=$(PARAPHORE_COM_PATH)/play/dev/`date | md5sum -- `; \
												newDirName=$${newDirName:0:-3}; \
												rm -rf $(PARAPHORE_COM_PATH)/play/dev/*; \
												mkdir -p $$newDirName;
	$(MAKE) shipDir SHIP_DIR="$(PARAPHORE_COM_PATH)/play/dev"
	
	$(MAKE) shipInter
	dirName=`ls -d $(PARAPHORE_COM_PATH)/play/dev/*`; \
									echo New url is $$dirName;

shipInter:
	$(MAKE) resetSite
	
	cp res/currentMod.phore bin
	$(MAKE) boptflash EXTRA_DEFINES+="-D SEMI_DEV" SHIPPING=1
	$(MAKE) packWindows EXTRA_DEFINES+="-D SEMI_DEV" SHIPPING=1
	$(MAKE) bandroid EXTRA_DEFINES+="-D SEMI_DEV" SHIPPING=1
	dirName=`ls -d $(PARAPHORE_COM_PATH)/play/dev/*`; \
									cp $(PARAPHORE_COM_PATH)/play/interphore.html $$dirName/index.html; \
									cp bin/engine.swf $$dirName/interphore.swf; \
									cp bin/$(GAME_NAME).zip $$dirName/interphore.zip; \
									cp bin/engine.apk $$dirName/interphore.apk;
	
	$(MAKE) shipDir SHIP_DIR="$(PARAPHORE_COM_PATH)/play/dev/"

shipInterEarly:
	$(MAKE) resetSite
	
	cp res/currentMod.phore bin
	$(MAKE) boptflash EXTRA_DEFINES+="-D SEMI_DEV" SHIPPING=1
	$(MAKE) packWindows EXTRA_DEFINES+="-D SEMI_DEV" SHIPPING=1
	$(MAKE) bandroid EXTRA_DEFINES+="-D SEMI_DEV" SHIPPING=1
	dirName=`ls -d $(PARAPHORE_COM_PATH)/play/early/*`; \
									cp $(PARAPHORE_COM_PATH)/play/interphore.html $$dirName/index.html; \
									cp bin/engine.swf $$dirName/interphore.swf; \
									cp bin/$(GAME_NAME).zip $$dirName/interphore.zip; \
									cp bin/engine.apk $$dirName/interphore.apk;
	
	$(MAKE) shipDir SHIP_DIR="$(PARAPHORE_COM_PATH)/play/early/"

shipInterPublic:
	$(MAKE) resetSite
	
	cp res/currentMod.phore bin
	$(MAKE) boptflash SHIPPING=1
	$(MAKE) packWindows SHIPPING=1
	$(MAKE) bandroid SHIPPING=1
	dirName=`ls -d `; \
									cp bin/engine.swf $(PARAPHORE_COM_PATH)/play/interphore.swf; \
									cp bin/$(GAME_NAME).zip $(PARAPHORE_COM_PATH)/play/interphore.zip; \
									cp bin/engine.apk $(PARAPHORE_COM_PATH)/play/interphore.apk;
	
	$(MAKE) shipDir SHIP_DIR="$(PARAPHORE_COM_PATH)/play"

shipDir:
	cd $(SHIP_DIR); \
		git add .; \
		git commit -m "inter up"; \
		git push; \
		newPrefix=`pwd | grep -o "paraphore.com.*"`; \
		newPrefix=$${newPrefix:14}; \
		s3cmd sync --delete-removed --acl-public --exclude '.git/*' . s3://paraphore.com/$$newPrefix/;

shipAll:
	cd $(PARAPHORE_COM_PATH); \
		git add .; \
		git commit -m "all up"; \
		git push; \
		s3cmd sync --delete-removed --acl-public --exclude '.git/*' . s3://paraphore.com/

shipCurrent:
	$(MAKE) shipDir SHIP_DIR="$(PARAPHORE_COM_PATH)/play"

include $(CPP_TOOLS)/engine/buildSystem/Makefile.common

testMod:
	cd bin/workingMod; \
	zip currentMod.phore *; \
	mv currentMod.phore ../
	
	make fast
