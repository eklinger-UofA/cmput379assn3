make: saucer.o
	cc saucer.o -lncurses -lpthread -o saucer

saucer.o:
	cc -c saucer.h saucer.c -lncurses -lpthread
