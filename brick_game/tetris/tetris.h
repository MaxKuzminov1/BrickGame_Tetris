#ifndef C_TETRIS_BACKEND_H_
#define C_TETRIS_BACKEND_H_

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "api.h"

#define ROWS_MAP 20
#define COLS_MAP 10
#define NEXT_SIZE 4
#define BLOCK_COUNT 7
#define NEW_LEVEL_THRESHOLD 600
#define MAX_LEVEL 10
#define EXIT_STATE 4

typedef enum {
  Initial = 0,
  Spawn,
  Moving,
  Shifting,
  Attaching,
  GameOver,
  Pauses,
  ExitState
} gameStatus_t;

typedef struct {
  int x;
  int y;
  int brick[4][4];
  char currentType;
  char nextType;
} Figure_t;

typedef struct {
  gameStatus_t status;
  int** field;
  Figure_t* figure;
  int** next;
  int score;
  int level;
  int speed;
  int pause;
  int highScore;
  UserAction_t action;
  bool newAction;
  unsigned long long timer;
  bool timerActivated;
  bool memAlloc;
} Game;

Game* getCurrentGameState(void);

void progressGameState(Game* gameState);

void initGame(Game* gameState);
void freeGameResources(Game* gameState);
void restartFunc(Game* gameState);
int** createMatrix(int rows, int cols);
void freeMatrix(int** matrix);

void createFigure(Game* gameState);
void generateNextFigure(Game* gameState);
void rotateFigure(Figure_t* figure);
void attachFigure(Game* gameState);
int** prepareFieldWithFigure(Game* gameState);
void rotate(int (*oldFigure)[4], int (*newFigure)[4], int size);

void moveBrick(Game* gameState);
void moveHorizontal(Game* gameState, int direction);
bool checkCollision(Game* gameState, int newX, int newY);
bool canMoveDown(Game* gameState);
void actionFunc(Game* gameState);

bool checkMoveToShiftFunc(const Game* gameState);
void handleInitialStateInput(Game* gameState);
void handleMovingStateInput(Game* gameState);
void pauseFunc(Game* gameState);
void shiftingFunc(Game* gameState);
void attachingFunc(Game* gameState);
void gameOverFunc(Game* gameState);
void exitFunc(Game* gameState);
int clearCompletedRows(int** field);

void countGameScore(Game* gameState);
void updateLevel(Game* gameState);
void saveMaxScore(int score);
int handleHighScore();

bool checkGameOver(const Game* gameState);

unsigned long long currentTimeMillis();
void cpTetrToTetr(Figure_t* dst, Figure_t* src);
bool isCellColliding(Game* gameState, int x, int y);
void setFieldCell(Game* gameState, int x, int y);
void processBrickCell(Game* gameState, int i, int j);
GameInfo_t fillGameInfo(Game* gameState);
void copyNextFigure(int** dest, int** src);
void freeGameInfo(GameInfo_t* info);
#endif