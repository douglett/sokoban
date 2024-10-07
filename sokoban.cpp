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

struct {
	int sprite;
} player;
vector<int> boxes;

void makebox(int tx, int ty) {
	boxes.push_back( gfx.makesprite(TSIZE, TSIZE, tsetimage) );
	auto& spr = gfx.getsprite( boxes.back() );
	spr.src.x = 4 * TSIZE;
	spr.pos.x = tx * TSIZE;
	spr.pos.y = ty * TSIZE;
}

void level2map(const vector<string>& level) {
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

	auto& pspr = gfx.getsprite(player.sprite);

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
	else if (dpad.b == DPad::KDOWN)  level2map(MINICOSMOS[1]);

	// move player
	int tx = x * TSIZE, ty = y * TSIZE;
	auto& pspr = gfx.getsprite(player.sprite);
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
	if (onpoint == (int)boxes.size())
		printf("win\n");
}

void repaint() {
	// update player sprite
	// auto& spr = gfx.getsprite(player.sprite);
	// spr.pos.x = TSIZE * player.tx;
	// spr.pos.y = TSIZE * player.ty;

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
	level2map(MINICOSMOS[1]);

	while (!sdl.quit) {
		update();
		repaint();
	}

	sdl.close();
	return 0;
}