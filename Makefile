CC = gcc 
CFLAGS = -Wall # Show all reasonable warnings
LDFLAGS = 

all: files 

files: controller.o overseer.o

controller.o: Client.c
			$(CC) -o controller Client.c

overseer.o: Server.c
			$(CC) -o overseer Server.c

clean:
	rm -f fibfork *.o 
 
.PHONY: clean