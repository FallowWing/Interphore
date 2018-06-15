ASSET_PATH := sourceAssets
EXTRA_DEFINES := 
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

include ../other/Makefile.secret
GAME_WIDTH=1280
GAME_HEIGHT=720
SCREEN_ORIENTATION := landscape

all:
	cp res/currentMod.phore bin
	$(MAKE) defaultAll

exportAssets:
	# bash $(CPP_TOOLS)/engine/buildSystem/exportAssets.sh ../writerExportedAssets/img assets/img

resetSite:
	cd $(PARAPHORE_COM_PATH) && \
		git fetch origin && \
		git reset --hard origin/master && \
		git pull

shipInter:
	$(MAKE) resetSite
	
	$(MAKE) boptflash EXTRA_DEFINES="-D SEMI_DEV"
	$(MAKE) packWindows EXTRA_DEFINES="-D SEMI_DEV"
	$(MAKE) bandroid EXTRA_DEFINES="-D SEMI_DEV"
	cp bin/engine.swf $(PARAPHORE_COM_PATH)/interphore/interphore.swf
	cp bin/$(GAME_NAME).zip $(PARAPHORE_COM_PATH)/interphore/interphore.zip
	cp bin/engine.apk $(PARAPHORE_COM_PATH)/interphore/interphore.apk
	
	$(MAKE) shipDir SHIP_DIR="$(PARAPHORE_COM_PATH)/interphore"

shipDir:
	cd $(SHIP_DIR); \
		git add .; \
		git commit -m "semi up"; \
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
	$(MAKE) shipDir SHIP_DIR="$(PARAPHORE_COM_PATH)/paraphore/dev"
	$(MAKE) shipDir SHIP_DIR="$(PARAPHORE_COM_PATH)/semiphore"

include $(CPP_TOOLS)/engine/buildSystem/Makefile.common
