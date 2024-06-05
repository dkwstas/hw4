#Compiler
CC = gcc
#Compiler flags
CFLAGS = -Wall -g
#All executable file to be created
EXEC = hw4 p4diff

all: $(EXEC)

hw4: hw4.o
	$(CC) $(CFLAGS) $< -o $@ -fsanitize=address

p4diff: p4diff.o
	$(CC) $(CFLAGS) $< -o $@ -fsanitize=address

hw4.o: hw4.c
	$(CC) $(CFLAGS) -c $<

p4diff.o: p4diff.c
	$(CC) $(CFLAGS) -c $<

clean:
	$(RM) hw4.o
	$(RM) p4diff.o
	$(RM) hw4
	$(RM) p4diff