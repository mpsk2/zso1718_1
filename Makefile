emu:
	gcc -Wall -lncurses --std=c11 -O2 src/debug.c src/display.c src/main.c -o emu
