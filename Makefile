CC = gcc
PROG = um32.out

all:
	$(CC) -std=c99 -m32 -O3 -o $(PROG) main.c um32_machine.c um32_platter.c

debug:
	$(CC) -std=c99 -m32 -g -pg o $(PROG) main.c um32_machine.c um32_platter.c

clean:
	rm -f $(PROG)
