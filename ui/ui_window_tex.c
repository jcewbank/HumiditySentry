#include "ui_window.h"

#define UI_WND_TEX_BRDR_SZ (5)
const uint8_t UI_WND_TEX_TOPLEFT[] = {
    0, 0, 1, 1, 1,
    0, 1, 2, 2, 2,
    1, 2, 2, 1, 1,
    1, 2, 1, 1, 1,
    1, 2, 1, 1, 0,
};

const uint8_t UI_WND_TEX_TOP[] = {
    1,
    2,
    1,
    1,
    0,
};

const uint8_t UI_WND_TEX_TOPRIGHT[] = {
    1, 1, 1, 0, 0,
    2, 2, 2, 1, 0,
    1, 1, 2, 2, 1,
    1, 1, 1, 2, 1,
    0, 1, 1, 2, 1,
};

const uint8_t UI_WND_TEX_RIGHT[] = {
    0, 0, 1, 2, 1,
};

const uint8_t UI_WND_TEX_BOTRIGHT[] = {
    0, 1, 2, 2, 1,
    1, 2, 2, 2, 1,
    2, 2, 2, 1, 1,
    1, 1, 1, 1, 0,
    1, 1, 1, 0, 0,
};

const uint8_t UI_WND_TEX_BOT[] = {
    0,
    1,
    2,
    1,
    1,
};

const uint8_t UI_WND_TEX_BOTLEFT[] = {
    1, 2, 2, 1, 0,    
    1, 2, 2, 2, 1,
    1, 1, 2, 2, 2,
    0, 1, 1, 1, 1,
    0, 0, 1, 1, 1
};

const uint8_t UI_WND_TEX_LEFT[] = {
    1, 2, 1, 0, 0,
};