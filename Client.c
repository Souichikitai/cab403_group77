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

#define MAXDATASIZE 100 /* max number of bytes we can get at once */

/*Error Message showing the proper notation for client input */
static void show_error(){
		fprintf(stderr, "usage: controller <address> <port> {[- out_file] [-log log_file] [-t seconds] <file> [arg...] | mem [pid] | memkill <percent>}\n");
		exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd;
    int numbytes;
    char buf[MAXDATASIZE];
    struct hostent *he;
    struct sockaddr_in their_addr; 
    
    /*Screens Each Argument and prints a usage error depending on the input of user */
    if (argc < 3)
    {
        //fprintf(stderr, "usage: <hostname> <port number>\n");
        //exit(1);
        show_error();
        exit(1);
    }

    if(argv[3] == NULL){  /*Error output when nothing is written in the third argument */
        show_error();
        exit(1);
    }
    
	if (strcmp(argv[1], "--help") == 0){  /*Error output when --help is written in the first argument */
		show_error();
	}

    if(argv[2] == NULL){  /*Error output when nothing is written in the second argument */
        show_error();
        exit(1);
    }
	
    int port = atoi(argv[2]);
    int localhost1 =  atoi(argv[1]);
    //int flag = 0;


    if ((he = gethostbyname(argv[1])) == NULL)
    { /* get the host info */
        show_error();
        //fprintf(stderr, "Could not connect with port %d\n", port);
        perror("wrong");
        exit(1);
    }

    if(strcmp(argv[1], "localhost") < 0){
       fprintf(stderr, "Could not connect to overseer %d at %d\n",localhost1,port);
        exit(1);
    }

    //if(isalpha(argv[2])){
        //show_error();
        //exit(1);
    //}
    //if((strcmp(argv[5], "-o")==0)||(strcmp(argv[7], "-o")==0)||(strcmp(argv[7], "-log")==0)){
        //show_error();
        //exit(1);
    //}
    

	/* generate the socket */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        show_error();
        perror("socket");
        exit(1);
    }

    /* clear address struct */
    memset(&their_addr, 0, sizeof(their_addr));

    their_addr.sin_family = AF_INET;   /* host byte order */
    their_addr.sin_port = htons(port); /* short, network byte order */
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);  /* address network byte order */

    if (connect(sockfd, (struct sockaddr *)&their_addr,
                sizeof(struct sockaddr)) == -1)
    {
        perror("connect");
        fprintf(stderr, "Could not connect with port %d\n", port);
        exit(1);
    }


    
	char * send_value = "";
	//int * sending_size = 0;

    
    int size_of_length = 0;
		
	for(int i=3; i< argc; i++){
			size_of_length+=strlen(argv[i]);
			if(argc > i+1){
				
				size_of_length++;
			}
	}
		
		//printf("%d\n",size_of_length);
		
		send_value = malloc(size_of_length);
		send_value[0] = '\0';
		//sending_size = malloc(size_of_length);
		//sending_size = '\0';
		
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
            while (1)
            {
                if ((numbytes = recv(sockfd, buf, MAXDATASIZE, 0)) == -1)
                {
                    perror("recv_in_here");
                    // exit(1);
                    break;
                }else
                {
                    buf[numbytes] = '\0';
            
                    printf("%s\n", buf);
                }
                
            }
        }





    free(send_value);
//arguments.split(" ");
    return 0;
}
