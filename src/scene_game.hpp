#pragma once
#include "global.hpp"
#include "levels_skinner.hpp"
#include "levels_minicosmos.hpp"
#include "scene_wipe.hpp"
using namespace std;

struct SceneGame : Scene {
	GFX::Scene gfx;
	SceneWipe wipe;
	int tmap = 0, playerspr = 0;
	int levelno = 0;
	vector<int> boxes;
	vector<vector<GFX::Rect>> boardstack;

	// animations
	struct { int dx, dy, box, delta; } walkanim = {0};

	void init() {
		// make sprites and maps
		tmap = gfx.makemap( 5, 5, TSIZE, tsetimage );
		playerspr = gfx.makesprite( TSIZE, TSIZE, pimage );

		wipe.init();
	}

	void makebox(int tx, int ty) {
		boxes.push_back( gfx.makesprite(TSIZE, TSIZE, tsetimage) );
		auto& spr = gfx.getsprite( boxes.back() );
		spr.src.x = 4 * TSIZE;
		spr.pos.x = tx * TSIZE;
		spr.pos.y = ty * TSIZE;
	}

	void reset() {
		level2map(levelno);
	}

	void level2map(int _levelno) {
		// auto& level = LEVELS_SKINNER.at( _levelno );
		auto& level = LEVELS_MINICOSMOS.at( _levelno );
		levelno = _levelno;

		for (auto& box : boxes)
			gfx.freesprite(box);
		boxes = {};
		boardstack = {};

		int w = 0, h = level.size();
		for (auto s : level)
			if ((int)s.length() > w) w = s.length();
		auto& map = gfx.getmap( tmap );
		map.tw = w;
		map.th = h;
		map.data.resize(w * h, 0);

		auto& pspr = gfx.getsprite( playerspr );
		pspr.src.x = TSIZE * 2;

		for (int y = 0; y < map.th; y++) {
			for (int x = 0; x < map.tw; x++) {
				char c = x < (int)level[y].size() ? level[y][x] : '_';
				int t = 0;
				if      (c == ' ')  t = 1;
				else if (c == '_')  t = 0;
				else if (c == '#')  t = -3;
				else if (c == '.')  t = 2;
				else if (c == '$')  t = 1,  makebox(x, y);
				else if (c == '*')  t = 2,  makebox(x, y);
				else if (c == '@')  t = 1,  pspr.pos.x = x * TSIZE, pspr.pos.y = y * TSIZE;
				map.data[ y * w + x ] = t;
				// cout << t << ' ';
			}
			// cout << endl;
		}

		pushstate();

		printf("maps: %d, sprites: %d\n", (int)gfx.tilemaps.size(), (int)gfx.sprites.size());
	}

	void pushstate() {
		boardstack.push_back({});
		auto& state = boardstack.back();
		// add player state
		auto& pspr = gfx.getsprite( playerspr );
		state.push_back({ pspr.pos.x, pspr.pos.y, 1 });
		// add box state
		for (int b : boxes) {
			auto& box = gfx.getsprite( b );
			state.push_back({ box.pos.x, box.pos.y });
		}
	}

	void popstate() {
		if (boardstack.size() > 1)
			boardstack.pop_back();
		auto& state = boardstack.back();
		// set player state
		auto& pspr = gfx.getsprite( playerspr );
		pspr.pos.x = state.at(0).x;
		pspr.pos.y = state.at(0).y;
		// set box state
		for (int i = 1; i < (int)state.size(); i++) {
			auto& box = gfx.getsprite( boxes.at(i - 1) );
			box.pos.x = state[i].x;
			box.pos.y = state[i].y;
		}
	}

	void paint() {
		gfx.drawscene();
		wipe.paint();
	}

	void update() {
		if (walkanim.dx || walkanim.dy)
			animatemove();
		else if (wipe.active())
			animatelevelwipe();
		else if (checkwin())
			;
		else
			playerinput();
	}

	void playerinput() {
		// get player input
		int x = 0, y = 0;
		if      (dpad.u == DPad::KPRESSED)  y = -1;
		else if (dpad.d == DPad::KPRESSED)  y =  1;
		else if (dpad.l == DPad::KPRESSED)  x = -1;
		else if (dpad.r == DPad::KPRESSED)  x =  1;
		else if (dpad.b == DPad::KDOWN)     popstate();
		else if (dpad.a == DPad::KDOWN)     { switchscene(SCENE_MENU);  return; }

		// move player
		int tx = x * TSIZE, ty = y * TSIZE;
		auto& pspr = gfx.getsprite(playerspr);
		if (( x || y ) && !gfx.collide_map( pspr, tx, ty )) {
			if (gfx.collide_sprite( pspr, tx, ty )) {
				int boxid = gfx.collisions_sprite.at(0);
				auto& box = gfx.getsprite( boxid );
				if (!gfx.collide_all( box, tx, ty )) {
					walkanim = { x, y, boxid };
					animatemove();
				}
			}
			else {
				walkanim = { x, y };
				animatemove();
			}
		}
	}

	int checkwin() {
		// calculate boxes in place
		int onpoint = 0;
		for (auto b : boxes) {
			auto& box = gfx.getsprite( b );
			if (gfx.mapatpx( gfx.getmap(tmap), box.pos.x, box.pos.y ) == 2)
				onpoint++;
		}
		// check win
		if (onpoint == (int)boxes.size()) {
			printf("win\n");
			// level2map(levelno + 1);
			wipe.start();
			return 1;
		}
		return 0;
	}

	void animatemove() {
		static const int WALKSPEED = 1;
		// move player
		auto& spr = gfx.getsprite( playerspr );
		spr.pos.x += walkanim.dx * WALKSPEED;
		spr.pos.y += walkanim.dy * WALKSPEED;
		// set animation
		int dir = 0;
		if      (walkanim.dy == -1)  dir = 0;
		else if (walkanim.dx ==  1)  dir = 1;
		else if (walkanim.dy ==  1)  dir = 2;
		else if (walkanim.dx == -1)  dir = 3;
		if (walkanim.delta < TSIZE / 2)  dir += 4;
		spr.src.x = dir * TSIZE;
		// move pushed box
		if (walkanim.box > 0) {
			auto& box = gfx.getsprite( walkanim.box );
			box.pos.x += walkanim.dx * WALKSPEED;
			box.pos.y += walkanim.dy * WALKSPEED;
		}
		// next
		walkanim.delta += WALKSPEED;
		if (walkanim.delta >= TSIZE) {
			pushstate();
			walkanim = {0};
		}
	}

	void animatelevelwipe() {
		wipe.update();
		if (wipe.midpoint())
			level2map(levelno + 1);
	}
};