
CC=gcc -g
CFLAGS=`pkg-config --cflags gtk+-3.0`
LIBS=`pkg-config --libs gtk+-3.0` -lm




.PHONY:clean

test-digitalclock: test-digitalclock.c digitalClock.o
	${CC} ${CFLAGS} $^ ${LIBS} -o $@

digitalClock.o:digitalClock.c digitalClock.h
	${CC} ${CFLAGS} -c $< -o $@



clean:
	-rm *.o
	-rm test-digitalclock

