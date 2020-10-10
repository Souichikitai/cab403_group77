#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>

int main(int argc, char *argv[])
{
	printf("I recieved an argument: %s\n", argv[0]);

	fflush(stdout);
	

	sleep(2);

	fprintf(stderr, "stdderr wrriten\n");

	printf("This message is sent to stdout. In three seconds I will terminate with a status code 5\n");

	sleep(3);
    return 5;
}
