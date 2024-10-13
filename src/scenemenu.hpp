#pragma once
#include "global.hpp"
using namespace std;

struct SceneMenu : Scene {
	const vector<string> items = {
		"back (b)",
		"reset",
		"quit",
	};
	const int LINE_SPACING = 12;
	GFX gfx;
	int menuimage = 0, bgsprite = 0, handsprite = 0;
	int handpos = 0;

	void init() {
		if (menuimage < 1) {
			menuimage = gfx.makeimage(12 * gfx.FONT_W, 5 * gfx.FONT_H);
			auto& mimg = gfx.getimage(menuimage);
			// background menu
			// gfx.outline(mimg, 0xffffffff);
			for (int i = 0; i < (int)items.size(); i++)
				gfx.print(mimg, items[i], 0, i * LINE_SPACING);
		}
		// menu sprite
		auto& mimg = gfx.getimage(menuimage);
		bgsprite = gfx.makesprite(mimg.w, mimg.h, menuimage);
		auto& bgspr = gfx.getsprite(bgsprite);
		bgspr.pos.y = (gfx.screen.h - bgspr.pos.h) / 2;
		bgspr.pos.x = (gfx.screen.w - bgspr.pos.w + 30) / 2;
		// hand sprite
		handsprite = gfx.makesprite(8, 8, pimage);
		auto& hspr = gfx.getsprite(handsprite);
		hspr.pos.y = bgspr.pos.y;
		hspr.pos.x = bgspr.pos.x - 14;
	}

	void update() {
		// move hand
		if      (dpad.u == DPad::KDOWN)  handpos = max(0, handpos - 1);
		else if (dpad.d == DPad::KDOWN)  handpos = min(int(items.size() - 1), handpos + 1);
		else if (dpad.b == DPad::KDOWN)  {
			switchscene(SCENE_GAME);
			return;
		}

		// update hand position
		auto& hspr = gfx.getsprite(handsprite);
		auto& bgspr = gfx.getsprite(bgsprite);
		hspr.pos.y = bgspr.pos.y + LINE_SPACING * handpos;
	}

	void paint() {
		gfx.drawscene();
	}
};