#pragma once
#include "scenelib/sdl.hpp"
#include "scenelib/gfx.hpp"
#include "scenelib/dpad.hpp"


struct Scene {
	enum GAMESCENE {
		SCENE_TITLE,
		SCENE_GAME,
		SCENE_MENU,
	};

	static const  int TSIZE = 16;
	static inline int tsetimage = 0, pimage = 0;
	static inline GAMESCENE gamescene = SCENE_TITLE;
	static inline DPad dpad;

	static void switchscene(GAMESCENE scene) {
		gamescene = scene;
	}
	
	virtual void init() {}
	virtual void update() {}
	virtual void paint() {}
};