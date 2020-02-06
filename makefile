# The variabel CC is the compiler to use
CC=gcc
# CFLAGS will be the options we'll pass to the compiler.
CFLAGS=-c -Wall

all: mysh

mysh: mysh.o mycat mycp myls
	$(CC) mysh.o -o mysh

mysh.o:
	$(CC) $(CFLAGS) mysh.c

mycat: mycat.o
	$(CC) mycat.o -o mycat

mycp: mycp.o
	$(CC) mycp.o -o mycp

myls: myls.o
	$(CC) myls.o -o myls

mycat.o: mycat.c
	$(CC) $(CFLAGS) mycat.c

mycp.o: mycp.c
	$(CC) $(CFLAGS) mycp.c

myls.o: myls.c
	$(CC) $(CFLAGS) myls.c

clean:
	rm *.o mysh mycat mycp myls
