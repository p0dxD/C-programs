CC = gcc
CFLAGS = -DDEBUG -Wall -Werror -g -pthread
SALTFLAGS = -lssl -lcrypto
BIN = server client chat
LIB = server.a client.a chat.a
LIBS = lib/
SRC_DIR = src/
INCLUDE = include/
SOURCE_C = $(SRC_DIR)csapp.c $(SRC_DIR)hw5.c $(SRC_DIR)sfwrite.c
SOURCE_O = $(LIBS)csapp.o $(LIBS)hw5.o $(LIBS)sfwrite.o
# LFLAGS = -ltinfo -lncurses
SRC_SERVER = $(SRC_DIR)server.c $(SOURCE_C)
SRC_CLIENT = $(SRC_DIR)client.c $(SOURCE_C)
SRC_CHAT = $(SRC_DIR)chat.c $(SOURCE_C)
#LIBS
LIBS_CHAT = $(LIBS)chat.o $(SOURCE_O)
LIBS_SERVER = $(LIBS)server.o $(SOURCE_O)
LIBS_CLIENT = $(LIBS)client.o $(SOURCE_O)

all: $(BIN)

server: $(SRC_SERVER)
	$(CC) -I $(INCLUDE) $(CFLAGS) -c $^
	mv *.o $(LIBS)
	ar -cr $@.a $(LIBS_SERVER)
	$(CC) -I $(INCLUDE) $(CFLAGS) $@.a -o $@ $(SALTFLAGS)

client: $(SRC_CLIENT)
	$(CC) -I $(INCLUDE) $(CFLAGS) -c $^
	mv *.o $(LIBS)
	ar -cr $@.a $(LIBS_CLIENT)
	$(CC) -I $(INCLUDE) $(CFLAGS) $@.a -o $@

chat: $(SRC_CHAT)
	$(CC) -I $(INCLUDE) $(CFLAGS) -c $^
	mv *.o $(LIBS)
	ar -cr $@.a $(LIBS_CHAT)
	$(CC) -I $(INCLUDE) $(CFLAGS) $@.a -o $@

.PHONY: clean
clean:
	rm -f $(LIBS)*.o $(LIB)
	rm -f $(BIN) *.o
