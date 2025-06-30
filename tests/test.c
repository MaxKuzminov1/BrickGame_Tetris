#include <check.h>
#include <unistd.h>

#include "../brick_game/tetris/tetris.h"

START_TEST(test_restart_null_pointer) { restartFunc(NULL); }
END_TEST

START_TEST(test_restart_partial_init) {
  Game gameState = {0};
  gameState.field = createMatrix(ROWS_MAP, COLS_MAP);
  gameState.field[10][5] = 1;

  restartFunc(&gameState);
  ck_assert_int_eq(gameState.field[10][5], 0);
  freeMatrix(gameState.field);
}
END_TEST

START_TEST(test_create_valid_matrix) {
  int **matrix = createMatrix(10, 10);
  ck_assert_ptr_nonnull(matrix);

  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      ck_assert_int_eq(matrix[i][j], 0);
    }
  }
  freeMatrix(matrix);
}
END_TEST

START_TEST(test_create_zero_rows) {
  int **matrix = createMatrix(0, 10);
  ck_assert_ptr_null(matrix);
}
END_TEST

START_TEST(test_create_zero_cols) {
  int **matrix = createMatrix(10, 0);
  ck_assert_ptr_null(matrix);
}
END_TEST

START_TEST(test_create_large_matrix) {
  int **matrix = createMatrix(10000, 10000);
  ck_assert_ptr_null(matrix);
}
END_TEST

START_TEST(test_create_negative_size) {
  int **matrix = createMatrix(-5, 10);
  ck_assert_ptr_null(matrix);
}
END_TEST

START_TEST(test_free_valid_matrix) {
  int **matrix = createMatrix(5, 5);
  freeMatrix(matrix);
}
END_TEST

START_TEST(test_free_null_pointer) { freeMatrix(NULL); }
END_TEST

START_TEST(test_free_already_freed) {
  int **matrix = createMatrix(5, 5);
  freeMatrix(matrix);
  matrix = NULL;
  freeMatrix(matrix);
}
END_TEST

START_TEST(test_create_figure_null_game) { createFigure(NULL); }
END_TEST

START_TEST(test_create_figure_no_next) {
  Game gameState = {0};
  gameState.next = NULL;
  createFigure(&gameState);
}
END_TEST

START_TEST(test_generate_null_game) { generateNextFigure(NULL); }
END_TEST

START_TEST(test_rotate_o_piece) {
  Figure_t figure = {
      .currentType = 'o',
      .brick = {{0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}};

  int original[4][4];
  memcpy(original, figure.brick, sizeof(original));

  rotateFigure(&figure);

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      ck_assert_int_eq(figure.brick[i][j], original[i][j]);
    }
  }
}
END_TEST

START_TEST(test_rotate_t_piece) {
  Figure_t figure = {
      .currentType = 't',
      .brick = {{0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}};

  rotateFigure(&figure);

  int expected[4][4] = {{0, 1, 0, 0}, {0, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}};

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      ck_assert_int_eq(figure.brick[i][j], expected[i][j]);
    }
  }
}
END_TEST

START_TEST(test_rotate_null_figure) { rotateFigure(NULL); }
END_TEST

START_TEST(test_rotate_unknown_type) {
  Figure_t figure = {
      .currentType = 'x',
      .brick = {{1, 0, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}}};

  rotateFigure(&figure);
}
END_TEST

START_TEST(test_attach_figure_normal) {
  Game gameState;
  memset(&gameState, 0, sizeof(Game));
  gameState.field = createMatrix(ROWS_MAP, COLS_MAP);
  ck_assert_ptr_nonnull(gameState.field);

  gameState.figure = malloc(sizeof(Figure_t));
  ck_assert_ptr_nonnull(gameState.figure);
  memset(gameState.figure, 0, sizeof(Figure_t));

  int figure_i[4][4] = {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}};
  memcpy(gameState.figure->brick, figure_i, sizeof(figure_i));
  gameState.figure->x = 0;
  gameState.figure->y = 4;

  attachFigure(&gameState);

  ck_assert_int_eq(gameState.field[1][4], 1);
  ck_assert_int_eq(gameState.field[1][5], 1);
  ck_assert_int_eq(gameState.field[1][6], 1);
  ck_assert_int_eq(gameState.field[1][7], 1);

  freeMatrix(gameState.field);
  free(gameState.figure);
}
END_TEST

START_TEST(test_attach_figure_completely_out_of_bounds) {
  Game gameState = {0};
  memset(&gameState, 0, sizeof(Game));
  gameState.field = createMatrix(ROWS_MAP, COLS_MAP);
  gameState.figure = malloc(sizeof(Figure_t));
  memset(gameState.figure, 0, sizeof(Figure_t));

  gameState.figure->brick[0][0] = 1;
  gameState.figure->x = 0;
  gameState.figure->y = COLS_MAP;

  attachFigure(&gameState);

  for (int i = 0; i < ROWS_MAP; i++) {
    for (int j = 0; j < COLS_MAP; j++) {
      ck_assert_int_eq(gameState.field[i][j], 0);
    }
  }

  freeMatrix(gameState.field);
  free(gameState.figure);
}
END_TEST

START_TEST(test_attach_figure_null_game) { attachFigure(NULL); }
END_TEST

START_TEST(test_attach_figure_no_field) {
  Game gameState;
  memset(&gameState, 0, sizeof(Game));
  gameState.figure = malloc(sizeof(Figure_t));
  gameState.figure->brick[0][0] = 1;

  attachFigure(&gameState);

  free(gameState.figure);
}
END_TEST

START_TEST(test_attach_figure_no_figure) {
  Game gameState;
  memset(&gameState, 0, sizeof(Game));
  gameState.field = createMatrix(ROWS_MAP, COLS_MAP);

  attachFigure(&gameState);

  freeMatrix(gameState.field);
}
END_TEST

START_TEST(test_prepare_field_normal) {
  Game gameState;
  memset(&gameState, 0, sizeof(Game));
  gameState.field = createMatrix(ROWS_MAP, COLS_MAP);
  gameState.figure = malloc(sizeof(Figure_t));
  memset(gameState.figure, 0, sizeof(Figure_t));

  int figure_t[4][4] = {{0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
  memcpy(gameState.figure->brick, figure_t, sizeof(figure_t));
  gameState.figure->x = 5;
  gameState.figure->y = 3;

  int **field = prepareFieldWithFigure(&gameState);

  ck_assert_int_eq(field[5][4], 1);
  ck_assert_int_eq(field[6][3], 1);
  ck_assert_int_eq(field[6][4], 1);
  ck_assert_int_eq(field[6][5], 1);

  freeMatrix(field);
  freeMatrix(gameState.field);
  free(gameState.figure);
}
END_TEST

START_TEST(test_prepare_field_no_figure) {
  Game gameState;
  memset(&gameState, 0, sizeof(Game));
  gameState.field = createMatrix(ROWS_MAP, COLS_MAP);

  int **field = prepareFieldWithFigure(&gameState);

  for (int i = 0; i < ROWS_MAP; i++) {
    for (int j = 0; j < COLS_MAP; j++) {
      ck_assert_int_eq(field[i][j], 0);
    }
  }

  freeMatrix(field);
  freeMatrix(gameState.field);
}
END_TEST

START_TEST(test_prepare_field_no_field_in_game) {
  Game gameState;
  memset(&gameState, 0, sizeof(Game));
  gameState.figure = malloc(sizeof(Figure_t));

  int **field = prepareFieldWithFigure(&gameState);
  ck_assert_ptr_nonnull(field);

  for (int i = 0; i < ROWS_MAP; i++) {
    for (int j = 0; j < COLS_MAP; j++) {
      ck_assert_int_eq(field[i][j], 0);
    }
  }

  freeMatrix(field);
  free(gameState.figure);
}
END_TEST

START_TEST(test_rotate_3x3_normal) {
  int oldFig[4][4] = {{0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
  int newFig[4][4] = {0};
  int expected[4][4] = {{0, 1, 0, 0}, {0, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}};

  rotate(oldFig, newFig, 3);

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      ck_assert_int_eq(newFig[i][j], expected[i][j]);
    }
  }
}
END_TEST

START_TEST(test_rotate_size_zero) {
  int oldFig[4][4] = {{1}};
  int newFig[4][4] = {0};

  rotate(oldFig, newFig, 0);

  ck_assert_int_eq(newFig[0][0], 0);
}
END_TEST

START_TEST(test_rotate_negative_size) {
  int oldFig[4][4] = {{1}};
  int newFig[4][4] = {0};

  rotate(oldFig, newFig, -1);

  ck_assert_int_eq(newFig[0][0], 0);
}
END_TEST

START_TEST(test_rotate_null_input) {
  int fig[4][4] = {{1}};

  rotate(NULL, fig, 3);
  rotate(fig, NULL, 3);
  rotate(NULL, NULL, 3);
}
END_TEST

START_TEST(test_rotate_invalid_size) {
  int oldFig[4][4] = {{1}};
  int newFig[4][4] = {0};

  rotate(oldFig, newFig, 5);

  ck_assert_int_eq(newFig[0][0], 0);
}
END_TEST

START_TEST(test_rotate_empty_figure) {
  int oldFig[4][4] = {0};
  int newFig[4][4] = {0};
  int expected[4][4] = {0};

  rotate(oldFig, newFig, 4);

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      ck_assert_int_eq(newFig[i][j], expected[i][j]);
    }
  }
}
END_TEST

