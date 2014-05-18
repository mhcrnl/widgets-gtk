

.PHONT:all clean


all:
	cd floatBorder && ${MAKE}
	cd digitalClock && ${MAKE}
	cd analogClock && ${MAKE}

clean:
	cd floatBorder && ${MAKE} clean
	cd digitalClock && ${MAKE} clean 
	cd analogClock && ${MAKE} clean

