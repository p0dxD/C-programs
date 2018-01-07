CC = gcc
CFLAGS = -Wall -Werror -g
DDEBUG = -g -DCSE320
BIN = utfconverter

SRC = $(wildcard *.c)

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean

clean:
	rm -f *.o $(BIN)

