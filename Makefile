CC = gcc 
CFLAGS = -Wall # Show all reasonable warnings
LDFLAGS = -pthread
OBJS = overseer.o 

all: files 

files: controller.o overseer.o test.o 

controller.o: Client.c
			$(CC) -o controller Client.c $(CFLAGS)

overseer.o: Server.c
			$(CC) -o overseer Server.c linked_que.c $(CFLAGS) $(LDFLAGS)

test.o: test.c
			$(CC) -o test test.c

# linked_que.o: linked_que.c linked_que.h
# 			$(CC) -o linked_que linked_que.c 

clean:
	rm -f overseer *.o , controller *.o
 
.PHONY: clean