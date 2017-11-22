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
}

bool inBounds(State *s, int c) {
  Pair future = {s->cursor.x, s->cursor.y};

  switch(c) {
    case KEY_UP:
        future.x--;
        break;
    case KEY_DOWN:
        future.x++;
        break;
    case KEY_LEFT:
        future.y--;
        break;
    case KEY_RIGHT:
        future.y++;
        break;
    default:
        break;
  }
  bool in_bounds = (0 <= future.x && future.x <= s->max.x
                &&  0 <= future.y && future.y <= s->max.y);
  return in_bounds; // future not out of bounds && future no wall
}

void step(State *s, int c) {
  if (!inBounds(s, c)) {
    // TODO moveMap(int c);
    
    return;
  }
  //clear();
  char * x;
  switch(c) {
    case KEY_UP:
        s->cursor.x--;
        x = "u";
        break;
    case KEY_DOWN:
        s->cursor.x++;
        x = "d";
        break;
    case KEY_LEFT:
        s->cursor.y--;
        x = "l";
        break;
    case KEY_RIGHT:
        s->cursor.y++;
        x = "r";
        break;
    default:
        x = "o";
        break;
  }
  mvprintw(s->cursor.x, s->cursor.y, x);
  refresh();
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

int main(int argc, const char * argv[]) {
  init();
  int max_x, max_y;
  State s = { {0, 0}, {LINES-1, COLS-1}, Map(LINES, COLS) };

//  info();
  s.map.generate();
  maze(&s);

  // step 0
//  clear();
  mvprintw(s.cursor.x, s.cursor.y, "@");
  getch();
////
  while(1) {
//    maze(&s);
    int c = getch();
    step(&s, c);
  }
  endwin();
  return 0;
}
