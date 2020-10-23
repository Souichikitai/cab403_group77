#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MB 1e6

int main(){
    while(1){
        char* ptr = malloc(MB);
        memset(ptr, 40, MB);
        //sleep(9999);
    }

}