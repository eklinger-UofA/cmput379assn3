make:
	cc -c saucer.c saucer.h
	cc saucer.o -lncurses -lpthread -o saucer

run:
	./saucer

clean:
	rm -f *.o *~ core *.h.gch
