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
    vector<Tile> map;
  public:
    WINDOW * pad;
    int m; // lines
    int n; // cols

    Map(int m, int n): m(m), n(n), map(m*n), pad(newpad(m,n)) {
      fillPad();
    } 

    void fillMap() {
      Pair cursor = { 0, 0 };
      for (; cursor.x <= m; cursor.x++) {
        for (; cursor.y <= n; cursor.y++) {
          Tile t = Tile{ true, false };
          if (rand() % 100 < 15) // 15 % walls - TODO base on grass?
            t = Tile{ true, true };
          set(cursor.x, cursor.y, t);
          refresh();
          usleep(DELAY);
        }
        cursor.y = 0;
      }
    }

    // fillPad pad with random grass
    int fillPad() {
      char c = ' ';
      for (int x = 0; x < m; x++) {
        for (int y = 0; y < n; y++) {
          if (rand() % 100 < 15) // 15 % grass
            c = 'w';
          else 
            c = ' ';
          wattron(pad, COLOR_PAIR(2));
          mvwaddch(pad, x, y, c);
          wattroff(pad, COLOR_PAIR(2));
        }
      }
      prefresh(pad, 0, 0, 0, 0, LINES-1, COLS-1);
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
    Map map;
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
  init_pair(1, COLOR_RED,     COLOR_BLACK);
  init_pair(2, COLOR_GREEN,   COLOR_BLACK);
  init_pair(3, COLOR_YELLOW,  COLOR_BLACK);
  init_pair(4, COLOR_BLUE,    COLOR_BLACK);
  init_pair(5, COLOR_CYAN,    COLOR_BLACK);
  init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(7, COLOR_WHITE,   COLOR_BLACK);
  scrollok(stdscr, TRUE);
}

Pair nextPos(State *s, int c) {
  Pair next = { s->cursor.x, s->cursor.y };

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

bool inScrollBounds(Pair cursor, Pair max) {
  return (0 <= cursor.x && cursor.x <= max.x
      &&  0 <= cursor.y && cursor.y <= max.y);
}

void step(State *s, int c) {
  Pair next = nextPos(s, c);

  // TODO more readable, model as dims pair
  int map_lines = s->map.m;
  int map_cols = s->map.n;
  // only move the map if we are not at the edge
  if (inScrollBounds(next, {map_cols - COLS, map_lines - LINES})) {
    s->cursor = next;
    prefresh(s->map.pad, s->cursor.x, s->cursor.y, 0, 0, LINES-1, COLS-1);
  }
}

int main() {
  init();
  State s = { .cursor = {0, 0}, 
              .map = Map(LINES + 50, COLS + 50) };

  // TODO why do I have to enter two chars?
  int c = wgetch(s.map.pad);
  while(1) {
    step(&s, c);
    // TODO these +1's are wrong + we wanna be not in the corner
    mvwaddch(s.map.pad, s.cursor.x+1, s.cursor.y+1, '@');
    c = getch();
  }

  delwin(s.map.pad);
  endwin();
  exit(0);
}
