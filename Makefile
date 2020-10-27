CC = gcc 
CFLAGS = -Wall # Show all reasonable warnings
LDFLAGS = -pthread
OBJS = overseer.o 

all: files 

files: controller.o overseer.o

controller.o: controller.c
			$(CC) -o controller controller.c $(CFLAGS)

overseer.o: overseer.c
			$(CC) -o overseer overseer.c linked_que.c $(CFLAGS) $(LDFLAGS)


clean:
	rm -f overseer *.o , controller *.o
 
.PHONY: clean