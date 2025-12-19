
# variabili 
CC = gcc 
CFLAGS = -g -Wall -pedantic

shell : shell.o
	$(CC) $(CFLAGS) shell.o -o shell  
shell.o : shell.c
	$(CC) $(CFLAGS) shell.c -c

.PHONY:clean
clean:
	rm -f *.o shell