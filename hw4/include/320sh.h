#ifndef __320SH__
#define __320SH__

#include "csapp.h"
#include "320debug.h"
#include <getopt.h>
// #include <ncurses.h>
// #include <curses.h>
// #include <curses.h>

#define MAXARGS 128
typedef struct History History;

struct History{
  char cmd[MAX_INPUT];/*Largest it can be*/
  History *prev;
  History *next;
};

void history_exec();
void clearHistory_exec();
void help_exec();
void eval(char *cmd);
int tokenize(char *buf, char **argv);
int shell_command(char **argv, int argc);
int cd_exec(char **argv);
int pwd_exec(void);
char *get_current_dir();
int echo_exec(char **argv, int argc);
int set_exec(char **argv, int argc);
int stat_check (char *file);
int tokenize_k(char *buf, char **argv);



/*for history*/
void enqueue(char *command);
void dequeue(void);
void clearHistoryCounter(void);
History *getHead(void);
History *getTail(void);
void initqueue(void);
char *get_next(void);
char *get_prev(void);
void write_history();
void insert(char *dest, char *strA, char *strB, int offset);
void delete_char(char *dest, char *str, int i);
void read_history();
void write_history();
#endif
