CC = gcc 
CFLAGS = -Wall # Show all reasonable warnings
LDFLAGS = 

all: files 

files: controller.o overseer.o test.o

controller.o: Client.c
			$(CC) -o controller Client.c

overseer.o: Server.c
			$(CC) -o overseer Server.c

test.o: test.c
			$(CC) -o test test.c

clean:
	rm -f overseer *.o , controller *.o
 
.PHONY: clean