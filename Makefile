#Makefile del programma asl_db

CC	= gcc
OPTIMIZE	= -g -wall -wextra
CFLAGS	= $(OPTIMIZE)
PROGS	= main.c
LIBS	= `mysql_config --cflags --include --libs`

all:clean
	$(CC) $(CFLAGS) -o asl_db.exe $(PROGS)$(LIBS)

.PHONY: clean
clean:
	-rm asl_db.exe
