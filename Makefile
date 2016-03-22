
CFLAGS=-Os -Wall `pkg-config --cflags libpcsclite`
LDLIBS=`pkg-config --libs libpcsclite`

all: smacget smacdig smactty

smacget: smacget.o pcscwrap.o

smacget.o: smacget.c pcscwrap.h

pcscwrap.o: pcscwrap.c pcscwrap.h

smacdig: smacdig.o pcscwrap.o

smacdig.o: smacdig.c pcscwrap.h

smactty: smactty.o pcscwrap.o

smactty.o: smactty.c pcscwrap.h
