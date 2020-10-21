#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MB 1e6

int main(){
    while(1){
        char* ptr = malloc(MB);
        memset(ptr, 1, MB);
        sleep(1);
    }

}