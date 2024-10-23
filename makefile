OUT  = sokoban.exe
SRC  = src/sokoban.cpp
HEAD = scenelib/sdl.hpp scenelib/gfx.hpp scenelib/dpad.hpp \
		src/global.hpp src/scene_title.hpp src/scene_game.hpp src/scene_menu.hpp src/scene_wipe.hpp \
		src/levels_skinner.hpp src/levels_minicosmos.hpp


$(OUT): $(SRC) $(HEAD)
	g++ -std=c++17 -Wall -gdwarf -o $(OUT) $(SRC) \
		-IC:/bin/devlib/SDL2/include/SDL2 -LC:/bin/devlib/SDL2/lib \
		-I. \
		-lmingw32 -lSDL2main -lSDL2 -mwindows -mconsole

release: $(SRC) $(HEAD)
	g++ -std=c++17 -Wall -Ofast -o $(OUT) $(SRC) \
		-IC:/bin/devlib/SDL2/include/SDL2 -LC:/bin/devlib/SDL2/lib \
		-I. \
		-lmingw32 -lSDL2main -lSDL2 -mwindows

run: $(OUT)
	./$(OUT)