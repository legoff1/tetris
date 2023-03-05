all:
	g++ -o tetris_main tetris_main.cpp tetris.hpp tetrino.hpp frames_per_level.hpp -lSDL2