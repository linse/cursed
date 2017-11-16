#include <iostream>
#include <ncurses.h>
#include <unistd.h>
using namespace std;

#define DELAY 30000
//usleep(DELAY);

class Pair {
  public: int x; int y;
};

class State {
  public: 
    Pair cursor;
    Pair max;
};

void info() {
  initscr();
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
 noecho();
 curs_set(FALSE);
 keypad(stdscr, TRUE);
}

void step(State *s, int c) {
  clear();
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

int main(int argc, const char * argv[]) {
  State s = { {0, 0}, {0, 0}};
  // stdscr created by `initscr()`
  getmaxyx(stdscr, s.max.y, s.max.x);

  info();
  init();
  while(1) {
    int c = getch();
    step(&s, c);
  }
  endwin();
  return 0;
}
