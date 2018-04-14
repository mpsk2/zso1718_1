CFLAGS=-O2 -Wall -pedantic --std=c11 -DDEBUG=1 -DHAS_COLOR=0
LDFLAGS=-lncurses
DEPS = alienos.h debug.h display.h
OBJ = debug.o display.o main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

emu: $(OBJ)
	gcc -o $@ $^ $(LDFLAGS)

clean:
	rm emu
	rm *.o
