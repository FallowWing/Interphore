#define DESKTOP_ICONS_MAX 16
#define DESKTOP_ICONS_MAX_COLS 5
#define DESKTOP_PROGRAMS_MAX 4

namespace WriterDesktop {
	bool exists = false;

	void js_addDesktopIcon(CScriptVar *v, void *userdata);
	void js_installDesktopExtentions(CScriptVar *v, void *userdata);
	void js_createDesktop(CScriptVar *v, void *userdata);
	void updateDesktop();
	void startProgram(const char *programName);

	struct DesktopProgram {
		bool exists;
		int index;
		MintSprite *bg;
		MintSprite *titleBar;
		char programName[MED_STR];
	};

	struct DesktopIcon {
		bool exists;
		int index;
		MintSprite *sprite;
		MintSprite *tf;
		char programName[MED_STR];
	};

	struct DesktopStruct {
		MintSprite *bg;
		DesktopIcon icons[DESKTOP_ICONS_MAX];
		int iconsNum;

		DesktopProgram programs[DESKTOP_PROGRAMS_MAX];
		int programsNum;

		DesktopProgram *draggedProgram;
	};

	DesktopStruct *desktop;
	Writer::WriterStruct *writer;

	void js_installDesktopExtentions(CScriptVar *v, void *userdata) {

		desktop = (DesktopStruct *)zalloc(sizeof(DesktopStruct));
		writer = Writer::writer;
		Writer::jsInterp->addNative("function addDesktopIcon(iconText, iconImg, programName)", &js_addDesktopIcon, 0);
		Writer::jsInterp->addNative("function createDesktop()", &js_createDesktop, 0);
		Writer::clear();
	}

	void js_createDesktop(CScriptVar *v, void *userdata) {
		exists = true;

		{ /// Desktop bg
			MintSprite *spr = createMintSprite();
			spr->setupRect(writer->bg->width*0.95, writer->bg->height*0.95, 0x222222);
			writer->bg->addChild(spr);
			spr->gravitate(0.5, 0.5);

			desktop->bg = spr;
		}
	}

	void js_addDesktopIcon(CScriptVar *v, void *userdata) {
		const char *iconText = v->getParameter("iconText")->getString().c_str();
		const char *iconImg = v->getParameter("iconImg")->getString().c_str();
		const char *programName = v->getParameter("programName")->getString().c_str();

		DesktopIcon *icon = NULL;

		ForEach (DesktopIcon *curIcon, desktop->icons) {
			if (!curIcon->exists) {
				icon = curIcon;
				break;
			}
		}

		if (!icon) {
			Writer::msg("Failed to create icon", Writer::MSG_ERROR);
			return;
		}

		icon->exists = true;
		icon->index = desktop->iconsNum++;
		strcpy(icon->programName, programName);

		{ /// Icon image
			MintSprite *spr = createMintSprite();
			if (streq(iconImg, "none")) {
				spr->setupRect(64, 64, 0xFF0000);
			} else {
				spr->setupAnimated(iconImg);
			}
			desktop->bg->addChild(spr);

			icon->sprite = spr;
		}

		{ /// Position icon
			int xIndex = icon->index % DESKTOP_ICONS_MAX_COLS;
			int yIndex = icon->index / DESKTOP_ICONS_MAX_COLS;
			int desktopEdgePad = 40;

			icon->sprite->x = xIndex * (icon->sprite->width * 3) + desktopEdgePad;
			icon->sprite->y = yIndex * (icon->sprite->height * 3) + desktopEdgePad;
		}

		{ /// Icon text
			MintSprite *spr = createMintSprite();
			spr->setupEmpty(128, 64);
			icon->sprite->addChild(spr);
			spr->setText(iconText);
			spr->gravitate(0.5, 0);
			spr->y = spr->parent->height + 5;

			icon->tf = spr;
		}
	}

	void updateDesktop() {
		bool canClickIcons = true;
		bool canClickPrograms = true;

		/// Figure out if dragging needs to happen
		ForEach (DesktopProgram *program, desktop->programs) {
			if (!program->exists) continue;

			if (canClickPrograms && program->titleBar->justPressed) {
				canClickIcons = false;
				canClickPrograms = false;
				desktop->draggedProgram = program;
			}
		}
		if (desktop->draggedProgram && !desktop->draggedProgram->titleBar->holding) {
			desktop->draggedProgram->bg->alpha = 1;
			desktop->draggedProgram = NULL;
		}

		/// Do actual dragging
		if (desktop->draggedProgram) {
			DesktopProgram *prog = desktop->draggedProgram;
			prog->bg->alpha = 0.5;

			canClickIcons = false;
			canClickPrograms = false;
		}

		/// Update the programs
		ForEach (DesktopProgram *program, desktop->programs) {
			if (!program->exists) continue;

			if (canClickPrograms && program->bg->hovering) {
				canClickIcons = false;
			}
		}

		/// Update the icons
		ForEach (DesktopIcon *icon, desktop->icons) {
			if (!icon->exists) continue;

			if (canClickIcons && icon->sprite->justReleased) {
				startProgram(icon->programName);
			}
		}
	}

	void startProgram(const char *programName) {
		DesktopProgram *program = NULL;

		ForEach (DesktopProgram *curProgram, desktop->programs) {
			if (!curProgram->exists) {
				program = curProgram;
				break;
			}
		}

		if (!program) {
			Writer::msg("Failed to create program", Writer::MSG_ERROR);
			return;
		}

		program->exists = true;
		program->index = desktop->programsNum++;
		strcpy(program->programName, programName);

		{ /// Program background
			MintSprite *spr = createMintSprite();
			spr->setupRect(512, 512, 0x333333);
			desktop->bg->addChild(spr);
			spr->gravitate(0.25, 0.25);
			spr->x += program->index * 20;
			spr->y += program->index * 20;

			program->bg = spr;
		}

		{ /// Tile bar
			MintSprite *spr = createMintSprite();
			spr->setupRect(program->bg->width, 20, 0x777777);
			program->bg->addChild(spr);

			program->titleBar = spr;
		}
	}
}