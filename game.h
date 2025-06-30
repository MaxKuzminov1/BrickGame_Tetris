#ifndef GAME_H
#define GAME_H

#include <stdlib.h>
#include <unistd.h>

#include "brick_game/tetris/api.h"
#include "gui/cli/frontend.h"

#define ACTION_BUTTON 32
#define TERMINATE_BUTTON 27
#define PAUSE_BUTTON 112
#define START_BUTTON 115

#define SUCCESS 0

void updateUserAction();
void gameLoop();
void actionUser(int userAction);
void freeGameInfo(GameInfo_t *info);

#endif