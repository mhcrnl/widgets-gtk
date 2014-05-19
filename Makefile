

.PHONT:all clean


all:
	cd slideBar && ${MAKE}
	cd floatBorder && ${MAKE}
	cd digitalClock && ${MAKE}
	cd analogClock && ${MAKE}

clean:
	cd slideBar && ${MAKE} clean
	cd floatBorder && ${MAKE} clean
	cd digitalClock && ${MAKE} clean 
	cd analogClock && ${MAKE} clean