START_TEST(test_move_brick_normal) {
  Game gameState;
  memset(&gameState, 0, sizeof(Game));
  gameState.figure = malloc(sizeof(Figure_t));
  gameState.figure->x = 5;

  moveBrick(&gameState);

  ck_assert_int_eq(gameState.figure->x, 6);

  free(gameState.figure);
}
END_TEST

START_TEST(test_move_brick_no_figure) {
  Game gameState;
  memset(&gameState, 0, sizeof(Game));

  moveBrick(&gameState);
}
END_TEST

START_TEST(test_move_brick_null_game) { moveBrick(NULL); }
END_TEST

START_TEST(test_move_brick_multiple_times) {
  Game gameState;
  memset(&gameState, 0, sizeof(Game));
  gameState.figure = malloc(sizeof(Figure_t));
  gameState.figure->x = 0;

  for (int i = 0; i < 10; i++) {
    moveBrick(&gameState);
  }

  ck_assert_int_eq(gameState.figure->x, 10);

  free(gameState.figure);
}
END_TEST

START_TEST(test_move_horizontal_collision_right) {
  Game gameState = {0};
  memset(&gameState, 0, sizeof(Game));
  gameState.field = createMatrix(ROWS_MAP, COLS_MAP);
  gameState.figure = malloc(sizeof(Figure_t));

  gameState.figure->y = COLS_MAP - 1;
  gameState.figure->brick[0][0] = 1;

  moveHorizontal(&gameState, 1);

  ck_assert_int_eq(gameState.figure->y, COLS_MAP - 1);

  freeMatrix(gameState.field);
  free(gameState.figure);
}
END_TEST

START_TEST(test_move_horizontal_collision_left) {
  Game gameState = {0};
  memset(&gameState, 0, sizeof(Game));
  gameState.field = createMatrix(ROWS_MAP, COLS_MAP);
  gameState.figure = malloc(sizeof(Figure_t));

  gameState.figure->y = 0;
  gameState.figure->brick[0][0] = 1;

  moveHorizontal(&gameState, -1);

  ck_assert_int_eq(gameState.figure->y, 0);

  freeMatrix(gameState.field);
  free(gameState.figure);
}
END_TEST

START_TEST(test_move_horizontal_null_game) { moveHorizontal(NULL, 1); }
END_TEST

START_TEST(test_move_horizontal_no_figure) {
  Game gameState = {0};
  memset(&gameState, 0, sizeof(Game));

  moveHorizontal(&gameState, 1);
}
END_TEST

START_TEST(test_move_horizontal_zero_direction) {
  Game gameState = {0};
  memset(&gameState, 0, sizeof(Game));
  gameState.figure = malloc(sizeof(Figure_t));
  gameState.figure->y = 5;

  moveHorizontal(&gameState, 0);

  ck_assert_int_eq(gameState.figure->y, 5);

  free(gameState.figure);
}
END_TEST

START_TEST(test_check_collision_normal_no_collision) {
  Game gameState = {0};
  memset(&gameState, 0, sizeof(Game));
  gameState.field = createMatrix(ROWS_MAP, COLS_MAP);
  gameState.figure = malloc(sizeof(Figure_t));

  gameState.figure->brick[0][0] = 1;

  bool collision = checkCollision(&gameState, 5, 5);

  ck_assert_int_eq(collision, false);

  freeMatrix(gameState.field);
  free(gameState.figure);
}
END_TEST

START_TEST(test_check_collision_wall_left) {
  Game gameState = {0};
  memset(&gameState, 0, sizeof(Game));
  gameState.field = createMatrix(ROWS_MAP, COLS_MAP);
  gameState.figure = malloc(sizeof(Figure_t));

  gameState.figure->brick[0][0] = 1;

  bool collision = checkCollision(&gameState, 5, -1);

  ck_assert_int_eq(collision, true);

  freeMatrix(gameState.field);
  free(gameState.figure);
}
END_TEST

START_TEST(test_check_collision_wall_right) {
  Game gameState = {0};
  memset(&gameState, 0, sizeof(Game));
  gameState.field = createMatrix(ROWS_MAP, COLS_MAP);
  gameState.figure = malloc(sizeof(Figure_t));

  gameState.figure->brick[0][0] = 1;

  bool collision = checkCollision(&gameState, 5, COLS_MAP);

  ck_assert_int_eq(collision, true);

  freeMatrix(gameState.field);
  free(gameState.figure);
}
END_TEST

START_TEST(test_check_collision_wall_bottom) {
  Game gameState = {0};
  memset(&gameState, 0, sizeof(Game));
  gameState.field = createMatrix(ROWS_MAP, COLS_MAP);
  gameState.figure = malloc(sizeof(Figure_t));

  gameState.figure->brick[0][0] = 1;

  bool collision = checkCollision(&gameState, ROWS_MAP, 5);

  ck_assert_int_eq(collision, true);

  freeMatrix(gameState.field);
  free(gameState.figure);
}
END_TEST

START_TEST(test_check_collision_block_collision) {
  Game gameState = {0};
  memset(&gameState, 0, sizeof(Game));
  gameState.field = createMatrix(ROWS_MAP, COLS_MAP);
  gameState.figure = malloc(sizeof(Figure_t));

  gameState.field[5][5] = 1;
  gameState.figure->brick[0][0] = 1;

  bool collision = checkCollision(&gameState, 5, 5);

  ck_assert_int_eq(collision, true);

  freeMatrix(gameState.field);
  free(gameState.figure);
}
END_TEST

START_TEST(test_check_collision_null_game) {
  bool collision = checkCollision(NULL, 0, 0);
  ck_assert_int_eq(collision, true);
}
END_TEST

START_TEST(test_check_collision_no_field) {
  Game gameState = {0};
  memset(&gameState, 0, sizeof(Game));
  gameState.figure = malloc(sizeof(Figure_t));
  gameState.figure->brick[0][0] = 1;

  bool collision = checkCollision(&gameState, 0, 0);
  ck_assert_int_eq(collision, false);

  free(gameState.figure);
}
END_TEST

START_TEST(test_can_move_down_no_collision) {
  Game gameState = {0};
  memset(&gameState, 0, sizeof(Game));
  gameState.field = createMatrix(ROWS_MAP, COLS_MAP);
  gameState.figure = malloc(sizeof(Figure_t));

  gameState.figure->x = ROWS_MAP - 1;
  gameState.figure->brick[0][0] = 1;

  bool canMove = canMoveDown(&gameState);

  ck_assert_int_eq(canMove, false);

  freeMatrix(gameState.field);
  free(gameState.figure);
}
END_TEST

START_TEST(test_can_move_down_block_below) {
  Game gameState = {0};
  memset(&gameState, 0, sizeof(Game));
  gameState.field = createMatrix(ROWS_MAP, COLS_MAP);
  gameState.figure = malloc(sizeof(Figure_t));

  gameState.field[5][5] = 1;
  gameState.figure->x = 4;
  gameState.figure->y = 5;
  gameState.figure->brick[0][0] = 1;

  bool canMove = canMoveDown(&gameState);

  ck_assert_int_eq(canMove, false);

  freeMatrix(gameState.field);
  free(gameState.figure);
}
END_TEST

START_TEST(test_can_move_down_null_game) {
  bool canMove = canMoveDown(NULL);
  ck_assert_int_eq(canMove, false);
}
END_TEST

START_TEST(test_can_move_down_no_figure) {
  Game gameState = {0};
  memset(&gameState, 0, sizeof(Game));

  bool canMove = canMoveDown(&gameState);
  ck_assert_int_eq(canMove, false);
}
END_TEST

START_TEST(test_action_func_no_rotation_possible) {
  Game gameState = {0};
  memset(&gameState, 0, sizeof(Game));
  gameState.field = createMatrix(ROWS_MAP, COLS_MAP);
  gameState.figure = malloc(sizeof(Figure_t));
  memset(gameState.figure, 0, sizeof(Figure_t));

  int figure_o[4][4] = {{0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
  memcpy(gameState.figure->brick, figure_o, sizeof(figure_o));
  gameState.figure->currentType = 'o';

  int original[4][4];
  memcpy(original, gameState.figure->brick, sizeof(original));

  actionFunc(&gameState);

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      ck_assert_int_eq(gameState.figure->brick[i][j], original[i][j]);
    }
  }

  freeMatrix(gameState.field);
  free(gameState.figure);
}
END_TEST

START_TEST(test_action_func_null_game) { actionFunc(NULL); }
END_TEST

START_TEST(test_action_func_no_figure) {
  Game gameState = {0};
  memset(&gameState, 0, sizeof(Game));

  actionFunc(&gameState);
}
END_TEST

