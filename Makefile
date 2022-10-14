all:	a2.o
	cc -o a2 a2.o

CC = cc 
FLAGS = -Wall -pedantic -Wextra

a2.o:	a2.c
	$(CC) $(FLAGS) -c a2.c

