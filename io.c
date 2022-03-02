#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <setjmp.h>


#include "io.h"

jmp_buf leave_buff;
bool io_initialized;

static void leave(void)
{
		puts("Fine sessione...arrivederci!");
		exit(EXIT_SUCCESS);
}

char *get_input(char *question, int len, char *buff, bool hide)
{
	printf("%s", question);
	// Chiedo all'utente di autenticarsi

	if(fgets(buff, len, stdin) != NULL) {
		buff[strcspn(buff, "\n")] = 0;
	} else {
		printf("Errore nella lettura dell'input utente");
		fflush(stdout);
		leave();
	}

	// Svuota stdin
	if(strlen(buff) + 1 == len) {
		int ch;
		while(((ch = getchar()) != EOF) && (ch != '\n'));
		if(ch == EOF) {
			printf("Errore nel processamento dell'input utente\n");
			fflush(stdout);
			leave();
		}
	}


	return buff;
}


bool yes_or_no(char *question, char yes, char no, bool default_answer)
{
	int extra;

	// yes and no characters should be lowercase by default
	yes = (char)tolower(yes);
	no = (char)tolower(no);

	while(true) {
		printf("%s [y=yes/n=no] : ", question);
		extra = 0;

		char c = (char)getchar();
		char ch = 0;
		if(c != '\n') {
			while(((ch = (char)getchar()) != EOF) && (ch != '\n'))
				extra++;
		}
		if(c == EOF || ch == EOF) {
			printf("EOF...Errore nel processamento dell'input utente\n");
			fflush(stdout);
			leave();
		}
		if(extra > 0)
			continue;

		// Check the answer
		if(c == '\n') {
			return default_answer;
		} else if(c == yes || c == toupper(yes)) {
			return true;
		} else if(c == no || c == toupper(no)) {
			return false;
		}
	}
}


char multi_choice(char *question, const char choices[], int num)
{
	char possibilities[2 * num * sizeof(char)];
	int i, j = 0, extra;
	for(i = 0; i < num; i++) {
		possibilities[j++] = choices[i];
		possibilities[j++] = '/';
	}
	possibilities[j - 1] = '\0'; // Remove last '/'

	while(true) {
		printf("%s [%s]: ", question, possibilities);

		extra = 0;
		char c = (char)getchar();
		if(c == '\n')
			continue;
		char ch;
		while(((ch = (char)getchar()) != EOF) && (ch != '\n'))
			extra++;
		if(c == EOF || ch == EOF) {
			printf("EOF received, leaving...\n");
			fflush(stdout);
			leave();
		}
		if(extra > 1) // Need exactly one character on stdin
			continue;

		// Check if the choice is valid
		for(i = 0; i < num; i++) {
			if(c == choices[i])
				return c;
		}
	}
}


void clear_screen(void)
{
	printf("\033[2J\033[H");
}

void press_anykey(void)
{
	char c;
	puts("\nPress any key to continue...");
	while((c = (char)getchar()) != '\n');
	(void)c;
}
