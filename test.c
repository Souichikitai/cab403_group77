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
	printf("Hello from test and given: %s\n", argv[0]);
	exit(1);
    return 0;
}
