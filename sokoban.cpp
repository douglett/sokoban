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
int tmap = 0;

struct Actor {
	int x, y;
};
Actor player;
vector<Actor> boxes;

void level2map(const vector<string>& level) {
	boxes = {};

	int w = 0, h = level.size();
	for (auto s : level)
		if ((int)s.length() > w) w = s.length();
	auto& map = gfx.getmap(tmap);
	map.twidth = w;
	map.theight = h;
	map.data.resize(w * h, 0);

	for (int y = 0; y < map.theight; y++)
	for (int x = 0; x < map.twidth; x++) {
		char c = (int)level[y].size() < x ? level[y][x] : ' ';
		int t = 0;
		if      (c == ' ')  t = 0;
		else if (c == '#')  t = -1;
		else if (c == '.')  t = -2;
		else if (c == '$')  t = 0,  boxes.push_back({ x, y });
		else if (c == '@')  t = 0,  player.x = x, player.y = y;
		map.data[ y * w + x ] = t;
	}
}

void update() {

}

void repaint() {
	gfx.fill(0xff000000);
	gfx.drawscene();
	gfx.print(sdl.fps, 144, 1);
	sdl.flip(gfx.screen, dpad);
}

int main(int argc, char* args[]) {
	sdl.init();
	gfx.init(160, 160);

	tmap = gfx.makemap(5, 5, TSIZE, 0);

	while (!sdl.quit) {
		update();
		repaint();
	}

	sdl.close();
	return 0;
}