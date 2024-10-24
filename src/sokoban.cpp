#include <iostream>
#include "global.hpp"
#include "scene_title.hpp"
#include "scene_game.hpp"
#include "scene_menu.hpp"
#include "scene_wipe.hpp"
using namespace std;

SDLmanager sdl( "Sokoban", 534, 480 );
GFX gfx;

SceneTitle  title;
SceneGame   game;
SceneMenu   menu;
SceneWipe   wipe;


// switch between scenes
void Scene::switchscene(GAMESCENE scene, int levelno) {
	// title -> ingame : load level 1
	if (currentscene == SCENE_TITLE && scene == SCENE_GAME) {
		game.level2map( levelno );
	}
	// switch scenes
	if (currentscene == scene || scene == SCENE_MENU || (currentscene == SCENE_MENU && scene == SCENE_GAME))
		currentscene = scene;
	else {
		toscene = scene;
		wipe.start();
	}
}

// get currently active scene
Scene& getscene() {
	switch (Scene::currentscene) {
		case Scene::SCENE_GAME:     return game;
		case Scene::SCENE_MENU:     return menu;
		default:
		case Scene::SCENE_TITLE:    return title;
	}
}

void update() {
	if (wipe.active()) {
		wipe.update();
		if (wipe.midpoint())
			Scene::currentscene = Scene::toscene;
	}
	else
		getscene().update();
}

void paint() {
	gfx.fill(0xff000000);
	getscene().paint();
	wipe.paint();
	gfx.print(sdl.fps, 144, 1);
	sdl.flip(gfx.screen, Scene::dpad);
}


int main(int argc, char* args[]) {
	sdl.init();
	gfx.init(160, 160);
	// gfx.flag_hit = 1;

	Scene::tsetimage = sdl.makebmp("assets/tiles.bmp");
	Scene::pimage = sdl.makebmp("assets/player.bmp");

	title.init();
	game.init();
	menu.init();
	wipe.init();

	Scene::switchscene(Scene::SCENE_TITLE);

	while (!sdl.quit) {
		update();
		paint();
	}

	sdl.close();
	return 0;
}