#pragma once
#include <stdbool.h>


extern char *get_input(char *question, int len, char *buff, bool hide);
extern bool yes_or_no(char *question, char yes, char no, bool default_answer, bool insensitive);
extern char multi_choice(char *question, const char choices[], int num);
extern void clear_screen(void);
extern void press_anykey(void);
