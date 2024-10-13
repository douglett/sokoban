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


// paint menu scene on top of game scene
void SceneMenu::paint() {
	game.paint();
	gfx.drawscene();
}

// switch between scenes
void Scene::switchscene(GAMESCENE scene) {
	// title -> ingame : load level 1
	if (currentscene == SCENE_TITLE && scene == SCENE_GAME) {
		game.level2map(0);
	}
	// switch scenes
	currentscene = scene;
}

// get currently active scene
Scene& getscene() {
	switch (Scene::currentscene) {
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

	title.init();
	game.init();
	menu.init();

	Scene::switchscene(Scene::SCENE_TITLE);

	while (!sdl.quit) {
		update();
		paint();
	}

	sdl.close();
	return 0;
}