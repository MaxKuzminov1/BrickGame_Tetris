#ifndef C_CLI_H_
#define C_CLI_H_

#include <curses.h>

#include "../../brick_game/tetris/api.h"

#define COLOR_DEFAULT 1
#define COLOR_BLOCK 2
#define COLOR_TEXT 3
#define COLOR_HIGHLIGHT 4
#define COLOR_DIM 5
#define COLOR_POPUP_BG 6
#define COLOR_POPUP_FRAME 7

void frontendGeneralFunc(GameInfo_t info);
void initNcurses();

#endif