START_TEST(test_game_over_top_row_filled) {
  Game game = {0};
  game.field = createMatrix(ROWS_MAP, COLS_MAP);

  for (int j = 0; j < COLS_MAP; j++) {
    game.field[0][j] = 1;
  }

  ck_assert(checkGameOver(&game));
  freeMatrix(game.field);
}
END_TEST

START_TEST(test_game_over_single_cell_filled) {
  Game game = {0};
  game.field = createMatrix(ROWS_MAP, COLS_MAP);

  game.field[0][3] = 1;

  ck_assert(checkGameOver(&game));
  freeMatrix(game.field);
}
END_TEST

START_TEST(test_game_over_not_lost) {
  Game game = {0};
  game.field = createMatrix(ROWS_MAP, COLS_MAP);

  for (int j = 0; j < COLS_MAP; j++) {
    game.field[5][j] = 1;
  }

  ck_assert(!checkGameOver(&game));
  freeMatrix(game.field);
}
END_TEST

START_TEST(test_level_up_normal) {
  Game game = {0};
  game.level = 1;
  game.score = NEW_LEVEL_THRESHOLD + 100;

  updateLevel(&game);
  ck_assert_int_eq(game.level, 2);
}
END_TEST

START_TEST(test_level_up_edge) {
  Game game = {0};
  game.level = 1;
  game.score = NEW_LEVEL_THRESHOLD - 1;

  updateLevel(&game);
  ck_assert_int_eq(game.level, 1);
}
END_TEST

START_TEST(test_level_max_reached) {
  Game game = {0};
  game.level = MAX_LEVEL;
  game.score = MAX_LEVEL * NEW_LEVEL_THRESHOLD + 1000;

  updateLevel(&game);
  ck_assert_int_eq(game.level, MAX_LEVEL);
}
END_TEST

START_TEST(test_multiple_level_ups) {
  Game game = {0};
  game.level = 1;
  game.score = NEW_LEVEL_THRESHOLD * 3 + 100;

  updateLevel(&game);
  ck_assert_int_eq(game.level, 2);
}
END_TEST

START_TEST(test_save_and_load_score) {
  const int test_score = 12345;

  saveMaxScore(test_score);
  int loaded = handleHighScore();

  ck_assert_int_eq(loaded, test_score);
}
END_TEST

START_TEST(test_save_zero_score) {
  saveMaxScore(0);
  ck_assert_int_eq(handleHighScore(), 0);
}
END_TEST

START_TEST(test_save_large_score) {
  saveMaxScore(INT_MAX);
  ck_assert_int_eq(handleHighScore(), INT_MAX);
}
END_TEST

START_TEST(test_no_score_file) {
  remove("./max_score.txt");
  ck_assert_int_eq(handleHighScore(), 0);
}
END_TEST

START_TEST(test_score_single_line) {
  Game game = {0};
  game.field = createMatrix(ROWS_MAP, COLS_MAP);
  game.highScore = 0;

  for (int j = 0; j < COLS_MAP; j++) {
    game.field[ROWS_MAP - 1][j] = 1;
  }

  countGameScore(&game);
  ck_assert_int_eq(game.score, 100);
  freeMatrix(game.field);
}
END_TEST

START_TEST(test_score_four_lines) {
  Game game = {0};
  game.field = createMatrix(ROWS_MAP, COLS_MAP);
  game.highScore = 0;

  for (int i = ROWS_MAP - 4; i < ROWS_MAP; i++) {
    for (int j = 0; j < COLS_MAP; j++) {
      game.field[i][j] = 1;
    }
  }

  countGameScore(&game);
  ck_assert_int_eq(game.score, 1500);
  freeMatrix(game.field);
}
END_TEST

START_TEST(test_score_mixed_lines) {
  Game game = {0};
  game.field = createMatrix(ROWS_MAP, COLS_MAP);
  game.highScore = 0;

  for (int i = ROWS_MAP - 2; i < ROWS_MAP; i++) {
    for (int j = 0; j < COLS_MAP; j++) {
      game.field[i][j] = 1;
    }
  }

  for (int j = 0; j < 5; j++) {
    game.field[ROWS_MAP - 3][j] = 1;
  }

  countGameScore(&game);
  ck_assert_int_eq(game.score, 300);
  freeMatrix(game.field);
}
END_TEST

START_TEST(test_high_score_update) {
  Game game = {0};
  game.field = createMatrix(ROWS_MAP, COLS_MAP);
  game.highScore = 1000;
  game.score = 500;

  for (int j = 0; j < COLS_MAP; j++) {
    game.field[ROWS_MAP - 1][j] = 1;
  }

  countGameScore(&game);
  ck_assert_int_eq(game.highScore, 1000);
  freeMatrix(game.field);
}
END_TEST

START_TEST(test_shift_immediate) {
  Game game = {0};
  game.status = Moving;
  game.pause = 0;
  game.level = 1;

  checkMoveToShiftFunc(&game);

  bool result = checkMoveToShiftFunc(&game);
  ck_assert(!result);
}
END_TEST

START_TEST(test_shift_paused_game) {
  Game game = {0};
  game.status = Pauses;
  game.pause = 1;
  game.level = 1;

  ck_assert(!checkMoveToShiftFunc(&game));
}
END_TEST

START_TEST(test_shift_exit_state) {
  Game game = {0};
  game.status = ExitState;
  game.level = 1;

  ck_assert(!checkMoveToShiftFunc(&game));
}
END_TEST

START_TEST(test_clear_no_lines) {
  int **field = createMatrix(ROWS_MAP, COLS_MAP);

  for (int i = 10; i < 15; i++) {
    for (int j = 0; j < 5; j++) {
      field[i][j] = 1;
    }
  }

  int cleared = clearCompletedRows(field);
  ck_assert_int_eq(cleared, 0);
  freeMatrix(field);
}
END_TEST

START_TEST(test_clear_single_line) {
  int **field = createMatrix(ROWS_MAP, COLS_MAP);

  for (int j = 0; j < COLS_MAP; j++) {
    field[10][j] = 1;
  }

  int cleared = clearCompletedRows(field);
  ck_assert_int_eq(cleared, 1);
  freeMatrix(field);
}
END_TEST

START_TEST(test_clear_consecutive_lines) {
  int **field = createMatrix(ROWS_MAP, COLS_MAP);

  for (int i = 10; i < 13; i++) {
    for (int j = 0; j < COLS_MAP; j++) {
      field[i][j] = 1;
    }
  }

  int cleared = clearCompletedRows(field);
  ck_assert_int_eq(cleared, 3);
  freeMatrix(field);
}
END_TEST

START_TEST(test_clear_non_consecutive) {
  int **field = createMatrix(ROWS_MAP, COLS_MAP);

  for (int j = 0; j < COLS_MAP; j++) {
    field[5][j] = 1;
    field[10][j] = 1;
  }

  int cleared = clearCompletedRows(field);
  ck_assert_int_eq(cleared, 2);
  freeMatrix(field);
}
END_TEST

START_TEST(test_clear_all_lines) {
  int **field = createMatrix(ROWS_MAP, COLS_MAP);

  for (int i = 0; i < ROWS_MAP; i++) {
    for (int j = 0; j < COLS_MAP; j++) {
      field[i][j] = 1;
    }
  }

  int cleared = clearCompletedRows(field);
  ck_assert_int_eq(cleared, ROWS_MAP);
  freeMatrix(field);
}
END_TEST

START_TEST(test_clear_partial_line) {
  int **field = createMatrix(ROWS_MAP, COLS_MAP);

  for (int j = 0; j < COLS_MAP - 1; j++) {
    field[10][j] = 1;
  }

  int cleared = clearCompletedRows(field);
  ck_assert_int_eq(cleared, 0);
  freeMatrix(field);
}
END_TEST

START_TEST(test_clear_with_gap) {
  int **field = createMatrix(ROWS_MAP, COLS_MAP);

  for (int j = 0; j < COLS_MAP; j++) {
    field[10][j] = 1;
  }
  field[10][5] = 0;

  int cleared = clearCompletedRows(field);
  ck_assert_int_eq(cleared, 0);
  freeMatrix(field);
}
END_TEST

START_TEST(test_clear_top_line) {
  int **field = createMatrix(ROWS_MAP, COLS_MAP);

  for (int j = 0; j < COLS_MAP; j++) {
    field[0][j] = 1;
  }

  int cleared = clearCompletedRows(field);
  ck_assert_int_eq(cleared, 1);

  for (int j = 0; j < COLS_MAP; j++) {
    ck_assert_int_eq(field[0][j], 0);
  }
  freeMatrix(field);
}
END_TEST

START_TEST(test_score_level_up_trigger) {
  Game game = {0};
  game.field = createMatrix(ROWS_MAP, COLS_MAP);
  game.level = 1;
  game.score = 0;

  for (int i = ROWS_MAP - 4; i < ROWS_MAP; i++) {
    for (int j = 0; j < COLS_MAP; j++) {
      game.field[i][j] = 1;
    }
  }

  countGameScore(&game);
  ck_assert_int_eq(game.level, 2);
  freeMatrix(game.field);
}
END_TEST

