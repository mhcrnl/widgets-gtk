CC=clang -g
CFLAGS=`pkg-config --cflags gtk+-3.0`
LIBS=`pkg-config --libs gtk+-3.0`



test-slidebar:test-slidebar.c slideBar.o
	${CC} ${CFLAGS} $^ ${LIBS} -o $@

slideBar.o:slideBar.c slideBar.h
	${CC} ${CFLAGS} -c $< -o $@

.PHONY:clean

clean:
	-rm *.o
	-rm test-slidebar

