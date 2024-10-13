#pragma once
#include "global.hpp"
using namespace std;


struct SceneTitle : Scene {
	const string title = "Sokoban";
	GFX gfx;
	int titleimage = 0, titlesprite = 0, playersprite = 0;
	int dir = 2, delta = 0;

	void init() {
		if (titleimage < 1) {
			titleimage = gfx.makeimage(title.size() * gfx.FONT_W, gfx.FONT_H);
			auto& img = gfx.getimage(titleimage);
			gfx.print(img, title, 0, 0);
		}
		// words
		titlesprite = gfx.makesprite(title.size() * gfx.FONT_W, gfx.FONT_H, titleimage);
		auto& tspr = gfx.getsprite(titlesprite);
		tspr.pos.x = (gfx.screen.w - tspr.pos.w) / 2;
		tspr.pos.y = (gfx.screen.h / 2) - 20;
		// animated sprite
		playersprite = gfx.makesprite(TSIZE, TSIZE, pimage);
		auto& spr = gfx.getsprite(playersprite);
		spr.pos.x = (gfx.screen.w - spr.pos.w) / 2;
		spr.pos.y = (gfx.screen.h / 2) + 2;
		// boxes
		int box1sprite = gfx.makesprite(TSIZE, TSIZE, tsetimage);
		auto& box1 = gfx.getsprite(box1sprite);
		box1.src.x = TSIZE * 4;
		box1.pos.y = (gfx.screen.h / 2) + 2;
		box1.pos.x = (gfx.screen.w - 45 - TSIZE*2) / 2;
		int box2sprite = gfx.makesprite(TSIZE, TSIZE, tsetimage);
		auto& box2 = gfx.getsprite(box2sprite);
		box2.src = box1.src;
		box2.pos.y = box1.pos.y;
		box2.pos.x = (gfx.screen.w + 45) / 2;
	}

	void update() {
		// animate spinning guy
		delta++;
		if (delta > 20) {
			dir = (dir + 1) % 4;
			delta = 0;
		}
		auto& spr = gfx.getsprite(playersprite);
		spr.src.x = dir * TSIZE;

		// start game on button
		if (dpad.a == DPad::KDOWN) {
			switchscene(SCENE_GAME);
			// game.level2map(0);
		}
	}

	void paint() {
		gfx.drawscene();
	}
};