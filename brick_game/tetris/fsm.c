#include "api.h"
#include "tetris.h"

Game *getCurrentGameState() {
  static Game gameState;
  return &gameState;
}

void progressGameState(Game *gameState) {
  if (gameState) {
    switch (gameState->status) {
      case Initial:
        handleInitialStateInput(gameState);
        break;
      case Spawn:
        createFigure(gameState);
        generateNextFigure(gameState);
        gameState->status = Moving;
        break;
      case Moving:
        handleMovingStateInput(gameState);
        break;
      case Pauses:
        pauseFunc(gameState);
        break;
      case Shifting:
        shiftingFunc(gameState);
        break;
      case Attaching:
        attachingFunc(gameState);
        break;
      case GameOver:
        gameOverFunc(gameState);
        break;
      case ExitState:
        exitFunc(gameState);
        break;
    }
  }
}

void handleInitialStateInput(Game *gameState) {
  if (gameState && gameState->newAction) {
    if (gameState->action == Start) {
      if (gameState->pause == 2) {
        restartFunc(gameState);
        gameState->pause = 0;
      }
      initGame(gameState);
      generateNextFigure(gameState);
      gameState->status = Spawn;
      gameState->pause = 0;
    } else if (gameState->action == Terminate) {
      gameState->status = ExitState;
    }
  }
}

void handleMovingStateInput(Game *gameState) {
  if (gameState->newAction) {
    switch (gameState->action) {
      case Action:
        actionFunc(gameState);
        break;
      case Down:
        gameState->status = Shifting;
        break;
      case Right:
        moveHorizontal(gameState, 1);
        break;
      case Left:
        moveHorizontal(gameState, -1);
        break;
      case Terminate:
        gameState->status = ExitState;
        progressGameState(gameState);
        break;
      case Pause:
        gameState->status = Pauses;
        gameState->pause = 3;
        break;
      default:
    }
  }
  if (checkMoveToShiftFunc(gameState)) gameState->status = Shifting;
  gameState->newAction = false;
}

void pauseFunc(Game *gameState) {
  if (gameState) {
    if (gameState->newAction) {
      switch (gameState->action) {
        case Pause:
          gameState->pause = 0;
          gameState->status = Moving;
          break;
        case Terminate:
          gameState->status = ExitState;
          break;
        default:
          break;
      }
    }
    gameState->newAction = false;
  }
}

void shiftingFunc(Game *gameState) {
  if (gameState) {
    if (!canMoveDown(gameState)) {
      gameState->status = Attaching;
    } else {
      moveBrick(gameState);
      gameState->status = Moving;
    }
  }
}

void attachingFunc(Game *gameState) {
  if (gameState) {
    attachFigure(gameState);
    countGameScore(gameState);
    if (checkGameOver(gameState))
      gameState->status = GameOver;
    else
      gameState->status = Spawn;
  }
}

void gameOverFunc(Game *gameState) {
  if (gameState) {
    gameState->status = Initial;
    gameState->pause = 2;
    gameState->newAction = false;
  }
}

void exitFunc(Game *gameState) {
  if (gameState) {
    if (gameState->field) {
      freeMatrix(gameState->field);
      gameState->field = NULL;
    }
    if (gameState->next) {
      freeMatrix(gameState->next);
      gameState->next = NULL;
    }
    if (gameState->figure) {
      free(gameState->figure);
      gameState->figure = NULL;
    }
    gameState->memAlloc = false;
    gameState->pause = EXIT_STATE;
    gameState->status = ExitState;
    gameState->newAction = false;
  }
}