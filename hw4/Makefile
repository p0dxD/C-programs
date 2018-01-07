CC = gcc
CFLAGS = -DDEBUG -Wall -Werror -g -pthread
BIN = 320sh
LIB = libs320sh.a
LIBS = lib/
SRC_DIR = src/
INCLUDE = include/
# LFLAGS = -ltinfo -lncurses

SRC = $(wildcard $(SRC_DIR)*.c)
SRC_OBJS = $(patsubst %.c, %.o, $(SRC))
OBJS = $(wildcard $(LIBS)*.o)

all: $(BIN)

$(BIN): $(LIB)
	$(CC) -I $(INCLUDE) $(CFLAGS) $^ -o $@

$(LIB): $(SRC_OBJS)
	ar -cr $@ $(OBJS)

$(SRC_OBJS): $(SRC)
	$(CC) -I $(INCLUDE) $(CFLAGS) -c $^
	mv *.o $(LIBS)

.PHONY: clean
clean:
	rm -f $(LIBS)320sh.o $(LIB)
	rm -f $(BIN) *.o
