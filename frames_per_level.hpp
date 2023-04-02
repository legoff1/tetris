#include <stdint.h>

// setting the number of frames for each level according to the Tetris DS(Nintendo) guideline
const uint8_t FRAMES_PER_LEVEL[]={
    48,
	43,
	38,
	33,
	28,
	23,
	18,
	13,
	8,
	6,
	5,
    5,
    5,
	4,
    4,
    4, 
	3,
    3,
    3,
	2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
	1
};

// setting the seconds per frame to do the drop
const float SECONDS_PER_FRAME = 1.f/60.f;