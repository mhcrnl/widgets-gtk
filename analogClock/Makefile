

CFLAGS=`pkg-config --cflags gtk+-3.0`
LIBS=`pkg-config --libs gtk+-3.0 ` 


.PHONY:clean


test-analogclock:test-analogclock.c  analogClock.o
	gcc ${CFLAGS} $^  ${LIBS} -lm -o $@



analogClock.o:analogClock.c analogClock.h
	gcc ${CFLAGS} -c $< -o $@



clean:
	-rm *.o
	-rm test-analogclock
