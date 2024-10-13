#include <iostream>
#include "global.hpp"
#include "scenetitle.hpp"
#include "scenegame.hpp"
#include "scenemenu.hpp"
using namespace std;

SDLmanager sdl;
GFX gfx;

SceneTitle title;
SceneGame  game;
SceneMenu  menu;


Scene& getscene() {
	switch (Scene::gamescene) {
		case Scene::SCENE_TITLE:    return title;
		case Scene::SCENE_GAME:     return game;
		case Scene::SCENE_MENU:     return menu;
	}
	return title;
}

void update() {
	getscene().update();
}

void paint() {
	gfx.fill(0xff000000);
	getscene().paint();
	gfx.print(sdl.fps, 144, 1);
	sdl.flip(gfx.screen, Scene::dpad);
}


int main(int argc, char* args[]) {
	sdl.init();
	gfx.init(160, 160);
	// gfx.flag_hit = 1;

	Scene::tsetimage = sdl.makebmp("assets/tiles.bmp");
	Scene::pimage = sdl.makebmp("assets/player.bmp");
	// switchstate(STATE_TITLE);
	// switchstate(STATE_MENU);

	title.init();
	game.init();
	menu.init();

	// Scene::switchscene(SCENE_TITLE);

	while (!sdl.quit) {
		update();
		paint();
	}

	sdl.close();
	return 0;
}