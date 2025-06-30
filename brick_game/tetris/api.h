#ifndef C_API_H_
#define C_API_H_

#define FIELD_H 20
#define FIELD_W 10
#include <stdbool.h>
#define EXIT_STATE 4

typedef enum {
  Start,
  Pause,
  Terminate,
  Left,
  Right,
  Up,
  Down,
  Action
} UserAction_t;

typedef struct {
  int **field;
  int **next;
  int score;
  int highScore;
  int level;
  int speed;
  int pause;
} GameInfo_t;

typedef enum { None, GameStart, GamePause } GamePauseStatus_t;

void userInput(UserAction_t action, bool hold);
GameInfo_t updateCurrentState();

#endif