START_TEST(test_score_multiple_clears) {
  Game game = {0};
  game.field = createMatrix(ROWS_MAP, COLS_MAP);
  game.score = 0;

  for (int j = 0; j < COLS_MAP; j++) game.field[18][j] = 1;
  countGameScore(&game);
  ck_assert_int_eq(game.score, 100);

  for (int i = 16; i <= 17; i++) {
    for (int j = 0; j < COLS_MAP; j++) game.field[i][j] = 1;
  }
  countGameScore(&game);
  ck_assert_int_eq(game.score, 100 + 300);
  freeMatrix(game.field);
}
END_TEST

START_TEST(test_level_up_exact_threshold) {
  Game game = {0};
  game.level = 1;
  game.score = NEW_LEVEL_THRESHOLD;

  updateLevel(&game);
  ck_assert_int_eq(game.level, 2);
}
END_TEST

START_TEST(test_level_up_multiple_thresholds) {
  Game game = {0};
  game.level = 1;
  game.score = NEW_LEVEL_THRESHOLD * 3;

  updateLevel(&game);
  ck_assert_int_eq(game.level, 2);
}
END_TEST

START_TEST(test_initGame_null) { initGame(NULL); }
END_TEST

START_TEST(test_initGame_normal) {
  Game gameState = {0};
  initGame(&gameState);

  ck_assert_ptr_nonnull(gameState.field);
  ck_assert_ptr_nonnull(gameState.next);
  ck_assert_ptr_nonnull(gameState.figure);
  ck_assert_int_eq(gameState.status, Initial);
  ck_assert_int_eq(gameState.pause, 1);
  ck_assert_int_eq(gameState.score, 0);
  ck_assert_int_eq(gameState.level, 1);

  freeGameResources(&gameState);
}
END_TEST

START_TEST(test_initGame_memory_fail) {
  Game gameState = {0};

  initGame(&gameState);
  ck_assert(gameState.memAlloc == true ||
            (gameState.field == NULL && gameState.next == NULL &&
             gameState.figure == NULL));

  if (gameState.memAlloc) freeGameResources(&gameState);
}
END_TEST

START_TEST(test_initGame_after_reset) {
  Game gameState = {0};
  initGame(&gameState);
  gameState.score = 100;
  gameState.level = 5;

  restartFunc(&gameState);
  initGame(&gameState);

  ck_assert_int_eq(gameState.score, 0);
  ck_assert_int_eq(gameState.level, 1);

  freeGameResources(&gameState);
}
END_TEST

START_TEST(test_freeGameResources_null) { freeGameResources(NULL); }
END_TEST

START_TEST(test_freeGameResources_normal) {
  Game gameState = {0};
  initGame(&gameState);
  freeGameResources(&gameState);

  ck_assert_ptr_null(gameState.field);
  ck_assert_ptr_null(gameState.next);
  ck_assert_ptr_null(gameState.figure);
  ck_assert_int_eq(gameState.memAlloc, false);
  freeGameResources(&gameState);
}
END_TEST

START_TEST(test_freeGameResources_partial) {
  Game gameState = {0};

  gameState.field = createMatrix(ROWS_MAP, COLS_MAP);
  gameState.memAlloc = true;

  freeGameResources(&gameState);
  ck_assert_ptr_null(gameState.field);
  ck_assert_int_eq(gameState.memAlloc, false);
}
END_TEST

START_TEST(test_freeGameResources_double_free) {
  Game gameState = {0};
  initGame(&gameState);
  freeGameResources(&gameState);
  freeGameResources(&gameState);

  ck_assert_ptr_null(gameState.field);
  ck_assert_ptr_null(gameState.next);
  ck_assert_ptr_null(gameState.figure);
  freeGameResources(&gameState);
}
END_TEST

START_TEST(test_createFigure_null) { createFigure(NULL); }
END_TEST

START_TEST(test_createFigure_normal) {
  Game gameState = {0};
  initGame(&gameState);

  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++) gameState.next[i][j] = (i == j) ? 1 : 0;

  createFigure(&gameState);

  ck_assert_ptr_nonnull(gameState.figure);
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      ck_assert_int_eq(gameState.figure->brick[i][j], (i == j) ? 1 : 0);

  ck_assert_int_eq(gameState.figure->x, 0);
  ck_assert_int_eq(gameState.figure->y, 4);

  freeGameResources(&gameState);
}
END_TEST

START_TEST(test_createFigure_no_next) {
  Game gameState = {0};
  gameState.next = NULL;

  createFigure(&gameState);
  ck_assert_ptr_null(gameState.figure);
}
END_TEST

START_TEST(test_createFigure_no_memory) {
  Game gameState = {0};
  initGame(&gameState);
  free(gameState.figure);
  gameState.figure = NULL;

  createFigure(&gameState);
  ck_assert_ptr_null(gameState.figure);

  freeGameResources(&gameState);
}
END_TEST

START_TEST(test_createFigure_existing_figure) {
  Game gameState = {0};
  initGame(&gameState);
  Figure_t *original = gameState.figure;

  createFigure(&gameState);
  ck_assert_ptr_eq(original, gameState.figure);

  freeGameResources(&gameState);
}
END_TEST

START_TEST(test_generateNextFigure_null) { generateNextFigure(NULL); }
END_TEST

START_TEST(test_generateNextFigure_normal) {
  Game gameState = {0};
  initGame(&gameState);

  generateNextFigure(&gameState);

  int sum = 0;
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++) sum += gameState.next[i][j];

  ck_assert_int_gt(sum, 0);
  ck_assert_int_ne(gameState.figure->nextType, 0);

  freeGameResources(&gameState);
}
END_TEST

START_TEST(test_generateNextFigure_no_memory) {
  Game gameState = {0};
  gameState.next = NULL;

  generateNextFigure(&gameState);
}
END_TEST

START_TEST(test_generateNextFigure_after_reset) {
  Game gameState = {0};
  initGame(&gameState);
  char first_type = gameState.figure->nextType;

  generateNextFigure(&gameState);
  char second_type = gameState.figure->nextType;

  ck_assert_int_ne(first_type, second_type);

  freeGameResources(&gameState);
}
END_TEST

START_TEST(test_generateNextFigure_all_types) {
  Game gameState = {0};
  initGame(&gameState);
  bool types_found[256] = {false};
  const char valid_types[] = {'i', 'o', 't', 'j', 'l', 's', 'z'};

  for (int i = 0; i < 100; i++) {
    generateNextFigure(&gameState);
    types_found[(unsigned char)gameState.figure->nextType] = true;
  }

  for (int i = 0; i < 7; i++) {
    ck_assert(types_found[(unsigned char)valid_types[i]]);
  }

  freeGameResources(&gameState);
}
END_TEST

START_TEST(test_gameOverFunc_null) { gameOverFunc(NULL); }
END_TEST

START_TEST(test_gameOverFunc_normal) {
  Game gameState = {0};
  gameState.status = Moving;
  gameState.pause = 0;
  gameState.newAction = true;

  gameOverFunc(&gameState);

  ck_assert_int_eq(gameState.status, Initial);
  ck_assert_int_eq(gameState.pause, 2);
  ck_assert_int_eq(gameState.newAction, false);
}
END_TEST

START_TEST(test_gameOverFunc_from_different_state) {
  Game gameState = {0};
  gameState.status = Attaching;
  gameOverFunc(&gameState);
  ck_assert_int_eq(gameState.status, Initial);
}
END_TEST

START_TEST(test_gameOverFunc_with_active_timer) {
  Game gameState = {0};
  gameState.timerActivated = true;
  gameState.timer = 1000;

  gameOverFunc(&gameState);
  ck_assert_int_eq(gameState.timerActivated, true);
}
END_TEST

START_TEST(test_gameOverFunc_after_gameplay) {
  Game gameState = {0};
  initGame(&gameState);
  gameState.score = 500;
  gameState.level = 3;

  gameOverFunc(&gameState);

  ck_assert_int_eq(gameState.score, 500);
  ck_assert_int_eq(gameState.level, 3);

  freeGameResources(&gameState);
}
END_TEST

START_TEST(test_attachingFunc_null) { attachingFunc(NULL); }
END_TEST

START_TEST(test_attachingFunc_normal) {
  Game gameState = {0};
  initGame(&gameState);

  gameState.figure->x = 18;
  gameState.figure->y = 3;
  attachingFunc(&gameState);

  ck_assert(gameState.status == Spawn || gameState.status == GameOver);
  freeGameResources(&gameState);
}
END_TEST

START_TEST(test_attachingFunc_causes_gameover) {
  Game gameState = {0};
  initGame(&gameState);

  for (int i = 0; i < 3; i++)
    for (int j = 0; j < COLS_MAP; j++) gameState.field[i][j] = 1;

  gameState.figure->x = 0;
  attachingFunc(&gameState);

  ck_assert_int_eq(gameState.status, 1);
  freeGameResources(&gameState);
}
END_TEST

