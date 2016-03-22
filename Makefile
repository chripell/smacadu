
CFLAGS=-Os -Wall `pkg-config --cflags libpcsclite`
LDLIBS=`pkg-config --libs libpcsclite`

all: smacget smacdig smactty

smacget: smacget.o pcscwrap.o
	$(CC) -o $@ $^ $(LDLIBS)

smacget.o: smacget.c pcscwrap.h

pcscwrap.o: pcscwrap.c pcscwrap.h

smacdig: smacdig.o pcscwrap.o
	$(CC) -o $@ $^ $(LDLIBS)

smacdig.o: smacdig.c pcscwrap.h

smactty: smactty.o pcscwrap.o
	$(CC) -o $@ $^ $(LDLIBS)

smactty.o: smactty.c pcscwrap.h
