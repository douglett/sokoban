OUT  = sokoban.exe
SRC  = sokoban.cpp
HEAD = ../scene/sdl.hpp ../scene/gfx.hpp ../scene/dpad.hpp


$(OUT): $(SRC) $(HEAD)
	g++ -std=c++17 -Wall -gdwarf -o $(OUT) $(SRC) \
		-IC:/bin/devlib/SDL2/include/SDL2 -LC:/bin/devlib/SDL2/lib \
		-I../scene \
		-lmingw32 -lSDL2main -lSDL2 -mwindows -mconsole

run: $(OUT)
	./$(OUT)