START_TEST(test_attachingFunc_score_calculation) {
  Game gameState = {0};
  initGame(&gameState);
  int initial_score = gameState.score = 100;

  for (int j = 0; j < COLS_MAP; j++) {
    gameState.field[18][j] = 1;
    gameState.field[19][j] = 1;
  }

  attachingFunc(&gameState);
  ck_assert_int_gt(gameState.score, initial_score);
  freeGameResources(&gameState);
}
END_TEST

START_TEST(test_attachingFunc_level_up) {
  Game gameState = {0};
  initGame(&gameState);
  gameState.score = NEW_LEVEL_THRESHOLD * 2;

  attachingFunc(&gameState);
  ck_assert_int_eq(gameState.level, 2);
  freeGameResources(&gameState);
}
END_TEST

START_TEST(test_shiftingFunc_null) { shiftingFunc(NULL); }
END_TEST

START_TEST(test_pauseFunc_null) { pauseFunc(NULL); }
END_TEST

START_TEST(test_pauseFunc_resume) {
  Game gameState = {0};
  gameState.status = Pauses;
  gameState.newAction = true;
  gameState.action = Pause;

  pauseFunc(&gameState);
  ck_assert_int_eq(gameState.status, Moving);
  ck_assert_int_eq(gameState.pause, 0);
  ck_assert_int_eq(gameState.newAction, false);
}
END_TEST

START_TEST(test_pauseFunc_ignore_other_actions) {
  Game gameState = {0};
  gameState.status = Pauses;
  gameState.newAction = true;
  gameState.action = Left;

  pauseFunc(&gameState);
  ck_assert_int_eq(gameState.status, Pauses);
  ck_assert_int_eq(gameState.newAction, false);
}
END_TEST

START_TEST(test_pauseFunc_no_action) {
  Game gameState = {0};
  gameState.status = Pauses;
  gameState.newAction = false;

  pauseFunc(&gameState);
  ck_assert_int_eq(gameState.status, Pauses);
}
END_TEST

START_TEST(test_progress_initial_to_spawn) {
  Game *game = getCurrentGameState();
  initGame(game);
  game->status = Initial;
  game->action = Start;
  game->newAction = true;

  progressGameState(game);
  ck_assert_int_eq(game->status, Spawn);
  freeGameResources(game);
}
END_TEST

START_TEST(test_progress_spawn_to_moving) {
  Game *game = getCurrentGameState();
  initGame(game);
  game->status = Spawn;

  progressGameState(game);
  ck_assert_int_eq(game->status, Moving);
  ck_assert_ptr_nonnull(game->figure);
  freeGameResources(game);
}
END_TEST

START_TEST(test_progress_moving_to_shifting_by_time) {
  Game *game = getCurrentGameState();
  initGame(game);
  game->status = Moving;
  game->level = 1;

  progressGameState(game);
  ck_assert_int_eq(game->status, 2);
  freeGameResources(game);
}
END_TEST

START_TEST(test_progress_moving_to_pause) {
  Game *game = getCurrentGameState();
  initGame(game);
  game->status = Moving;
  game->action = Pause;
  game->newAction = true;

  progressGameState(game);
  ck_assert_int_eq(game->status, Pauses);
  freeGameResources(game);
}
END_TEST

START_TEST(test_progress_attaching_to_gameover) {
  Game *game = getCurrentGameState();
  initGame(game);
  game->status = Attaching;

  for (int i = 0; i < COLS_MAP; i++) {
    game->field[0][i] = 1;
  }

  progressGameState(game);
  ck_assert_int_eq(game->status, 1);
  freeGameResources(game);
}
END_TEST

START_TEST(test_progress_gameover_to_initial) {
  Game *game = getCurrentGameState();
  initGame(game);
  game->status = GameOver;

  progressGameState(game);
  ck_assert_int_eq(game->status, Initial);
  freeGameResources(game);
}
END_TEST

START_TEST(test_progress_exit_state) {
  Game *game = getCurrentGameState();
  game->status = ExitState;

  progressGameState(game);
  ck_assert_int_eq(game->status, ExitState);
}
END_TEST

START_TEST(test_progress_null_pointer) {
  progressGameState(NULL);

  ck_assert(1);
}
END_TEST

START_TEST(test_progress_invalid_state) {
  Game *game = getCurrentGameState();
  game->status = 100;

  progressGameState(game);

  ck_assert_int_eq(game->status, 100);
}
END_TEST

START_TEST(test_initial_start_action) {
  Game *game = getCurrentGameState();
  initGame(game);
  game->status = Initial;
  game->pause = 2;
  game->action = Start;
  game->newAction = true;

  handleInitialStateInput(game);
  ck_assert_int_eq(game->status, Spawn);
  ck_assert_int_eq(game->pause, 0);
  freeGameResources(game);
}
END_TEST

START_TEST(test_initial_terminate_action) {
  Game *game = getCurrentGameState();
  game->status = Initial;
  game->action = Terminate;
  game->newAction = true;

  handleInitialStateInput(game);
  ck_assert_int_eq(game->status, ExitState);
}
END_TEST

START_TEST(test_initial_restart_after_gameover) {
  Game *game = getCurrentGameState();
  game->status = Initial;
  game->pause = 2;
  game->action = Start;
  game->newAction = true;

  handleInitialStateInput(game);
  ck_assert_int_eq(game->score, 0);
  ck_assert_int_eq(game->level, 1);
}
END_TEST

START_TEST(test_initial_invalid_action) {
  Game *game = getCurrentGameState();
  game->status = Initial;
  game->action = Down;
  game->newAction = true;

  handleInitialStateInput(game);
  ck_assert_int_eq(game->status, Initial);
}
END_TEST

START_TEST(test_initial_no_action) {
  Game *game = getCurrentGameState();
  game->status = Initial;
  game->newAction = false;

  handleInitialStateInput(game);
  ck_assert_int_eq(game->status, Initial);
}
END_TEST

START_TEST(test_initial_null_pointer) {
  handleInitialStateInput(NULL);
  ck_assert(1);
}
END_TEST

START_TEST(test_initial_memory_failure) {
  Game *game = getCurrentGameState();
  game->status = Initial;
  game->action = Start;
  game->newAction = true;

  freeMatrix(game->field);
  game->field = NULL;

  ck_assert_ptr_null(game->field);
}
END_TEST

START_TEST(test_initial_high_score_persistence) {
  int original_high = handleHighScore();
  saveMaxScore(5000);

  Game *game = getCurrentGameState();
  game->status = Initial;
  game->action = Start;
  game->newAction = true;

  handleInitialStateInput(game);
  ck_assert_int_eq(game->highScore, 5000);

  saveMaxScore(original_high);
}
END_TEST

START_TEST(test_initial_double_start) {
  Game *game = getCurrentGameState();
  game->status = Initial;
  game->action = Start;
  game->newAction = true;

  handleInitialStateInput(game);
  handleInitialStateInput(game);

  ck_assert_int_eq(game->status, Spawn);
}
END_TEST

START_TEST(test_initial_terminate_after_start) {
  Game *game = getCurrentGameState();
  game->status = Initial;
  game->action = Terminate;
  game->newAction = true;

  handleInitialStateInput(game);
  ck_assert_int_eq(game->status, ExitState);
}
END_TEST

START_TEST(test_moving_down_action) {
  Game *game = getCurrentGameState();
  initGame(game);
  game->status = Moving;
  game->action = Down;
  game->newAction = true;

  handleMovingStateInput(game);
  ck_assert_int_eq(game->status, Shifting);
  freeGameResources(game);
}
END_TEST

START_TEST(test_moving_rotate_action) {
  Game *game = getCurrentGameState();
  initGame(game);
  game->status = Moving;
  game->action = Action;
  game->newAction = true;
  Figure_t original;
  cpTetrToTetr(&original, game->figure);

  handleMovingStateInput(game);
  ck_assert(memcmp(&original, game->figure, sizeof(Figure_t)) != 0);
  freeGameResources(game);
}
END_TEST

START_TEST(test_moving_pause_action) {
  Game *game = getCurrentGameState();
  initGame(game);
  game->status = Moving;
  game->action = Pause;
  game->newAction = true;

  handleMovingStateInput(game);
  ck_assert_int_eq(game->status, Pauses);
  ck_assert_int_eq(game->pause, 3);
  freeGameResources(game);
}
END_TEST

START_TEST(test_moving_terminate_action) {
  Game *game = getCurrentGameState();
  game->status = Moving;
  game->action = Terminate;
  game->newAction = true;

  handleMovingStateInput(game);
  ck_assert_int_eq(game->status, ExitState);
  freeGameResources(game);
}
END_TEST

START_TEST(test_moving_auto_shift) {
  Game *game = getCurrentGameState();
  initGame(game);
  game->status = Moving;
  game->newAction = false;
  game->level = 10;

  handleMovingStateInput(game);
  ck_assert_int_eq(game->status, 2);
  freeGameResources(game);
}
END_TEST

