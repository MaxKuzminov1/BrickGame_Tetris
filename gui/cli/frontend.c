#include "frontend.h"

static void drawStartScreen(void);
static void drawGameOverScreen(const GameInfo_t* info);
static void drawGameField(int** field);
static void drawStatsPanel(const GameInfo_t* info);
static void drawNextPiece(int y, int x, int** next);
static void drawWindowBorders(int y, int x, int h, int w, const char* title);
static void drawPausePopup(void);

void initColors() {
  start_color();
  init_pair(COLOR_DEFAULT, COLOR_WHITE, COLOR_BLACK);
  init_pair(COLOR_BLOCK, COLOR_BLACK, COLOR_CYAN);
  init_pair(COLOR_TEXT, COLOR_YELLOW, COLOR_BLACK);
  init_pair(COLOR_HIGHLIGHT, COLOR_GREEN, COLOR_BLACK);
  init_pair(COLOR_DIM, COLOR_BLACK, COLOR_BLACK);
  init_pair(COLOR_POPUP_BG, COLOR_WHITE, COLOR_BLUE);
  init_pair(COLOR_POPUP_FRAME, COLOR_YELLOW, COLOR_BLUE);
}

void initNcurses() {
  initscr();
  start_color();
  curs_set(0);
  cbreak();
  halfdelay(1);
  keypad(stdscr, TRUE);
}

void drawWindowBorders(int y, int x, int h, int w, const char* title) {
  attron(COLOR_PAIR(COLOR_DEFAULT));
  mvaddch(y, x, '+');
  mvaddch(y, x + w - 1, '+');
  mvaddch(y + h - 1, x, '+');
  mvaddch(y + h - 1, x + w - 1, '+');

  for (int i = x + 1; i < x + w - 1; i++) {
    mvaddch(y, i, '-');
    mvaddch(y + h - 1, i, '-');
  }

  for (int i = y + 1; i < y + h - 1; i++) {
    mvaddch(i, x, '|');
    mvaddch(i, x + w - 1, '|');
  }

  if (title) {
    mvprintw(y, x + 2, "[ %s ]", title);
  }
  attroff(COLOR_PAIR(COLOR_DEFAULT));
}

void drawPausePopup() {
  int popup_h = 7;
  int popup_w = 30;
  int popup_y = (LINES - popup_h) / 2;
  int popup_x = (COLS - popup_w) / 2;

  attron(COLOR_PAIR(COLOR_DIM) | A_DIM);
  for (int i = 0; i < LINES; i++) {
    move(i, 0);
    for (int j = 0; j < COLS; j++) {
      addch(' ');
    }
  }
  attroff(COLOR_PAIR(COLOR_DIM) | A_DIM);

  drawWindowBorders(popup_y, popup_x, popup_h, popup_w, NULL);

  attron(COLOR_PAIR(COLOR_HIGHLIGHT) | A_BOLD);
  mvprintw(popup_y, popup_x + (popup_w - 2) / 2, " || ");
  attroff(COLOR_PAIR(COLOR_HIGHLIGHT) | A_BOLD);

  attron(COLOR_PAIR(COLOR_TEXT) | A_BOLD);
  mvprintw(popup_y + 2, popup_x + (popup_w - 8) / 2, " PAUSED ");
  attroff(COLOR_PAIR(COLOR_TEXT) | A_BOLD);

  attron(COLOR_PAIR(COLOR_TEXT));
  mvprintw(popup_y + 4, popup_x + (popup_w - 20) / 2, "Press 'P' to continue");
  attroff(COLOR_PAIR(COLOR_TEXT));
}

static void drawStartScreen(void) {
  drawWindowBorders(5, 10, 18, 40, "T E T R I S");
  attron(COLOR_PAIR(COLOR_TEXT));
  mvprintw(7, 15, "Controls:");
  mvprintw(9, 15, "Left Arrow  - Move Left");
  mvprintw(10, 15, "Right Arrow - Move Right");
  mvprintw(11, 15, "Down Arrow  - Move Down");
  mvprintw(12, 15, "Up Arrow    - Rotate");
  mvprintw(13, 15, "Space       - Hard Drop");
  mvprintw(14, 15, "P           - Pause Game");
  mvprintw(15, 15, "ESC         - Quit");
  attroff(COLOR_PAIR(COLOR_TEXT));

  attron(COLOR_PAIR(COLOR_HIGHLIGHT));
  mvprintw(17, 15, "Press 'S' to Start Game");
  attroff(COLOR_PAIR(COLOR_HIGHLIGHT));
}

static void drawGameOverScreen(const GameInfo_t* info) {
  drawWindowBorders(5, 10, 10, 40, "GAME OVER");
  attron(COLOR_PAIR(COLOR_TEXT));
  mvprintw(7, 15, "Final Score: %d", info->score);
  mvprintw(8, 15, "High Score: %d", info->highScore);
  attroff(COLOR_PAIR(COLOR_TEXT));

  attron(COLOR_PAIR(COLOR_HIGHLIGHT));
  mvprintw(9, 15, "Press 'S' to Restart");
  attroff(COLOR_PAIR(COLOR_HIGHLIGHT));
}

static void drawGameField(int** field) {
  for (int i = 0; i < FIELD_H; i++) {
    for (int j = 0; j < FIELD_W; j++) {
      if (field[i][j]) {
        attron(COLOR_PAIR(COLOR_BLOCK));
        mvprintw(i + 2, j * 2 + 2, "  ");
        attroff(COLOR_PAIR(COLOR_BLOCK));
      } else {
        mvprintw(i + 2, j * 2 + 2, "  ");
      }
    }
  }
}

static void drawNextPiece(int y, int x, int** next) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (next[i][j]) {
        attron(COLOR_PAIR(COLOR_BLOCK));
        mvprintw(y + i, x + j * 2, "  ");
        attroff(COLOR_PAIR(COLOR_BLOCK));
      }
    }
  }
}

static void drawStatsPanel(const GameInfo_t* info) {
  const int panel_x = FIELD_W * 2 + 5;

  attron(COLOR_PAIR(COLOR_TEXT));
  mvprintw(3, panel_x + 8, "Score: %08d", info->score);
  mvprintw(5, panel_x + 8, "High:  %08d", info->highScore);
  mvprintw(7, panel_x + 8, "Level: %02d", info->level);
  mvprintw(9, panel_x + 8, "Next Piece:");
  attroff(COLOR_PAIR(COLOR_TEXT));

  drawNextPiece(11, panel_x + 12, info->next);
}

void frontendGeneralFunc(GameInfo_t info) {
  static bool colorsInitialized = false;
  if (!colorsInitialized) {
    initColors();
    colorsInitialized = true;
  }

  erase();

  if (info.pause != EXIT_STATE) {
    switch (info.pause) {
      case 1:
        drawStartScreen();
        break;
      case 2:
        drawGameOverScreen(&info);
        break;
      default:
        drawWindowBorders(1, 1, FIELD_H + 2, FIELD_W * 2 + 2, "GAME FIELD");
        if (info.field) drawGameField(info.field);
        drawWindowBorders(1, FIELD_W * 2 + 5, 16, 24, "STATS");
        drawStatsPanel(&info);
        if (info.pause == 3) drawPausePopup();
    }
  }

  refresh();
}