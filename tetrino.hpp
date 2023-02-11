#include <stdint.h>
// =============================================== STRUCT TETRINO -> DEFINITION AND FUNCTION IMPLEMENTATIONS ==============================================
 class Tetrino
{
    public:
    const uint8_t *data;
    const int32_t side;
};

inline Tetrino tetrino(const uint8_t *data, int32_t side)
{
    return { data, side };
}

const uint8_t TETRINO_1[] = { // TETRINO I 
    0, 0, 0, 0,
    1, 1, 1, 1,
    0, 0, 0, 0,
    0, 0, 0, 0
};

const uint8_t TETRINO_2[] = { // TETRINO O
    2, 2,
    2, 2
};

const uint8_t TETRINO_3[] = { // TETRINO T
    0, 0, 0,
    3, 3, 3,
    0, 3, 0
};

const uint8_t TETRINO_4[] = { // TETRINO S
    0, 4, 4,
    4, 4, 0,
    0, 0, 0
};

const uint8_t TETRINO_5[] = { // TETRINO Z
    5, 5, 0,
    0, 5, 5,
    0, 0, 0
};

const uint8_t TETRINO_6[] = { // TETRINO L
    6, 0, 0,
    6, 6, 6,
    0, 0, 0
};

const uint8_t TETRINO_7[] = { // // TETRINO J
    0, 0, 7,
    7, 7, 7,
    0, 0, 0
};


const Tetrino TETRINOS[] = {
    tetrino(TETRINO_1, 4),
    tetrino(TETRINO_2, 2),
    tetrino(TETRINO_3, 3),
    tetrino(TETRINO_4, 3),
    tetrino(TETRINO_5, 3),
    tetrino(TETRINO_6, 3),
    tetrino(TETRINO_7, 3),
};
