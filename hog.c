#include <stdlib.h>
#include <unistd.h>

#define MB 1e6
int main(int argc, char **argv)
{
        char* ptr = malloc(atoi(argv[1]) * MB);
        sleep(9999);
 

}
