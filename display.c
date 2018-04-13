//
// Created by micha on 12.04.18.
//

#include <sys/types.h>
#include <ncurses.h>
#include <errno.h>
#include <unistd.h>
#include "alienos.h"
#include "display.h"
#include "debug.h"

#define COLOR_MAKE(x) ((x) * 1000 / 255)

static bool colors_set;

static void make_color() {
    start_color();
    if (has_colors() && COLOR_PAIRS >= 16) {
        const short turquoise = 17;
        const short pink = 18;
        const short lightgray = 19;
        const short darkgray = 20;
        const short lightblue = 21;
        const short lightgreen = 22;
        const short lightturquoise = 23;
        const short lightred = 24;
        const short lightpink = 25;
        const short lightyellow = 26;
        colors_set = true;

        init_color(turquoise, COLOR_MAKE(64), COLOR_MAKE(224), COLOR_MAKE(208));
        init_color(pink, COLOR_MAKE(255), COLOR_MAKE(192), COLOR_MAKE(203));
        init_color(lightgray, COLOR_MAKE(211), COLOR_MAKE(211), COLOR_MAKE(211));
        init_color(darkgray, COLOR_MAKE(169), COLOR_MAKE(169), COLOR_MAKE(169));
        init_color(lightblue, COLOR_MAKE(173), COLOR_MAKE(216), COLOR_MAKE(230));
        init_color(lightgreen, COLOR_MAKE(144), COLOR_MAKE(238), COLOR_MAKE(144));
        init_color(lightturquoise, COLOR_MAKE(175), COLOR_MAKE(238), COLOR_MAKE(238));
        init_color(lightred, COLOR_MAKE(255), COLOR_MAKE(160), COLOR_MAKE(122));
        init_color(lightpink, COLOR_MAKE(255), COLOR_MAKE(182), COLOR_MAKE(203));
        init_color(lightyellow, COLOR_MAKE(255), COLOR_MAKE(255), COLOR_MAKE(224));

        init_pair(1, COLOR_BLACK, COLOR_WHITE);
        init_pair(2, COLOR_BLUE, COLOR_WHITE);
        init_pair(3, COLOR_GREEN, COLOR_WHITE);
        init_pair(4, turquoise, COLOR_WHITE);
        init_pair(5, COLOR_RED, COLOR_WHITE);
        init_pair(6, pink, COLOR_WHITE);
        init_pair(7, COLOR_YELLOW, COLOR_WHITE);
        init_pair(8, lightgray, COLOR_WHITE);
        init_pair(9, darkgray, COLOR_WHITE);
        init_pair(10, lightblue, COLOR_WHITE);
        init_pair(11, lightgreen, COLOR_WHITE);
        init_pair(12, lightturquoise, COLOR_WHITE);
        init_pair(13, lightred, COLOR_WHITE);
        init_pair(14, lightpink, COLOR_WHITE);
        init_pair(15, lightyellow, COLOR_WHITE);
        init_pair(16, COLOR_WHITE, COLOR_WHITE);
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
    if (
            (y + len - 1 >= 80) ||
            (y < 0) ||
            (x < 0) ||
            (x >= 24)
            ) {
        errno = EINVAL;
        return -1;
    }
    for (int i = 0; i < len; i++) {
        struct my_char *c = (struct my_char *) text;
        c->color_and_nothing %= 16;
        if (colors_set)
            attron(COLOR_PAIR(c->color_and_nothing + 1));
        mvprintw(y, x + i, "%c", c->ch);
        if (colors_set)
            attroff(COLOR_PAIR(c->color_and_nothing + 1));
        text += 1;
    }
    move(old_y, old_x);
    refresh();
    return 0;
}

int display_move_cursor(int x, int y) {

    if (
            (y >= 80) ||
            (y < 0) ||
            (x < 0) ||
            (x >= 24)
            ) {
        errno = EINVAL;
        return -1;
    }
    return move(y, x);
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
    errno = EINVAL;
    return -1;
}