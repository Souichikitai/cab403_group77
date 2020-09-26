#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv)
{
	printf("I received an argument: %s\n", argv[1]);
	printf("This message is sent to stdout. In 3 seconds I will terminate with a status code 5\n");
	sleep(3);
	return 5;
}