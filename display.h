//
// Michał Piotr Stankiewicz
// ms335789
// Zadanie zaliczeniowe ZSO 2017 / 2018
// Zadanie numer 1
//

#ifndef HSHARE_DISPLAY_H
#define HSHARE_DISPLAY_H

int display_init();
void display_close();
int display_show(int x, int y, uint16_t* text, int len);
int display_move_cursor(int x, int y);
int display_read_char(int* ch);

#endif //HSHARE_DISPLAY_H
