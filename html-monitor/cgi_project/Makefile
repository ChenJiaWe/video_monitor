CROSS_COMPILER:=

ifeq ($(ARCH),arm)
CROSS_COMPILER=arm-linux-gnueabihf-
else
CROSS_COMPILER=
endif



CFLAGS=-g -Wall
CC=$(CROSS_COMPILER)gcc
AR=$(CROSS_COMPILER)ar
RANLIB=ranlib
LIBS=-L./ -lcgic

all: libcgic.a  capture login.cgi beep.cgi env1.cgi

install: libcgic.a
	cp libcgic.a /usr/local/lib
	cp cgic.h /usr/local/include
	@echo libcgic.a is in /usr/local/lib. cgic.h is in /usr/local/include.

libcgic.a: cgic.o cgic.h
	rm -f libcgic.a
	$(AR) rc libcgic.a cgic.o
	$(RANLIB) libcgic.a

#mingw32 and cygwin users: replace .cgi with .exe



login.cgi: login.o libcgic.a
	$(CC) login.o -o login.cgi ${LIBS}

beep.cgi: beep.o libcgic.a
	$(CC) beep.o -o beep.cgi ${LIBS}

env1.cgi: env1.o libcgic.a
	$(CC) env1.o -o env1.cgi ${LIBS}

capture: capture.o libcgic.a
	$(CC) capture.o -o capture ${LIBS}

copy:
	cp -f *.cgi ~/workdir/boa-serve/cgi-bin

clean:
	rm -f *.o *.a  capture

