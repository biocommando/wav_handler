CC:=gcc
LIBS:=-lm
CFLAGS:=-Wall

test:
	$(CC) -o $@ test.c wav_handler.c $(CFLAGS) $(LIBS)

clean:
	rm -rf test
