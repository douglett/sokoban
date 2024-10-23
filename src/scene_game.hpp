#pragma once
#include "global.hpp"
#include "levels_skinner.hpp"
// #include "levels_minicosmos.hpp"
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
	struct { int dir, dx, dy, box, delta; } walkanim = {0};

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
		levelno = _levelno;
		if (levelno >= (int)LEVELS_SKINNER.size())  levelno = 0;  // loop-to-start
		// auto& level = LEVELS_MINICOSMOS.at( levelno );
		auto& level = LEVELS_SKINNER.at( levelno );

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
		centeronplayer();

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
		// center camera
		centeronplayer();
	}

	void playerinput() {
		// get player input
		int dir = -1, x = 0, y = 0;
		if      (dpad.u == DPad::KPRESSED)  dir = 0, y = -1;
		else if (dpad.r == DPad::KPRESSED)  dir = 1, x =  1;
		else if (dpad.d == DPad::KPRESSED)  dir = 2, y =  1;
		else if (dpad.l == DPad::KPRESSED)  dir = 3, x = -1;
		else if (dpad.b == DPad::KDOWN)     popstate();
		else if (dpad.a == DPad::KDOWN)     { switchscene(SCENE_MENU);  return; }

		// move player
		int tx = x * TSIZE, ty = y * TSIZE;
		auto& pspr = gfx.getsprite(playerspr);
		// walk into box
		if (dir > -1 && gfx.collide_sprite( pspr, tx, ty )) {
			int boxid = gfx.collisions_sprite.at(0);
			auto& box = gfx.getsprite( boxid );
			if (!gfx.collide_all( box, tx, ty )) {
				walkanim = { dir, x, y, boxid };
				animatemove();
			}
			else {
				pspr.src.x = dir * TSIZE;
			}
		}
		// walk into space
		else if (dir > -1 && !gfx.collide_map( pspr, tx, ty )) {
			walkanim = { dir, x, y };
			animatemove();
		}
		// neither
		else if (dir > -1) {
			pspr.src.x = dir * TSIZE;
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
		spr.src.x = ( walkanim.dir + ( walkanim.delta < TSIZE / 2 ? 4 : 0 ) ) * TSIZE;
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

	void centeronplayer() {
		auto& map = gfx.getmap( tmap );
		auto& pspr = gfx.getsprite( playerspr );
		const int MARGIN = 4;

		// center x
		if (map.tw * TSIZE < gfx.screen.w)
			gfx.sceneoffset.x = (gfx.screen.w - map.tw * TSIZE) / 2;
		else {
			gfx.sceneoffset.x = ((gfx.screen.w - pspr.pos.w) / 2) - pspr.pos.x;
			gfx.sceneoffset.x = min( gfx.sceneoffset.x, MARGIN );
			gfx.sceneoffset.x = max( gfx.sceneoffset.x, -(map.tw * TSIZE - gfx.screen.w + MARGIN) );
		}

		// center y
		if (map.th * TSIZE < gfx.screen.h)
			gfx.sceneoffset.y = (gfx.screen.h - map.th * TSIZE) / 2;
		else {
			gfx.sceneoffset.y = ((gfx.screen.h - pspr.pos.h) / 2) - pspr.pos.y;
			gfx.sceneoffset.y = min( gfx.sceneoffset.y, MARGIN );
			gfx.sceneoffset.y = max( gfx.sceneoffset.y, -(map.th * TSIZE - gfx.screen.h + MARGIN) );
		}
	}
};