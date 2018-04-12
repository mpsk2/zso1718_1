//
// Created by micha on 12.04.18.
//

#ifndef HSHARE_DISPLAY_H
#define HSHARE_DISPLAY_H

int display_init();
void display_close();
int display_show(int x, int y, uint16_t* text, int len);

#endif //HSHARE_DISPLAY_H
