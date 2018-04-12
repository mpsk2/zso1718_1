//
// Created by micha on 12.04.18.
//

#include <sys/types.h>
#include <curses.h>
#include <errno.h>
#include <zconf.h>
#include "../include/display.h"
#include "../include/debug.h"

static WINDOW* main_window;

int display_init() {
    main_window = initscr();
    if ( main_window == 0 ) {
        return -1;
    }
    start_color();
    if ( has_colors() && COLOR_PAIRS >= 15) {
        // TODO
        debug("Has colors\n");
        init_pair(0,  COLOR_BLACK,     COLOR_WHITE);
        init_pair(1,  COLOR_BLUE,     COLOR_WHITE);
        init_pair(2,  COLOR_GREEN,   COLOR_WHITE);
        init_pair(3,  COLOR_MAGENTA,  COLOR_WHITE); // TODO
        init_pair(4,  COLOR_RED,    COLOR_WHITE);
        init_pair(5,  COLOR_RED, COLOR_WHITE); // TODO
        init_pair(6,  COLOR_YELLOW,    COLOR_WHITE);
        init_pair(7,  COLOR_BLACK,    COLOR_WHITE); // TODO
        init_pair(8,  COLOR_BLACK,   COLOR_WHITE); // TODO
        init_pair(9,  COLOR_BLUE,   COLOR_WHITE); // TODO
        init_pair(10, COLOR_GREEN,    COLOR_WHITE); // TODO
        init_pair(11, COLOR_MAGENTA,   COLOR_WHITE); // TODO
        init_pair(12, COLOR_RED,   COLOR_WHITE); // TODO
        init_pair(13, COLOR_RED,   COLOR_WHITE); // TODO
        init_pair(14, COLOR_YELLOW, COLOR_WHITE); // TODO
        init_pair(15, COLOR_WHITE, COLOR_WHITE); // TODO
    } else {
        debug("Has no colors\n");
    }
    refresh();
    return 0;
}

void display_close() {
    if ( main_window != NULL ) {
        delwin(main_window);
        endwin();
        refresh();
    }
}

struct my_char {
    char ch;
    uint8_t color_and_nothing;
};

int display_show(int x, int y, uint16_t* text, int len) {
    fprintf(stderr, "Print(x=%d, y=%d, n=%d)\n", x, y, len);
    if (
            (y + len - 1 >= 80) &&
                    (y < 0) &&
                    (x < 0) &&
                    (x >= 24)
            ) {
        errno = EINVAL;
        return -1;
    }
    for (int i = 0; i < len; i++) {
        struct my_char* c = (struct my_char*) text;
        c->color_and_nothing /= 32;
        debug("\taddr=%d, ch=%c, color=%u\n", text, c->ch, c->color_and_nothing );
        color_set(c->color_and_nothing, NULL);
        mvaddch(x, y + i, c->ch);
        text += 1;
    }
    refresh();
    sleep(3);

    return 0;
}