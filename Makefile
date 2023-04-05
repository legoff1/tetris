all:
	g++ -o tetris_main tetris_main.cpp board.hpp board.cpp tetris.hpp tetris.cpp colors.hpp tetrino.hpp frames_per_level.hpp -lSDL2 -lSDL2_ttf
	g++ -o tetris_multi_main tetris_main.cpp board_multi.hpp board_multi.cpp ../network/*.cpp tetris.hpp tetris.cpp colors.hpp tetrino.hpp frames_per_level.hpp -lSDL2 -lSDL2_ttf -lsfml-network