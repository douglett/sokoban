#include <iostream>
#include "sdl.hpp"
#include "gfx.hpp"
#include "dpad.hpp"
#include "levels.hpp"
using namespace std;

enum GAMESTATE {
	STATE_TITLE,
	STATE_GAME
};
SDLmanager sdl;
GFX gfx;
DPad dpad;
const int TSIZE = 16;
int tsetimage = 0, pimage = 0;
GAMESTATE gamestate = STATE_TITLE;


void switchstate(GAMESTATE state);
void update();
void repaint();


struct Game {
	int tmap = 0, playersprite = 0;
	int levelno = 0;
	vector<int> boxes;

	void init() {
		// make sprites and maps
		tmap = gfx.makemap(5, 5, TSIZE, tsetimage);
		playersprite = gfx.makesprite(TSIZE, TSIZE, pimage);
		gfx.getsprite(playersprite).src.x = TSIZE * 2;
		// load map 1
		level2map(0);
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
} game;


struct TitleScreen {
	const string title = "Sokoban";
	int titleimage = 0, titlesprite = 0, playersprite = 0;
	int dir = 2, delta = 0;

	void init() {
		if (!titleimage) {
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

	// void paint() {
	// 	gfx.fill(0xff000000);
	// 	gfx.print( title, (gfx.screen.w - tspr.pos.w) / 2, (gfx.screen.h / 2) - 20 );
	// 	// boxes
	// 	Rect src = { TSIZE * 4, 0, TSIZE, TSIZE };
	// 	gfx.blit( tsetimage, (gfx.screen.w - 45 - TSIZE*2) / 2, (gfx.screen.h / 2) + 2, src );
	// 	gfx.blit( tsetimage, (gfx.screen.w + 45) / 2, (gfx.screen.h / 2) + 2, src );
	// }

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
		if (dpad.a == DPad::KDOWN)  switchstate(STATE_GAME);
	}
} titlescreen;


void switchstate(GAMESTATE state) {
	gfx.tilemaps = {};
	gfx.sprites = {};
	gamestate = state;
	switch (gamestate) {
		case STATE_TITLE:  titlescreen.init();  break;
		case STATE_GAME:   game.init();  break;
	}
}

void update() {
	switch (gamestate) {
		case STATE_TITLE:  titlescreen.update();  break;
		case STATE_GAME:   game.update();  break;
	}
}

void repaint() {
	// repaint game scene
	gfx.fill(0xff000000);
	gfx.drawscene();
	gfx.print(sdl.fps, 144, 1);
	sdl.flip(gfx.screen, dpad);
}

int main(int argc, char* args[]) {
	sdl.init();
	gfx.init(160, 160);
	// gfx.flag_hit = 1;

	tsetimage = sdl.makebmp(gfx, "tiles.bmp");
	pimage = sdl.makebmp(gfx, "player.bmp");
	// game.init();
	// titlescreen.init();
	switchstate(STATE_TITLE);

	while (!sdl.quit) {
		// titlescreen.update();
		// game.update();
		update();
		repaint();
	}

	sdl.close();
	return 0;
}