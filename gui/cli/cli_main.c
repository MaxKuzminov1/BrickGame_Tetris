#include "../../game.h"

int main() {
  initNcurses();
  gameLoop();
  endwin();
  return 0;
}

void gameLoop() {
  GameInfo_t gameInfo;
  while (1) {
    gameInfo = updateCurrentState();
    if (gameInfo.pause == EXIT_STATE) {
      freeGameInfo(&gameInfo);
      break;
    }
    frontendGeneralFunc(gameInfo);
    actionUser(getch());
    freeGameInfo(&gameInfo);
  }
}

void actionUser(int userAction) {
  UserAction_t action;

  switch (userAction) {
    case KEY_LEFT:
      action = Left;
      break;
    case KEY_RIGHT:
      action = Right;
      break;
    case KEY_UP:
      action = Up;
      break;
    case KEY_DOWN:
      action = Down;
      break;
    case ACTION_BUTTON:
      action = Action;
      break;
    case START_BUTTON:
      action = Start;
      break;
    case PAUSE_BUTTON:
      action = Pause;
      break;
    case TERMINATE_BUTTON:
      action = Terminate;
      break;
    default:
      return;
  }
  userInput(action, false);
}

void freeGameInfo(GameInfo_t *info) {
  if (info) {
    if (info->field) {
      if (info->field[0]) {
        free(info->field[0]);
      }
      free(info->field);
      info->field = NULL;
    }
    if (info->next) {
      if (info->next[0]) {
        free(info->next[0]);
      }
      free(info->next);
      info->next = NULL;
    }
  }
}