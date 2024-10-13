#pragma once
#include "global.hpp"
#include "levels.hpp"
using namespace std;

struct SceneGame : Scene {
	GFX gfx;
	int tmap = 0, playersprite = 0;
	int levelno = 0;
	vector<int> boxes;

	void init() {
		// make sprites and maps
		tmap = gfx.makemap(5, 5, TSIZE, tsetimage);
		playersprite = gfx.makesprite(TSIZE, TSIZE, pimage);
		gfx.getsprite(playersprite).src.x = TSIZE * 2;
		// reload map data
		level2map(levelno);
	}

	void makebox(int tx, int ty) {
		boxes.push_back( gfx.makesprite(TSIZE, TSIZE, tsetimage) );
		auto& spr = gfx.getsprite( boxes.back() );
		spr.src.x = 4 * TSIZE;
		spr.pos.x = tx * TSIZE;
		spr.pos.y = ty * TSIZE;
	}

	void level2map(int _levelno) {
		auto& level = MINICOSMOS.at(_levelno);
		levelno = _levelno;

		for (auto& box : boxes)
			gfx.freesprite(box);
		boxes = {};

		int w = 0, h = level.size();
		for (auto s : level)
			if ((int)s.length() > w) w = s.length();
		auto& map = gfx.getmap(tmap);
		map.tw = w;
		map.th = h;
		map.data.resize(w * h, 0);

		auto& pspr = gfx.getsprite(playersprite);

		for (int y = 0; y < map.th; y++) {
			for (int x = 0; x < map.tw; x++) {
				char c = x < (int)level[y].size() ? level[y][x] : '_';
				int t = 0;
				if      (c == ' ')  t = 1;
				else if (c == '_')  t = 0;
				else if (c == '#')  t = -3;
				else if (c == '.')  t = 2;
				else if (c == '$')  t = 1,  makebox(x, y);
				else if (c == '@')  t = 1,  pspr.pos.x = x * TSIZE, pspr.pos.y = y * TSIZE;
				map.data[ y * w + x ] = t;
				cout << t << ' ';
			}
			cout << endl;
		}

		printf("maps: %d, sprites: %d\n", (int)gfx.tilemaps.size(), (int)gfx.sprites.size());
	}

	void update() {
		// get player input
		int x = 0, y = 0;
		if      (dpad.u == DPad::KDOWN)  y = -1;
		else if (dpad.d == DPad::KDOWN)  y =  1;
		else if (dpad.l == DPad::KDOWN)  x = -1;
		else if (dpad.r == DPad::KDOWN)  x =  1;
		else if (dpad.b == DPad::KDOWN)  level2map(levelno);
		else if (dpad.a == DPad::KDOWN)  {
			switchscene(SCENE_MENU);
			return;
		}

		// move player
		int tx = x * TSIZE, ty = y * TSIZE;
		auto& pspr = gfx.getsprite(playersprite);
		if ((x || y) && !gfx.collide_map(pspr, tx, ty)) {
			if (gfx.collide_sprite(pspr, tx, ty)) {
				auto& box = gfx.getsprite( gfx.collisions_sprite.at(0) );
				if (!gfx.collide_all(box, tx, ty)) {
					box.pos.x += tx;
					box.pos.y += ty;
					pspr.pos.x += tx;
					pspr.pos.y += ty;
				}
			}
			else {
				pspr.pos.x += tx;
				pspr.pos.y += ty;
			}
		}

		// calculate win
		int onpoint = 0;
		for (auto b : boxes) {
			auto& box = gfx.getsprite(b);
			if (gfx.mapatpx( gfx.getmap(tmap), box.pos.x, box.pos.y ) == 2)
				onpoint++;
		}
		if (onpoint == (int)boxes.size()) {
			printf("win\n");
			level2map(levelno + 1);
		}
	}

	void paint() {
		gfx.drawscene();
	}
};