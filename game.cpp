#include <iostream>
#include <cstdlib>
#include <vector>
#include <ncurses.h>
#include <unistd.h>

#include <climits>    /* for CHAR_BIT */
#include <stdint.h>   /* for uint32_t */

using namespace std;

#define DELAY 3

class Pair {
  public: int x; int y;
};

class Tile {
  public: bool isDark;
  public: bool isWall;
};

class Map {
  private:
    int m;
    int n;
    vector<Tile> map;
  public:
    Map(int m, int n): m(m), n(n), map(m*n) {
    } 
    void generate() {
      clear();
      Pair cursor = { 0, 0 };
      for (; cursor.x <= m; cursor.x++) {
        for (; cursor.y <= n; cursor.y++) {
          Tile t = Tile{ true, false };
          if (rand() % 100 < 15) { // 15 % walls
            t = Tile{ true, true };
          }
          set(cursor.x, cursor.y, t);
          refresh();
          usleep(DELAY);
        }
        cursor.y = 0;
      }
    }
    void set(int x, int y, Tile t) {
      map[y * m + x] = t;
    }
    Tile get(int x, int y) {
      return map[y * m + x];
    }
};

class State {
  public: 
    Pair cursor;
    Pair max;
    Map map;
    WINDOW * pad;
};

void info() {
  printw("game works like this");
  refresh();
  getch();
  clear();
  printw("and that");
  refresh();
  getch();
}

void init() {
  initscr();
  start_color();
  noecho();
  curs_set(FALSE);
  keypad(stdscr, TRUE);

  if(has_colors() == FALSE) {
    endwin();
    printf("Your terminal does not support color\n");
    exit(1);
  }
  start_color();
  init_pair(1, COLOR_GREEN, COLOR_BLACK);

  scrollok(stdscr, TRUE);
}

Pair nextPos(State *s, int c) {
  Pair next = {s->cursor.x, s->cursor.y};

  switch(c) {
    case KEY_UP:
        next.x--;
        break;
    case KEY_DOWN:
        next.x++;
        break;
    case KEY_LEFT:
        next.y--;
        break;
    case KEY_RIGHT:
        next.y++;
        break;
    default:
        break;
  }
  return next;
}

bool inBounds(Pair cursor, Pair max) {
  return (0 <= cursor.x && cursor.x <= max.x
      &&  0 <= cursor.y && cursor.y <= max.y);
}

void maze(State *s) {
  clear();
  Pair cursor = { 0, 0 };
  for (; cursor.x <= s->max.x; cursor.x++) {
    for (; cursor.y <= s->max.y; cursor.y++) {
      if (s->map.get(cursor.x, cursor.y).isWall) { // 15 % walls
        attron(COLOR_PAIR(1));
        mvprintw(cursor.x, cursor.y, "W");
        attroff(COLOR_PAIR(1));
      }
      refresh();
    }
    cursor.y = 0;
  }
}

void step(State *s, int c) {
  Pair next = nextPos(s, c);
  if (!inBounds(next, s->max)) {
    int n = 0;
    switch(c) {
      case KEY_UP:
          n = -1;
          break;
      case KEY_DOWN:
          n = 1;
          break;
      case KEY_LEFT:
          break;
      case KEY_RIGHT:
          break;
      default:
          break;
    }
    scrl(n); 
    refresh();
    //maze(s); TODO maze does need offset
  }
  s->cursor = next;
  mvprintw(s->cursor.x, s->cursor.y, "o");
  refresh();
}


//int main(int argc, const char * argv[]) {
//  init();
//  State s = { {0, 0}, {LINES-1, COLS-1}, Map(LINES, COLS), newpad(LINES * 4, COLS * 4) };
//
//  mvprintw(pad, s.cursor.x, s.cursor.y, "@");
//  getch();
//  while(1) {
//    int c = getch();
//    step(&s, c);
//  }
//  endwin();
//  return 0;
//}

// fill pad with random grass
int fill(WINDOW * map, int lines, int cols) {
  char c = 'a';
  for (int x = 0; x < lines; x++) {
    for (int y = 0; y < cols; y++) {
      if (rand() % 100 < 15) // 15 % grass
        c = 'w';
      else 
        c = ' ';
      mvwaddch(map, x, y, c);
    }
  }
}

int main() {
  init();
  int map_lines = LINES + 50;
  int map_cols = COLS + 50;
  State s = { {0, 0}, {LINES-1, COLS-1}, Map(LINES, COLS), newpad(map_lines, map_cols) };

  fill(s.pad, map_lines, map_cols);

  // fill part of main window with section of pad
  for (int x = 0; x < 20; x++) {
    prefresh(s.pad, 0, x, 0, 0, LINES-1, COLS-1);
    wgetch(s.pad);
  }
//  while(1) {
//    int c = getch();
//    step(&s, c);
//  }

  delwin(s.pad);

  endwin();
  exit(0);
}
