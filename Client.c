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


#define MAXDATASIZE 100 /* max number of bytes we can get at once */

static void show_error(){
		fprintf(stderr, "usage: controller <address> <port> {[- out_file] [-log log_file] [-t seconds] <file> [arg...] | mem [pid] | memkill <percent>}\n");
		exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd, new_fd, numbytes;
    char buf[MAXDATASIZE];
    struct hostent *he;
    struct sockaddr_in their_addr; /* connector's address information */
    struct sockaddr_in connect_addr; /* connector's address information */
    socklen_t sin_size;

    if (argc != 3)
    {
        //fprintf(stderr, "usage: <hostname> <port number>\n");
        //exit(1);
    }
    
	if (strcmp(argv[1], "--help") == 0){
		show_error();
	}
	
    int port = atoi(argv[2]);
    #hello

    if ((he = gethostbyname(argv[1])) == NULL)
    { /* get the host info */
        show_error();
        exit(1);
    }

	/* generate the socket */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    /* clear address struct */
    memset(&their_addr, 0, sizeof(their_addr));

    their_addr.sin_family = AF_INET;   /* host byte order */
    their_addr.sin_port = htons(port); /* short, network byte order */
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);

    if (connect(sockfd, (struct sockaddr *)&their_addr,
                sizeof(struct sockaddr)) == -1)
    {
        //perror("connect");
        fprintf(stderr, "Could not connect with port %d\n", port);
        exit(1);
    }

    if ((numbytes = recv(sockfd, buf, MAXDATASIZE, 0)) == -1)
    {
        perror("recv");
        exit(1);
    }
    printf("%d\n",numbytes);
    
    buf[numbytes] = '\0';

    printf("Received: %s\n", buf);
    
    /*crate a new fork */    
    pid_t pid;
    pid = fork();
    int value = 0;
    if(pid < 0){
		perror("Failed");
	}
	if(pid == 0){
		
        /* this is the child process */
        value = execlp(argv[3], "from test", NULL);
        if(value == -1){
			perror("execlp failed");
		}
		

		
		//close(value);
		//close(sockfd);
		//exit(1);	
	}
	
		sin_size = sizeof(struct sockaddr_in);
        if ((new_fd = accept(sockfd, (struct sockaddr *)&connect_addr,
                             &sin_size)) == -1)
        {
            //perror("accept");
            //continue;
        }
        if(!fork()){
			for(int i = 3; i<= argc; ++i){
				if (send(sockfd, argv[i] , MAXDATASIZE, 0) == -1){
				perror("send");
				close(new_fd);
				exit(0);
				}
				/*
				if (send(sockfd, argv[3] , MAXDATASIZE, 0) == -1){
				perror("send");
				close(new_fd);
				exit(0);
				}
				if (send(sockfd, argv[4] , MAXDATASIZE, 0) == -1){
					perror("send");
					close(new_fd);
					exit(0);
					
				}*/
			}
		}
//sizeof(argv[3])

    

    close(new_fd);

    return 0;
}
