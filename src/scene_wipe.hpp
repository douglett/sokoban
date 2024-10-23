#pragma once
#include "global.hpp"
using namespace std;


struct SceneWipe : Scene {
	GFX::Scene gfx;
	int overlayspr = 0;
	int dir = 0, speed = 5;

	void init() {
		// overlay box sprite
		overlayspr = gfx.makespriteimage( gfx.screen.w, gfx.screen.h );
		auto& ospr = gfx.getsprite( overlayspr );
		gfx.fill( gfx.getimage(ospr.image), 0xff000000 );
		// gfx.fill( gfx.getimage(ospr.image), 0xff440000 );
		// ospr.hit = ospr.hurt = { 0 };
		// ospr.z = 1000;
		ospr.visible = false;
	}

	bool active() {
		return dir != 0;
	}
	bool midpoint() {
		auto& ospr = gfx.getsprite( overlayspr );
		return active() && ospr.pos.x == 0;
	}

	void start() {
		dir = 1;
		auto& ospr = gfx.getsprite( overlayspr );
		ospr.pos.x = -ospr.pos.w;
	}

	void update() {
		auto& ospr = gfx.getsprite( overlayspr );
		// animate in left
		if (dir == 1) {
			ospr.visible = true;
			ospr.pos.x += speed;
			if (ospr.pos.x >= 0) {
				ospr.pos.x = 0;
				dir = -1;
			}
		}
		// animate out right
		else if (dir == -1) {
			ospr.visible = true;
			ospr.pos.x += speed;
			if (ospr.pos.x >= ospr.pos.w) {
				dir = 0;
				ospr.visible = false;
			}
		}
		// animation end
		else {
			dir = 0;
			ospr.visible = false;
		}
	}

	void paint() {
		gfx.drawscene();
	}
};