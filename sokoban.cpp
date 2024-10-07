#include <iostream>
#include "sdl.hpp"
#include "gfx.hpp"
#include "dpad.hpp"
#include "levels.hpp"
using namespace std;

SDLmanager sdl;
GFX gfx;
DPad dpad;
const int TSIZE = 16;
int tmap = 0, tsetimage = 0, pimage = 0;

struct Actor {
	int tx, ty;
	int sprite;
};
Actor player;
vector<Actor> boxes;

void makebox(int tx, int ty) {
	boxes.push_back({ tx, ty, gfx.makesprite(TSIZE, TSIZE, tsetimage) });
	auto& spr = gfx.getsprite(boxes.back().sprite);
	spr.src.x = 4 * TSIZE;
	spr.pos.x = tx * TSIZE;
	spr.pos.y = ty * TSIZE;
}

void level2map(const vector<string>& level) {
	for (auto& box : boxes)
		gfx.freesprite(box.sprite);
	boxes = {};

	int w = 0, h = level.size();
	for (auto s : level)
		if ((int)s.length() > w) w = s.length();
	auto& map = gfx.getmap(tmap);
	map.tw = w;
	map.th = h;
	map.data.resize(w * h, 0);

	for (int y = 0; y < map.th; y++) {
		for (int x = 0; x < map.tw; x++) {
			char c = x < (int)level[y].size() ? level[y][x] : '_';
			int t = 0;
			if      (c == ' ')  t = 1;
			else if (c == '_')  t = 0;
			else if (c == '#')  t = -3;
			else if (c == '.')  t = 2;
			else if (c == '$')  t = 1,  makebox(x, y);
			else if (c == '@')  t = 1,  player.tx = x, player.ty = y;
			map.data[ y * w + x ] = t;
			cout << t << ' ';
		}
		cout << endl;
	}
}

void update() {
	auto& spr = gfx.getsprite(player.sprite);
	if (dpad.u == DPad::KDOWN) {
		if (!gfx.collide_all(spr, 0, -TSIZE))
			player.ty--;
	}
	if (dpad.d == DPad::KDOWN) {
		if (!gfx.collide_all(spr, 0, TSIZE))
			player.ty++;
	}
	if (dpad.l == DPad::KDOWN) {
		if (!gfx.collide_all(spr, -TSIZE, 0))
			player.tx--;
	}
	if (dpad.r == DPad::KDOWN) {
		if (!gfx.collide_all(spr, TSIZE, 0))
			player.tx++;
	}
}

void repaint() {
	// update player sprite
	auto& spr = gfx.getsprite(player.sprite);
	spr.pos.x = TSIZE * player.tx;
	spr.pos.y = TSIZE * player.ty;

	// update boxes

	gfx.fill(0xff000000);
	gfx.drawscene();
	gfx.print(sdl.fps, 144, 1);
	sdl.flip(gfx.screen, dpad);
}

int main(int argc, char* args[]) {
	sdl.init();
	gfx.init(160, 160);
	gfx.flag_hit = 1;

	tsetimage = sdl.makebmp(gfx, "tiles.bmp");
	tmap = gfx.makemap(5, 5, TSIZE, tsetimage);
	pimage = sdl.makebmp(gfx, "player.bmp");
	player.sprite = gfx.makesprite(TSIZE, TSIZE, pimage);
	gfx.getsprite(player.sprite).src.x = TSIZE * 2;

	// load map 1
	level2map(level1);

	while (!sdl.quit) {
		update();
		repaint();
	}

	sdl.close();
	return 0;
}