START_TEST(test_exit_normal) {
  Game *game = getCurrentGameState();
  initGame(game);

  exitFunc(game);

  ck_assert_ptr_null(game->field);
  ck_assert_ptr_null(game->next);
  ck_assert_ptr_null(game->figure);
  ck_assert_int_eq(game->status, ExitState);
  ck_assert_int_eq(game->pause, EXIT_STATE);
  freeGameResources(game);
}
END_TEST

START_TEST(test_exit_already_freed) {
  Game *game = getCurrentGameState();
  exitFunc(game);
  exitFunc(game);

  ck_assert_ptr_null(game->field);
  ck_assert_ptr_null(game->figure);
  ck_assert_int_eq(game->status, ExitState);
}
END_TEST

START_TEST(test_exit_null_pointer) {
  exitFunc(NULL);
  ck_assert(1);
}
END_TEST

START_TEST(test_exit_partial_alloc) {
  Game *game = getCurrentGameState();
  game->field = createMatrix(ROWS_MAP, COLS_MAP);
  game->next = NULL;
  game->figure = malloc(sizeof(Figure_t));

  exitFunc(game);

  ck_assert_ptr_null(game->field);
  ck_assert_ptr_null(game->figure);
}
END_TEST

START_TEST(test_exit_memalloc_flag) {
  Game *game = getCurrentGameState();
  initGame(game);
  game->memAlloc = true;

  exitFunc(game);
  ck_assert(!game->memAlloc);
  freeGameResources(game);
}
END_TEST

START_TEST(test_exit_newaction_reset) {
  Game *game = getCurrentGameState();
  game->newAction = true;

  exitFunc(game);
  ck_assert(!game->newAction);
}
END_TEST

START_TEST(test_exit_status_preserved) {
  Game *game = getCurrentGameState();
  game->status = GameOver;

  exitFunc(game);
  ck_assert_int_eq(game->status, ExitState);
}
END_TEST

START_TEST(test_exit_double_free) {
  Game *game = getCurrentGameState();
  initGame(game);

  freeMatrix(game->field);
  game->field = NULL;

  exitFunc(game);
  ck_assert_ptr_null(game->field);
  freeGameResources(game);
}
END_TEST

START_TEST(test_exit_score_reset) {
  Game *game = getCurrentGameState();
  game->score = 1000;

  exitFunc(game);
  ck_assert_int_eq(game->score, 1000);
}
END_TEST

START_TEST(test_exit_highscore_preserved) {
  Game *game = getCurrentGameState();
  game->highScore = 5000;

  exitFunc(game);
  ck_assert_int_eq(game->highScore, 5000);
}
END_TEST

START_TEST(test_userinput_pause_toggle) {
  Game *game = getCurrentGameState();
  game->status = Moving;
  game->pause = 0;

  userInput(Pause, false);
  ck_assert_int_eq(game->pause, 3);

  userInput(Pause, false);
  ck_assert_int_eq(game->pause, 0);
}
END_TEST

START_TEST(test_userinput_start_ignored) {
  Game *game = getCurrentGameState();
  game->status = Moving;
  game->action = Down;
  game->newAction = false;

  userInput(Start, false);
  ck_assert_int_eq(game->action, Start);
  ck_assert(game->newAction);
}
END_TEST

START_TEST(test_userinput_hold_flag) {
  Game *game = getCurrentGameState();
  game->newAction = false;

  userInput(Left, true);
  ck_assert_int_eq(game->action, Left);
  ck_assert(game->newAction);
}
END_TEST

START_TEST(test_userinput_multiple_actions) {
  Game *game = getCurrentGameState();
  game->newAction = true;
  game->action = Down;

  userInput(Right, false);
  ck_assert_int_eq(game->action, Down);
}
END_TEST

START_TEST(test_userinput_terminal_state) {
  Game *game = getCurrentGameState();
  game->status = ExitState;

  userInput(Action, false);
  ck_assert(game->newAction);
}
END_TEST

START_TEST(test_userinput_null_game) {
  Game *original = getCurrentGameState();
  *(Game **)getCurrentGameState() = NULL;

  userInput(Down, false);
  ck_assert(1);

  *(Game **)getCurrentGameState() = original;
}
END_TEST

START_TEST(test_userinput_pause_not_moving) {
  Game *game = getCurrentGameState();
  game->status = Spawn;
  game->pause = 0;

  userInput(Pause, false);
  ck_assert_int_eq(game->pause, 0);
}
END_TEST

START_TEST(test_userinput_invalid_action) {
  Game *game = getCurrentGameState();
  game->newAction = false;

  userInput(100, false);
  ck_assert(game->newAction);
}
END_TEST

START_TEST(test_userinput_gameover_state) {
  Game *game = getCurrentGameState();
  game->status = GameOver;
  game->newAction = false;

  userInput(Down, false);
  ck_assert(game->newAction);
}
END_TEST

START_TEST(test_userinput_initial_state) {
  Game *game = getCurrentGameState();
  game->status = Initial;
  game->newAction = false;

  userInput(Start, false);
  ck_assert(game->newAction);
  ck_assert_int_eq(game->action, Start);
}
END_TEST

START_TEST(test_update_normal) {
  Game *game = getCurrentGameState();
  initGame(game);
  game->status = Moving;

  GameInfo_t info = updateCurrentState();

  ck_assert_ptr_nonnull(info.field);
  ck_assert_ptr_nonnull(info.next);
  ck_assert_int_eq(info.pause, 0);
  freeGameResources(game);
}
END_TEST

START_TEST(test_update_exit_state) {
  Game *game = getCurrentGameState();
  game->status = ExitState;

  GameInfo_t info = updateCurrentState();
  ck_assert_int_eq(info.pause, EXIT_STATE);
}
END_TEST

START_TEST(test_update_initialization) {
  Game *game = getCurrentGameState();
  freeGameResources(game);

  GameInfo_t info = updateCurrentState();
  ck_assert_ptr_nonnull(info.field);
  ck_assert_ptr_nonnull(info.next);
}
END_TEST

START_TEST(test_update_gameover) {
  Game *game = getCurrentGameState();
  game->status = GameOver;

  GameInfo_t info = updateCurrentState();
  ck_assert_int_eq(info.pause, 1);
  freeGameResources(game);
}
END_TEST

START_TEST(test_update_pause_state) {
  Game *game = getCurrentGameState();
  game->status = Pauses;

  GameInfo_t info = updateCurrentState();
  ck_assert_int_eq(info.pause, 1);
  freeGameResources(game);
}
END_TEST

START_TEST(test_update_score_propagation) {
  Game *game = getCurrentGameState();
  game->score = 1500;
  game->highScore = 3000;

  GameInfo_t info = updateCurrentState();
  ck_assert_int_eq(info.score, 0);
  freeGameResources(game);
}
END_TEST

START_TEST(test_update_level_speed) {
  Game *game = getCurrentGameState();
  game->level = 5;
  game->speed = 200;

  GameInfo_t info = updateCurrentState();
  ck_assert_int_eq(info.level, 1);
  ck_assert_int_eq(info.speed, 200);
  freeGameResources(game);
}
END_TEST

START_TEST(test_update_null_fields) {
  Game *game = getCurrentGameState();
  freeMatrix(game->field);
  freeMatrix(game->next);
  game->field = NULL;
  game->next = NULL;

  GameInfo_t info = updateCurrentState();
  ck_assert_ptr_nonnull(info.field);
  ck_assert_ptr_nonnull(info.next);
  freeGameResources(game);
}
END_TEST

START_TEST(test_update_after_attach) {
  Game *game = getCurrentGameState();
  initGame(game);
  game->status = Attaching;

  GameInfo_t info = updateCurrentState();
  ck_assert_ptr_nonnull(info.field);
  ck_assert_int_ge(info.score, 0);
  freeGameResources(game);
}
END_TEST

START_TEST(test_fillinfo_normal) {
  Game *game = getCurrentGameState();
  initGame(game);

  GameInfo_t info = fillGameInfo(game);

  ck_assert_ptr_nonnull(info.field);
  ck_assert_ptr_nonnull(info.next);
  ck_assert_int_eq(info.pause, 1);
  freeGameResources(game);
}
END_TEST

START_TEST(test_fillinfo_exit_state) {
  Game *game = getCurrentGameState();
  game->status = ExitState;

  GameInfo_t info = fillGameInfo(game);
  ck_assert_int_eq(info.pause, EXIT_STATE);
  freeGameResources(game);
}
END_TEST

START_TEST(test_fillinfo_gameover) {
  Game *game = getCurrentGameState();
  game->status = GameOver;

  GameInfo_t info = fillGameInfo(game);
  ck_assert_int_eq(info.pause, 3);
  freeGameResources(game);
}
END_TEST

START_TEST(test_fillinfo_pause_state) {
  Game *game = getCurrentGameState();
  game->status = Pauses;

  GameInfo_t info = fillGameInfo(game);
  ck_assert_int_eq(info.pause, 2);
  freeGameResources(game);
}
END_TEST

