OUT  = sokoban.exe
SRC  = src/sokoban.cpp
HEAD = scenelib/sdl.hpp scenelib/gfx.hpp scenelib/dpad.hpp \
		src/global.hpp src/levels.hpp src/scenetitle.hpp src/scenegame.hpp src/scenemenu.hpp


$(OUT): $(SRC) $(HEAD)
	g++ -std=c++17 -Wall -gdwarf -o $(OUT) $(SRC) \
		-IC:/bin/devlib/SDL2/include/SDL2 -LC:/bin/devlib/SDL2/lib \
		-I. \
		-lmingw32 -lSDL2main -lSDL2 -mwindows -mconsole

run: $(OUT)
	./$(OUT)