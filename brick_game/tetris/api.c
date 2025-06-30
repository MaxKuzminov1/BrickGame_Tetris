#include "api.h"

#include "tetris.h"
void userInput(UserAction_t action, bool hold) {
  (void)hold;
  Game* gameState = getCurrentGameState();
  if (action == Pause && gameState->status == Moving) {
    gameState->pause = gameState->pause == 0 ? 3 : 0;
  }
  if (!gameState->newAction) {
    gameState->action = action;
    gameState->newAction = true;
  }
}

GameInfo_t updateCurrentState() {
  GameInfo_t info = {0};
  Game* gameState = getCurrentGameState();

  if (gameState->status == ExitState) {
    info.pause = EXIT_STATE;
  } else {
    if (gameState->field == NULL || gameState->next == NULL) {
      initGame(gameState);
    }

    if (gameState->status != ExitState) {
      progressGameState(gameState);
    }

    info = fillGameInfo(gameState);
  }

  return info;
}

GameInfo_t fillGameInfo(Game* gameState) {
  GameInfo_t info = {0};

  if (gameState->status == ExitState) {
    info.pause = EXIT_STATE;
  } else {
    info.field = prepareFieldWithFigure(gameState);
    info.next = createMatrix(4, 4);
    copyNextFigure(info.next, gameState->next);
    info.score = gameState->score;
    info.level = gameState->level;
    info.speed = gameState->speed;
    info.highScore = gameState->highScore;

    if (gameState->status == Pauses)
      info.pause = 2;
    else if (gameState->status == GameOver)
      info.pause = 3;
    else if (gameState->status == Initial)
      info.pause = gameState->pause;
    else
      info.pause = 0;
  }

  return info;
}

int** prepareFieldWithFigure(Game* gameState) {
  if (!gameState) return NULL;

  int** field = createMatrix(ROWS_MAP, COLS_MAP);

  if (gameState->field) {
    memcpy(field[0], gameState->field[0], ROWS_MAP * COLS_MAP * sizeof(int));
  }

  if (gameState->figure) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        int x = gameState->figure->x + i;
        int y = gameState->figure->y + j;
        if (x >= 0 && x < ROWS_MAP && y >= 0 && y < COLS_MAP &&
            gameState->figure->brick[i][j])
          field[x][y] = 1;
      }
    }
  }

  return field;
}