#include "tetris.h"

int **createMatrix(int rows, int cols) {
  int **matrix = NULL;
  int *data = NULL;
  int valid = (rows > 0 && cols > 0 && rows <= 1000 && cols <= 1000 &&
               rows <= INT_MAX / cols);

  if (valid) {
    matrix = malloc(rows * sizeof(int *));
  }
  if (valid && matrix) {
    data = calloc(rows * cols, sizeof(int));
  }
  if (valid && matrix && data) {
    for (int i = 0; i < rows; i++) {
      matrix[i] = data + i * cols;
    }
  } else if (valid && matrix) {
    free(matrix);
    matrix = NULL;
  }

  return matrix;
}

void freeMatrix(int **matrix) {
  if (matrix) {
    free(matrix[0]);
    free(matrix);
  }
}
void freeGameResources(Game *gameState) {
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
  }
}

void initGame(Game *gameState) {
  if (!gameState) return;

  freeGameResources(gameState);

  bool allocationSuccess = true;

  gameState->field = createMatrix(ROWS_MAP, COLS_MAP);
  if (!gameState->field) allocationSuccess = false;

  gameState->next = createMatrix(NEXT_SIZE, NEXT_SIZE);
  if (!gameState->next) allocationSuccess = false;

  gameState->figure = malloc(sizeof(Figure_t));
  if (!gameState->figure) allocationSuccess = false;

  if (!allocationSuccess) {
    freeGameResources(gameState);
  } else {
    for (int i = 0; i < ROWS_MAP; i++) {
      for (int j = 0; j < COLS_MAP; j++) {
        gameState->field[i][j] = 0;
      }
    }

    gameState->score = 0;
    gameState->level = 1;
    gameState->status = Initial;
    gameState->pause = 1;
    gameState->highScore = handleHighScore();
    gameState->memAlloc = true;

    generateNextFigure(gameState);
  }
}

void restartFunc(Game *gameState) {
  if (!gameState) return;

  if (gameState->field) {
    for (int i = 0; i < ROWS_MAP; i++) {
      for (int j = 0; j < COLS_MAP; j++) {
        gameState->field[i][j] = 0;
      }
    }
  }
  gameState->score = 0;
  gameState->level = 1;
  gameState->pause = 0;
  gameState->status = Initial;
}

void createFigure(Game *gameState) {
  if (!gameState || !gameState->figure || !gameState->next) return;

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      gameState->figure->brick[i][j] = gameState->next[i][j];
    }
  }

  gameState->figure->x = 0;
  gameState->figure->y = 4;
  gameState->figure->currentType = gameState->figure->nextType;
  gameState->figure->nextType = 0;
}

