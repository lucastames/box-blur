CC = gcc
CFLAGS = -Wall
LDFLAGS = -ljpeg

SRC = $(wildcard src/*.c)
BIN = blur-paralela

all:
	$(CC) $(CFLAGS) $(SRC) -o $(BIN) $(LDFLAGS)

clean:
	rm -f $(BIN)
