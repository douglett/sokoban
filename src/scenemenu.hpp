#pragma once
#include "global.hpp"
#include "scenegame.hpp"
using namespace std;

struct SceneMenu : Scene {
	const vector<string> items = {
		string("back (") + DPad::BUTTON_B + ")",
		"reset",
		"quit",
	};
	const int TEXT_LINE_SPACING = 12, TEXT_OFFSET_X = 12, TEXT_OFFSET_Y = 4;
	GFX gfx;
	int menuimage = 0, bgsprite = 0, handsprite = 0;
	int handpos = 0;

	void init() {
		// background menu image
		if (menuimage < 1) {
			menuimage = gfx.makeimage(12 * gfx.FONT_W, TEXT_LINE_SPACING * items.size() + TEXT_OFFSET_Y * 2);
			auto& mimg = gfx.getimage(menuimage);
			gfx.fill(mimg, 0xff0000ff);
			gfx.outline(mimg, 0xffffffff, { 1, 1, mimg.w - 2, mimg.h - 2 });
			for (int i = 0; i < (int)items.size(); i++)
				gfx.print(mimg, items[i], TEXT_OFFSET_X, TEXT_OFFSET_Y + i * TEXT_LINE_SPACING);
		}

		// menu sprite
		auto& mimg = gfx.getimage(menuimage);
		bgsprite = gfx.makesprite(mimg.w, mimg.h, menuimage);
		// auto& bgspr = gfx.getsprite(bgsprite);
		// hand sprite
		handsprite = gfx.makesprite(8, 8, pimage);
		auto& hspr = gfx.getsprite(handsprite);
		hspr.pos.y = TEXT_OFFSET_Y + TEXT_LINE_SPACING * handpos;

		// move the whole scene around the screen
		gfx.sceneoffset.x = gfx.sceneoffset.y = 10;
	}

	void update() {
		extern SceneGame game;

		// move hand
		if      (dpad.u == DPad::KDOWN)  handpos = max(0, handpos - 1);
		else if (dpad.d == DPad::KDOWN)  handpos = min(int(items.size() - 1), handpos + 1);
		else if (dpad.b == DPad::KDOWN)  {
			switchscene(SCENE_GAME);
			return;
		}
		else if (dpad.a == DPad::KDOWN)  {
			switch (handpos) {
				case 0:
					game.popstate();
					switchscene(SCENE_GAME);
					break;
				case 1:
					game.reset();
					switchscene(SCENE_GAME);
					break;
				case 2:
					switchscene(SCENE_TITLE);
					break;
			}
		}

		// update hand position
		auto& hspr = gfx.getsprite(handsprite);
		hspr.pos.y = TEXT_OFFSET_Y + TEXT_LINE_SPACING * handpos;
	}

	void paint() {
		extern SceneGame game;
		
		// paint menu scene on top of game scene
		game.paint();
		gfx.drawscene();
	}
};