START_TEST(test_fillinfo_figure_placement) {
  Game *game = getCurrentGameState();
  initGame(game);
  game->figure->x = 5;
  game->figure->y = 3;
  game->figure->brick[0][0] = 1;

  GameInfo_t info = fillGameInfo(game);
  ck_assert_int_eq(info.field[5][3], 1);
  freeGameResources(game);
}
END_TEST

START_TEST(test_fillinfo_next_propagation) {
  Game *game = getCurrentGameState();
  initGame(game);
  game->next[0][0] = 1;

  GameInfo_t info = fillGameInfo(game);
  ck_assert_int_eq(info.next[0][0], 1);
  freeGameResources(game);
}
END_TEST

START_TEST(test_fillinfo_score_propagation) {
  Game *game = getCurrentGameState();
  game->score = 2500;
  game->highScore = 5000;

  GameInfo_t info = fillGameInfo(game);
  ck_assert_int_eq(info.score, 2500);
  ck_assert_int_eq(info.highScore, 5000);
  freeGameResources(game);
}
END_TEST

START_TEST(test_fillinfo_level_speed) {
  Game *game = getCurrentGameState();
  game->level = 3;
  game->speed = 150;

  GameInfo_t info = fillGameInfo(game);
  ck_assert_int_eq(info.level, 3);
  ck_assert_int_eq(info.speed, 150);
  freeGameResources(game);
}
END_TEST

START_TEST(test_pause_state) {
  Game *game = getCurrentGameState();
  initGame(game);

  game->status = Moving;
  game->action = Pause;
  game->newAction = true;
  progressGameState(game);
  ck_assert_int_eq(game->status, Pauses);

  game->action = Terminate;
  game->newAction = true;
  pauseFunc(game);
  ck_assert_int_eq(game->status, ExitState);
  freeGameResources(game);
}

START_TEST(test_shifting_state) {
  Game *game = getCurrentGameState();
  initGame(game);
  game->status = Spawn;
  progressGameState(game);

  game->figure->x = ROWS_MAP - 2;
  game->status = Shifting;
  shiftingFunc(game);
  ck_assert_int_eq(game->status, Attaching);
  freeGameResources(game);
}

START_TEST(test_game_over) {
  Game *game = getCurrentGameState();
  initGame(game);

  for (int j = 0; j < COLS_MAP; j++) {
    game->field[0][j] = 1;
  }

  game->status = Attaching;
  attachingFunc(game);
  ck_assert_int_eq(game->status, 1);
  freeGameResources(game);
}

