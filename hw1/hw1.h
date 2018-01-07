#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define MAXSIZE 80

double totalpercentage(double, double);
char type_of_instruction(unsigned int);
int set_option(char option);
int display_instruction_information(int u);
int display_register_information(int u);
int display_opcode_information(int u);
void display_help_information(void);