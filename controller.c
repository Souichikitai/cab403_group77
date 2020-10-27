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
#include <ctype.h>

#define MAXDATASIZE 255 /* max number of bytes we can get at once */

/*
    -------Group 77-------
    Chig Yin Chen n9647465
    Sohichi Kitai n10140981
    Nigel Rabia n10236708

*/


/*Error Message showing the proper notation for client input */
static void show_error(){
		fprintf(stderr, "usage: controller <address> <port> {[- out_file] [-log log_file] [-t seconds] <file> [arg...] | mem [pid] | memkill <percent>}\n");
		exit(1);
}

int main(int argc, char *argv[]) 
{
    int sockfd;
    int numbytes;
    char buf[1026];
    struct hostent *he;
    struct sockaddr_in their_addr; 

    if (argc < 3)  /*Screens Each Argument and prints a usage error depending on the input of user */
    {
        show_error(); 
        exit(1);
    }

    if(argv[3] == NULL){ /*Error output when nothing is written in the third argument */
        show_error();
        exit(1);
    }
    
	if (strcmp(argv[1], "--help") == 0){ /*Error output when --help is written in the first argument */
		show_error();
	}

    if(argv[2] == NULL){ /*Error output when nothing is written in the second argument */
        show_error();
        exit(1);
    }
	
    int port = atoi(argv[2]);/*Error output when nothing is written in the second argument */

    if ((he = gethostbyname(argv[1])) == NULL)
    { /* get the host info */
        show_error();
        perror("wrong");
        exit(1);
    }

    if(strcmp(argv[1], "localhost") < 0){ /*Error output when nothing is written in the first argument */
        fprintf(stderr, "Could not connect to overseer %s at %d\n", argv[1],port);
        exit(1);
    }
    

	/* generate the socket */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        show_error();
        perror("socket");
        exit(1);
    }

    for(int i = 0; i<argc;i++){ /*Error output when "-t" is input incorrectly */
        if((strcmp(argv[i], "-t") == 0)){
            if((atoi(argv[i+1]) == 0)){
                show_error();
                exit(1);
            }
            
        }
    }

    /* clear address struct */
    memset(&their_addr, 0, sizeof(their_addr));

    their_addr.sin_family = AF_INET;   /* host byte order */
    their_addr.sin_port = htons(port); /* sort, network byte order */
    their_addr.sin_addr = *((struct in_addr *)he->h_addr); /* sort, address byte order */

    if (connect(sockfd, (struct sockaddr *)&their_addr,
                sizeof(struct sockaddr)) == -1)
    {
        perror("connect");
        fprintf(stderr, "Could not connect with port %d\n", port);
        exit(1);
    }
   
	char * send_value = "";
 
    int size_of_length = 0;
		
	for(int i=3; i< argc; i++){
			size_of_length+=strlen(argv[i]);
			if(argc > i+1){
				
				size_of_length++;
			}
	}
			
		send_value = malloc(size_of_length);
		send_value[0] = '\0';
		
		for(int i = 3; i < argc; i++){

			strcat(send_value, argv[i]);
			if(argc > i+1)
			{
				strcat(send_value, " ");
			}
								
		}
			
		if (send(sockfd, send_value , (size_t)&size_of_length, 0) == -1){
			perror("send");
				
			exit(0);
		}

        if(strcmp(argv[3], "mem") == 0){
            
                if ((numbytes = recv(sockfd, buf, MAXDATASIZE, 0)) == -1)
                {
                    perror("recv_in_here");
                    
                }else
                {
                    buf[numbytes] = '\0';
       
                        printf("%s\n", buf);                              
                }                       
        }

    free(send_value);

    return 0;
}