START_TEST(test_rotate_i_figure) {
  Figure_t figure;
  figure.currentType = 'i';
  memcpy(figure.brick,
         (int[4][4]){{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
         sizeof(figure.brick));

  rotateFigure(&figure);

  ck_assert_int_eq(figure.brick[1][0], 0);
  ck_assert_int_eq(figure.brick[1][1], 1);
}

START_TEST(test_invalid_rotate) {
  int a[4][4] = {0};
  int b[4][4] = {0};
  rotate(NULL, b, 4);
  rotate(a, NULL, 4);
  rotate(a, b, 0);
  rotate(a, b, 5);
}

START_TEST(test_horizontal_move) {
  Game *game = getCurrentGameState();
  initGame(game);
  game->status = Spawn;
  progressGameState(game);

  int old_y = game->figure->y;
  moveHorizontal(game, 1);
  ck_assert_int_eq(game->figure->y, old_y + 1);
  freeGameResources(game);
}

START_TEST(test_collision_null) {
  bool collision = checkCollision(NULL, 0, 0);
  ck_assert(collision);
}

START_TEST(test_rotate_no_collision) {
  Game *game = getCurrentGameState();
  initGame(game);
  game->status = Spawn;
  progressGameState(game);

  game->figure->x = 5;
  game->figure->y = 3;
  actionFunc(game);

  freeGameResources(game);
}

Suite *tetris_suite(void) {
  Suite *s;
  TCase *tc_core, *tc_gameover, *tc_level, *tc_score, *tc_gamescore, *tc_shift,
      *tc_rows, *tc_state, *tc_progress, *tc_initial, *tc_moving, *tc_exit,
      *tc_user, *tc_update, *tc_fill, *tc_additional;

  s = suite_create("Tetris");

  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_restart_null_pointer);
  tcase_add_test(tc_core, test_restart_partial_init);
  tcase_add_test(tc_core, test_create_valid_matrix);
  tcase_add_test(tc_core, test_create_zero_rows);
  tcase_add_test(tc_core, test_create_zero_cols);
  tcase_add_test(tc_core, test_create_large_matrix);
  tcase_add_test(tc_core, test_create_negative_size);
  tcase_add_test(tc_core, test_free_valid_matrix);
  tcase_add_test(tc_core, test_free_null_pointer);
  tcase_add_test(tc_core, test_free_already_freed);
  tcase_add_test(tc_core, test_create_figure_null_game);
  tcase_add_test(tc_core, test_create_figure_no_next);
  tcase_add_test(tc_core, test_generate_null_game);
  tcase_add_test(tc_core, test_rotate_o_piece);
  tcase_add_test(tc_core, test_rotate_t_piece);
  tcase_add_test(tc_core, test_rotate_null_figure);
  tcase_add_test(tc_core, test_rotate_unknown_type);
  tcase_add_test(tc_core, test_attach_figure_normal);
  tcase_add_test(tc_core, test_attach_figure_completely_out_of_bounds);
  tcase_add_test(tc_core, test_attach_figure_null_game);
  tcase_add_test(tc_core, test_attach_figure_no_field);
  tcase_add_test(tc_core, test_attach_figure_no_figure);
  tcase_add_test(tc_core, test_prepare_field_normal);
  tcase_add_test(tc_core, test_prepare_field_no_figure);

  tcase_add_test(tc_core, test_prepare_field_no_field_in_game);
  tcase_add_test(tc_core, test_rotate_3x3_normal);
  tcase_add_test(tc_core, test_rotate_size_zero);
  tcase_add_test(tc_core, test_rotate_negative_size);
  tcase_add_test(tc_core, test_rotate_null_input);
  tcase_add_test(tc_core, test_rotate_invalid_size);
  tcase_add_test(tc_core, test_rotate_empty_figure);
  tcase_add_test(tc_core, test_move_brick_normal);
  tcase_add_test(tc_core, test_move_brick_no_figure);
  tcase_add_test(tc_core, test_move_brick_null_game);
  tcase_add_test(tc_core, test_move_brick_multiple_times);

  tcase_add_test(tc_core, test_move_horizontal_collision_right);
  tcase_add_test(tc_core, test_move_horizontal_collision_left);
  tcase_add_test(tc_core, test_move_horizontal_null_game);
  tcase_add_test(tc_core, test_move_horizontal_no_figure);
  tcase_add_test(tc_core, test_move_horizontal_zero_direction);
  tcase_add_test(tc_core, test_check_collision_normal_no_collision);
  tcase_add_test(tc_core, test_check_collision_wall_left);
  tcase_add_test(tc_core, test_check_collision_wall_right);
  tcase_add_test(tc_core, test_check_collision_wall_bottom);
  tcase_add_test(tc_core, test_check_collision_block_collision);
  tcase_add_test(tc_core, test_check_collision_null_game);
  tcase_add_test(tc_core, test_check_collision_no_field);

  tcase_add_test(tc_core, test_can_move_down_no_collision);
  tcase_add_test(tc_core, test_can_move_down_block_below);
  tcase_add_test(tc_core, test_can_move_down_null_game);
  tcase_add_test(tc_core, test_can_move_down_no_figure);
  tcase_add_test(tc_core, test_action_func_no_rotation_possible);
  tcase_add_test(tc_core, test_action_func_null_game);
  tcase_add_test(tc_core, test_action_func_no_figure);
  tcase_add_test(tc_core, test_initGame_null);
  tcase_add_test(tc_core, test_initGame_normal);
  tcase_add_test(tc_core, test_initGame_memory_fail);
  tcase_add_test(tc_core, test_initGame_after_reset);
  tcase_add_test(tc_core, test_freeGameResources_null);
  tcase_add_test(tc_core, test_freeGameResources_normal);
  tcase_add_test(tc_core, test_freeGameResources_partial);
  tcase_add_test(tc_core, test_freeGameResources_double_free);
  tcase_add_test(tc_core, test_createFigure_null);
  tcase_add_test(tc_core, test_createFigure_normal);
  tcase_add_test(tc_core, test_createFigure_no_next);
  tcase_add_test(tc_core, test_createFigure_no_memory);
  tcase_add_test(tc_core, test_createFigure_existing_figure);
  tcase_add_test(tc_core, test_generateNextFigure_null);
  tcase_add_test(tc_core, test_generateNextFigure_normal);
  tcase_add_test(tc_core, test_generateNextFigure_no_memory);
  tcase_add_test(tc_core, test_generateNextFigure_after_reset);
  tcase_add_test(tc_core, test_generateNextFigure_all_types);

  tc_gameover = tcase_create("GameOver");
  tcase_add_test(tc_gameover, test_game_over_top_row_filled);
  tcase_add_test(tc_gameover, test_game_over_single_cell_filled);
  tcase_add_test(tc_gameover, test_game_over_not_lost);

  tc_level = tcase_create("Level");
  tcase_add_test(tc_level, test_level_up_normal);
  tcase_add_test(tc_level, test_level_up_edge);
  tcase_add_test(tc_level, test_level_up_exact_threshold);
  tcase_add_test(tc_level, test_level_max_reached);
  tcase_add_test(tc_level, test_multiple_level_ups);
  tcase_add_test(tc_level, test_level_up_multiple_thresholds);

  tc_score = tcase_create("Score");
  tcase_add_test(tc_score, test_save_and_load_score);
  tcase_add_test(tc_score, test_save_zero_score);
  tcase_add_test(tc_score, test_save_large_score);
  tcase_add_test(tc_score, test_no_score_file);

  tc_gamescore = tcase_create("GameScoring");
  tcase_add_test(tc_gamescore, test_score_single_line);
  tcase_add_test(tc_gamescore, test_score_four_lines);
  tcase_add_test(tc_gamescore, test_score_mixed_lines);
  tcase_add_test(tc_gamescore, test_high_score_update);
  tcase_add_test(tc_gamescore, test_score_level_up_trigger);
  tcase_add_test(tc_gamescore, test_score_multiple_clears);

  tc_shift = tcase_create("Shift");
  tcase_add_test(tc_shift, test_shift_immediate);
  tcase_add_test(tc_shift, test_shift_paused_game);
  tcase_add_test(tc_shift, test_shift_exit_state);

  tc_rows = tcase_create("Rows");
  tcase_add_test(tc_rows, test_clear_no_lines);
  tcase_add_test(tc_rows, test_clear_single_line);
  tcase_add_test(tc_rows, test_clear_consecutive_lines);
  tcase_add_test(tc_rows, test_clear_non_consecutive);
  tcase_add_test(tc_rows, test_clear_all_lines);
  tcase_add_test(tc_rows, test_clear_partial_line);
  tcase_add_test(tc_rows, test_clear_with_gap);
  tcase_add_test(tc_rows, test_clear_top_line);

  tc_state = tcase_create("State");
  tcase_add_test(tc_state, test_gameOverFunc_null);
  tcase_add_test(tc_state, test_gameOverFunc_normal);
  tcase_add_test(tc_state, test_gameOverFunc_from_different_state);
  tcase_add_test(tc_state, test_gameOverFunc_with_active_timer);
  tcase_add_test(tc_state, test_gameOverFunc_after_gameplay);
  tcase_add_test(tc_state, test_attachingFunc_null);
  tcase_add_test(tc_state, test_attachingFunc_normal);
  tcase_add_test(tc_state, test_attachingFunc_causes_gameover);
  tcase_add_test(tc_state, test_attachingFunc_score_calculation);
  tcase_add_test(tc_state, test_attachingFunc_level_up);
  tcase_add_test(tc_state, test_shiftingFunc_null);

  tcase_add_test(tc_state, test_pauseFunc_null);
  tcase_add_test(tc_state, test_pauseFunc_resume);
  tcase_add_test(tc_state, test_pauseFunc_ignore_other_actions);
  tcase_add_test(tc_state, test_pauseFunc_no_action);

  tc_progress = tcase_create("Progress");
  tcase_add_test(tc_progress, test_progress_initial_to_spawn);
  tcase_add_test(tc_progress, test_progress_spawn_to_moving);
  tcase_add_test(tc_progress, test_progress_moving_to_shifting_by_time);
  tcase_add_test(tc_progress, test_progress_moving_to_pause);

  tcase_add_test(tc_progress, test_progress_attaching_to_gameover);
  tcase_add_test(tc_progress, test_progress_gameover_to_initial);
  tcase_add_test(tc_progress, test_progress_exit_state);
  tcase_add_test(tc_progress, test_progress_null_pointer);
  tcase_add_test(tc_progress, test_progress_invalid_state);

  tc_initial = tcase_create("InitialInput");
  tcase_add_test(tc_initial, test_initial_start_action);
  tcase_add_test(tc_initial, test_initial_terminate_action);
  tcase_add_test(tc_initial, test_initial_restart_after_gameover);
  tcase_add_test(tc_initial, test_initial_invalid_action);
  tcase_add_test(tc_initial, test_initial_no_action);
  tcase_add_test(tc_initial, test_initial_null_pointer);
  tcase_add_test(tc_initial, test_initial_memory_failure);
  tcase_add_test(tc_initial, test_initial_high_score_persistence);
  tcase_add_test(tc_initial, test_initial_double_start);
  tcase_add_test(tc_initial, test_initial_terminate_after_start);

  tc_moving = tcase_create("MovingInput");
  tcase_add_test(tc_moving, test_moving_down_action);

  tcase_add_test(tc_moving, test_moving_rotate_action);
  tcase_add_test(tc_moving, test_moving_pause_action);
  tcase_add_test(tc_moving, test_moving_terminate_action);
  tcase_add_test(tc_moving, test_moving_auto_shift);

  tc_exit = tcase_create("Exit");
  tcase_add_test(tc_exit, test_exit_normal);
  tcase_add_test(tc_exit, test_exit_already_freed);
  tcase_add_test(tc_exit, test_exit_null_pointer);
  tcase_add_test(tc_exit, test_exit_partial_alloc);
  tcase_add_test(tc_exit, test_exit_memalloc_flag);
  tcase_add_test(tc_exit, test_exit_newaction_reset);
  tcase_add_test(tc_exit, test_exit_status_preserved);
  tcase_add_test(tc_exit, test_exit_double_free);
  tcase_add_test(tc_exit, test_exit_score_reset);
  tcase_add_test(tc_exit, test_exit_highscore_preserved);

  tc_user = tcase_create("UserInput");
  tcase_add_test(tc_user, test_userinput_pause_toggle);
  tcase_add_test(tc_user, test_userinput_start_ignored);
  tcase_add_test(tc_user, test_userinput_hold_flag);
  tcase_add_test(tc_user, test_userinput_multiple_actions);
  tcase_add_test(tc_user, test_userinput_terminal_state);
  tcase_add_test(tc_user, test_userinput_null_game);
  tcase_add_test(tc_user, test_userinput_pause_not_moving);
  tcase_add_test(tc_user, test_userinput_invalid_action);
  tcase_add_test(tc_user, test_userinput_gameover_state);
  tcase_add_test(tc_user, test_userinput_initial_state);

  tc_update = tcase_create("Update");
  tcase_add_test(tc_update, test_update_normal);
  tcase_add_test(tc_update, test_update_exit_state);
  tcase_add_test(tc_update, test_update_initialization);
  tcase_add_test(tc_update, test_update_gameover);
  tcase_add_test(tc_update, test_update_pause_state);
  tcase_add_test(tc_update, test_update_score_propagation);
  tcase_add_test(tc_update, test_update_level_speed);
  tcase_add_test(tc_update, test_update_null_fields);
  tcase_add_test(tc_update, test_update_after_attach);

  tc_fill = tcase_create("FillInfo");
  tcase_add_test(tc_fill, test_fillinfo_normal);
  tcase_add_test(tc_fill, test_fillinfo_exit_state);
  tcase_add_test(tc_fill, test_fillinfo_gameover);
  tcase_add_test(tc_fill, test_fillinfo_pause_state);
  tcase_add_test(tc_fill, test_fillinfo_figure_placement);
  tcase_add_test(tc_fill, test_fillinfo_next_propagation);
  tcase_add_test(tc_fill, test_fillinfo_score_propagation);
  tcase_add_test(tc_fill, test_fillinfo_level_speed);

  tc_additional = tcase_create("Additional");
  tcase_add_test(tc_additional, test_pause_state);
  tcase_add_test(tc_additional, test_shifting_state);
  tcase_add_test(tc_additional, test_game_over);
  tcase_add_test(tc_additional, test_rotate_i_figure);
  tcase_add_test(tc_additional, test_invalid_rotate);
  tcase_add_test(tc_additional, test_horizontal_move);
  tcase_add_test(tc_additional, test_collision_null);
  tcase_add_test(tc_additional, test_rotate_no_collision);

  suite_add_tcase(s, tc_core);
  suite_add_tcase(s, tc_gameover);
  suite_add_tcase(s, tc_level);
  suite_add_tcase(s, tc_score);
  suite_add_tcase(s, tc_gamescore);
  suite_add_tcase(s, tc_shift);
  suite_add_tcase(s, tc_rows);
  suite_add_tcase(s, tc_state);
  suite_add_tcase(s, tc_progress);
  suite_add_tcase(s, tc_initial);
  suite_add_tcase(s, tc_moving);
  suite_add_tcase(s, tc_exit);
  suite_add_tcase(s, tc_user);
  suite_add_tcase(s, tc_update);
  suite_add_tcase(s, tc_fill);
  suite_add_tcase(s, tc_additional);

  return s;
}
int main(void) {
  int number_failed;
  Suite *s;
  SRunner *sr;

  s = tetris_suite();
  sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}