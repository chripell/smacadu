
CFLAGS=-Os -Wall `pkg-config --cflags libpcsclite`
LDFLAGS=`pkg-config --libs libpcsclite`

all: smacget smacdig smactty

smacget: smacget.o pcscwrap.o
	$(CC) $(LDFLAGS) -o $@ $^

smacget.o: smacget.c pcscwrap.h

pcscwrap.o: pcscwrap.c pcscwrap.h

smacdig: smacdig.o pcscwrap.o
	$(CC) $(LDFLAGS) -o $@ $^

smacdig.o: smacdig.c pcscwrap.h

smactty: smactty.o pcscwrap.o
	$(CC) $(LDFLAGS) -o $@ $^

smactty.o: smactty.c pcscwrap.h
