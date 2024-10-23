#pragma once
#include "global.hpp"
#include "levels_skinner.hpp"
// #include "levels_minicosmos.hpp"
using namespace std;


struct SceneTitle : Scene {
	const string title = "Sokoban";
	const int TEXT_LINE_SPACING = 12, TEXT_OFFSET_X = 12, TEXT_OFFSET_Y = 4;

	GFX::Scene gfx;
	int titlesprite = 0, playersprite = 0, menusprite = 0, handsprite = 0;
	int dir = 2, delta = 0, handpos = 0;

	void init() {
		// words sprite
		titlesprite = gfx.makespriteimage( title.size() * gfx.FONT_W, gfx.FONT_H );
		auto& tspr = gfx.getsprite( titlesprite );
		gfx.print( gfx.getimage(tspr.image), title, 0, 0 );
		tspr.pos.x = (gfx.screen.w - tspr.pos.w) / 2;
		tspr.pos.y = (gfx.screen.h / 2) - 20;
		// animated sprite
		playersprite = gfx.makesprite( TSIZE, TSIZE, pimage );
		auto& spr = gfx.getsprite( playersprite );
		spr.pos.x = (gfx.screen.w - spr.pos.w) / 2;
		spr.pos.y = (gfx.screen.h / 2) + 2;
		// boxes
		int box1sprite = gfx.makesprite( TSIZE, TSIZE, tsetimage );
		auto& box1 = gfx.getsprite( box1sprite );
		box1.src.x = TSIZE * 4;
		box1.pos.y = (gfx.screen.h / 2) + 2;
		box1.pos.x = (gfx.screen.w - 45 - TSIZE*2) / 2;
		int box2sprite = gfx.makesprite( TSIZE, TSIZE, tsetimage );
		auto& box2 = gfx.getsprite( box2sprite );
		box2.src = box1.src;
		box2.pos.y = box1.pos.y;
		box2.pos.x = (gfx.screen.w + 45) / 2;

		// menu
		menusprite = gfx.makespriteimage( 13 * gfx.FONT_W, LEVELS_SKINNER.size() * TEXT_LINE_SPACING + TEXT_OFFSET_Y * 2 );
		auto& mspr = gfx.getsprite( menusprite );
		mspr.visible = false;
		mspr.pos.x = 100;
		mspr.pos.y = 12;
		// draw sprite image
		auto& mimg = gfx.getimage( mspr.image );
		gfx.fill( mimg, 0xff0000ff );
		gfx.outline( mimg, 0xffffffff, { 1, 1, mimg.w - 2, mimg.h - 2 } );
		for (int i = 0; i < (int)LEVELS_SKINNER.size(); i++) {
			gfx.print( mimg, "Level " + to_string(i + 1), TEXT_OFFSET_X, TEXT_OFFSET_Y + i * TEXT_LINE_SPACING ); 
		}

		// hand sprite
		handsprite = gfx.makesprite( 8, 8, pimage );
		auto& hspr = gfx.getsprite( handsprite );
		hspr.visible = false;
		hspr.pos.x = mspr.pos.x;
		hspr.pos.y = mspr.pos.y + TEXT_OFFSET_Y + TEXT_LINE_SPACING * handpos;
	}

	void update() {
		// animate spinning guy
		delta++;
		if (delta > 20) {
			dir = (dir + 1) % 4;
			delta = 0;
		}
		auto& spr = gfx.getsprite( playersprite );
		spr.src.x = dir * TSIZE;

		auto& mspr = gfx.getsprite( menusprite );
		auto& hspr = gfx.getsprite( handsprite );

		// show menu
		if (!mspr.visible) {
			if (dpad.a == DPad::KDOWN) {
				mspr.visible = hspr.visible = true;
			}
		}
		// navigate menu
		else {
			if (dpad.u == DPad::KDOWN)
				handpos = max( handpos - 1, 0 );
			else if (dpad.d == DPad::KDOWN)
				handpos = min( handpos + 1, (int)LEVELS_SKINNER.size() - 1 );
			// start game on button
			else if (dpad.a == DPad::KDOWN) {
				mspr.visible = hspr.visible = false;
				switchscene( SCENE_GAME, handpos );
			}
			// update hand position
			hspr.pos.y = mspr.pos.y + TEXT_OFFSET_Y + TEXT_LINE_SPACING * handpos;
		}
	}

	void paint() {
		gfx.drawscene();
	}
};