void generateNextFigure(Game *gameState) {
  if (!gameState || !gameState->figure || !gameState->next) return;

  const int figures[7][4][4] = {
      {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
      {{0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
      {{0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
      {{1, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
      {{0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
      {{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
      {{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}};
  const char types[] = {'i', 'o', 't', 'j', 'l', 's', 'z'};
  int index = rand() % 7;
  memcpy(gameState->next[0], figures[index], sizeof(int) * 16);
  gameState->figure->nextType = types[index];
}

void rotateFigure(Figure_t *figure) {
  if (!figure) return;

  int size = (figure->currentType == 'i') ? 4 : 3;
  int oldBlock[4][4];

  if (figure->currentType == 'i') {
    for (int i = 0; i < size; i++)
      for (int j = 0; j < size; j++) oldBlock[j][i] = figure->brick[i][j];
    for (int i = 0; i < size; i++)
      for (int j = 0; j < size; j++) figure->brick[i][j] = oldBlock[i][j];
  } else if (figure->currentType != 'o') {
    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        oldBlock[i][j] = figure->brick[i][j];
      }
    }
    rotate(oldBlock, figure->brick, size);
  }
}

void attachFigure(Game *gameState) {
  if (gameState && gameState->figure && gameState->field) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        processBrickCell(gameState, i, j);
      }
    }
  }
}

void rotate(int (*oldFigure)[4], int (*newFigure)[4], int size) {
  if (!oldFigure || !newFigure || size <= 0 || size > 4) {
    return;
  }
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      newFigure[i][j] = oldFigure[size - j - 1][i];
    }
  }
}

void moveBrick(Game *gameState) {
  if (!gameState || !gameState->figure) return;
  gameState->figure->x++;
}

void moveHorizontal(Game *gameState, int direction) {
  if (!gameState || !gameState->figure) return;
  if (!checkCollision(gameState, gameState->figure->x,
                      gameState->figure->y + direction)) {
    gameState->figure->y += direction;
  }
}

bool checkCollision(Game *gameState, int newX, int newY) {
  bool collision = false;

  if (gameState && gameState->figure) {
    for (int i = 0; i < 4 && !collision; i++) {
      for (int j = 0; j < 4 && !collision; j++) {
        if (gameState->figure->brick[i][j])
          collision = isCellColliding(gameState, newX + i, newY + j);
      }
    }
  } else {
    collision = true;
  }
  return collision;
}

bool canMoveDown(Game *gameState) {
  bool flag = false;
  if (gameState && gameState->figure)
    flag = !checkCollision(gameState, gameState->figure->x + 1,
                           gameState->figure->y);
  return flag;
}
void actionFunc(Game *gameState) {
  if (gameState == NULL || gameState->figure == NULL) return;

  Figure_t original_tetr;
  cpTetrToTetr(&original_tetr, gameState->figure);
  rotateFigure(gameState->figure);

  Figure_t rotated_tetr;
  cpTetrToTetr(&rotated_tetr, gameState->figure);

  const int shifts[] = {-1, 1, -2, 2};  // const массив
  bool success = false;

  if (!checkCollision(gameState, rotated_tetr.x, rotated_tetr.y)) {
    success = true;
  } else {
    for (int i = 0; i < 4 && !success; i++) {  // i объявлена в цикле
      cpTetrToTetr(gameState->figure, &rotated_tetr);
      gameState->figure->y += shifts[i];
      success = !checkCollision(gameState, gameState->figure->x,
                                gameState->figure->y);
    }
  }

  if (!success) cpTetrToTetr(gameState->figure, &original_tetr);
}

bool checkMoveToShiftFunc(const Game *gameState) {
  bool shouldMove = false;

  if (gameState->status != ExitState && !gameState->pause) {
    static unsigned long long lastTime = 0;
    const unsigned long long currentTime = currentTimeMillis();
    const unsigned long long lvlSpeed = 1000 / gameState->level;

    if (currentTime - lastTime >= lvlSpeed) {
      lastTime = currentTime;
      shouldMove = true;
    }
  }
  return shouldMove;
}

int clearCompletedRows(int **field) {
  int rowsCleared = 0;
  for (int i = ROWS_MAP - 1; i >= 0; i--) {
    int rowFull = 1;
    for (int j = 0; j < COLS_MAP; j++) {
      if (field[i][j] == 0) {
        rowFull = 0;
        break;
      }
    }
    if (rowFull) {
      for (int k = i; k > 0; k--) {
        memcpy(field[k], field[k - 1], COLS_MAP * sizeof(int));
      }
      memset(field[0], 0, COLS_MAP * sizeof(int));
      rowsCleared++;
      i++;
    }
  }
  return rowsCleared;
}

void countGameScore(Game *gameState) {
  int filled_lines = clearCompletedRows(gameState->field);
  if (filled_lines > 0) {
    if (filled_lines == 1)
      gameState->score += 100;
    else if (filled_lines == 2)
      gameState->score += 300;
    else if (filled_lines == 3)
      gameState->score += 700;
    else if (filled_lines == 4)
      gameState->score += 1500;
  }
  if (gameState->score > gameState->highScore) {
    gameState->highScore = gameState->score;
    saveMaxScore(gameState->highScore);
  }
  updateLevel(gameState);
}

void updateLevel(Game *gameState) {
  if (gameState->score >= gameState->level * NEW_LEVEL_THRESHOLD &&
      gameState->level < MAX_LEVEL) {
    gameState->level++;
  }
}

void saveMaxScore(int score) {
  FILE *file = fopen("./max_score.txt", "w");
  if (file) {
    fprintf(file, "%d", score);
    fclose(file);
  }
}

int handleHighScore() {
  int highScore = 0;
  FILE *file = fopen("./max_score.txt", "r");
  if (file) {
    if (fscanf(file, "%d", &highScore) != 1) {
      highScore = 0;
    }
    fclose(file);
  }
  return highScore;
}

bool checkGameOver(const Game *gameState) {
  bool gameOver = false;
  for (int j = 0; j < COLS_MAP && !gameOver; j++) {
    if (gameState->field[0][j]) {
      gameOver = true;
    }
  }
  return gameOver;
}

unsigned long long currentTimeMillis() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000ULL + tv.tv_usec / 1000;
}

void cpTetrToTetr(Figure_t *dst, Figure_t *src) {
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++) dst->brick[i][j] = src->brick[i][j];
  dst->x = src->x;
  dst->y = src->y;
  dst->currentType = src->currentType;
  dst->nextType = src->nextType;
}

bool isCellColliding(Game *gameState, int x, int y) {
  bool collision = false;
  if (x < 0 || x >= ROWS_MAP || y < 0 || y >= COLS_MAP)
    collision = true;
  else if (gameState->field == NULL)
    collision = false;
  else
    collision = (gameState->field[x][y] != 0);

  return collision;
}

void setFieldCell(Game *gameState, int x, int y) {
  if (x >= 0 && x < ROWS_MAP && y >= 0 && y < COLS_MAP) {
    gameState->field[x][y] = 1;
  }
}

void processBrickCell(Game *gameState, int i, int j) {
  if (gameState != NULL && gameState->figure != NULL) {
    if (gameState->figure->brick[i][j]) {
      int x = gameState->figure->x + i;
      int y = gameState->figure->y + j;
      setFieldCell(gameState, x, y);
    }
  }
}

void copyNextFigure(int **dest, int **src) {
  if (dest && src) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        dest[i][j] = src[i][j];
      }
    }
  }
}
