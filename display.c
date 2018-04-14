//
// Michał Piotr Stankiewicz
// ms335789
// Zadanie zaliczeniowe ZSO 2017 / 2018
// Zadanie numer 1
//

#include <sys/types.h>
#include <ncurses.h>
#include <errno.h>
#include <unistd.h>
#include "alienos.h"
#include "display.h"
#include "debug.h"

#ifndef BG_COLOR
#define BG_COLOR COLOR_BLACK
#endif

#ifndef HAS_COLOR
#define HAS_COLOR 1
#endif

static bool colors_set;

static int my_color(int n) {
    switch (n) {
        case 8:
            return COLOR_PAIR(1) | A_BOLD;
        case 9:
            return COLOR_PAIR(1) | A_DIM;
        case 10:
            return COLOR_PAIR(2) | A_BOLD;
        case 11:
            return COLOR_PAIR(3) | A_BOLD;
        case 12:
            return COLOR_PAIR(4) | A_BOLD;
        case 13:
            return COLOR_PAIR(5) | A_BOLD;
        case 14:
            return COLOR_PAIR(6) | A_BOLD;
        case 15:
            return COLOR_PAIR(7) | A_BOLD;
        default:
            return COLOR_PAIR(n);
    }
}

static void make_color() {
    start_color();
    if (has_colors() && (COLOR_PAIRS >= 16) && HAS_COLOR) {
        colors_set = true;

        init_pair(1, COLOR_BLACK, BG_COLOR);
        init_pair(2, COLOR_BLUE, BG_COLOR);
        init_pair(3, COLOR_GREEN, BG_COLOR);
        init_pair(4, COLOR_CYAN, BG_COLOR);
        init_pair(5, COLOR_RED, BG_COLOR);
        init_pair(6, COLOR_MAGENTA, BG_COLOR);
        init_pair(7, COLOR_YELLOW, BG_COLOR);
        init_pair(16, COLOR_WHITE, BG_COLOR);
    } else {
        colors_set = false;
    }
}

int display_init() {
    initscr();
    make_color();
    noecho();
    keypad(stdscr, TRUE);
    clear();
    refresh();
    return 0;
}

void display_close() {
    clear();
    refresh();
    endwin();
}

struct my_char {
    char ch;
    uint8_t color_and_nothing;
};

int display_show(int x, int y, uint16_t *text, int len) {
    int old_x;
    int old_y;

    getyx((WINDOW *) stdscr, old_y, old_x);
    for (int i = 0; i < len; i++) {
        struct my_char *c = (struct my_char *) text;
        c->color_and_nothing %= 0x10;
        if (colors_set)
            attron(my_color(c->color_and_nothing + 1));
        mvaddch(y, x + i, c->ch);
        if (colors_set)
            attroff(my_color(c->color_and_nothing + 1));
        text += 1;
    }
    move(old_y, old_x);
    refresh();
    return 0;
}

int display_move_cursor(int x, int y) {
    int r = move(y, x);
    refresh();
    return r;
}

int display_read_char(int *ch) {
    int c = getch();
    switch (c) {
        case KEY_UP:
            *ch = ALIENOS_KEY_UP;
            break;
        case KEY_DOWN:
            *ch = ALIENOS_KEY_DOWN;
            break;
        case KEY_RIGHT:
            *ch = ALIENOS_KEY_RIGHT;
            break;
        case KEY_LEFT:
            *ch = ALIENOS_KEY_LEFT;
            break;
        case KEY_ENTER:
        case '\n':
            *ch = 0x0a;
            break;
        default:
            if ((c >= 0x20) && (c <= 0x7e)) {
                *ch = c;
                break;
            }
            goto read_char_fail;
    }
    return 0;
read_char_fail:
    errno = EPERM;
    return -1;
}