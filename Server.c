/*
*  Materials downloaded from the web.
*  Collected and modified for teaching purpose only.
*/

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

/*#define MYPORT 54321 the port users will be connecting to */

#define BACKLOG 10 /* how many pending connections queue will hold */

int main(int argc, char *argv[])
{
    int sockfd, new_fd;            /* listen on sock_fd, new connection on new_fd */
    struct sockaddr_in my_addr;    /* my address information */
    struct sockaddr_in their_addr; /* connector's address information */
    socklen_t sin_size;
    
    

    
    /* generate the socket */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }
    
    if(argc != 2){
		fprintf(stderr, "usage: <port number>\n");
		exit(1); 
	}
	
	int my_port = atoi(argv[1]);

    /* Enable address/port reuse, useful for server development */
    int opt_enable = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt_enable, sizeof(opt_enable));
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &opt_enable, sizeof(opt_enable));
    
   
    /* clear address struct */
    memset(&my_addr, 0, sizeof(my_addr));

    /* generate the end point */
    my_addr.sin_family = AF_INET;         /* host byte order */
    my_addr.sin_port = htons(my_port);     /* short, network byte order */
    my_addr.sin_addr.s_addr = INADDR_ANY; /* auto-fill with my IP */

    /* bind the socket to the end point */
    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("bind");
        exit(1);
    }

    /* start listnening */
    if (listen(sockfd, BACKLOG) == -1)
    {
        perror("listen");
        exit(1);
    }

    printf("server starts listening ...\n");

    /* repeat: accept, send, close the connection */
    /* for every accepted connection, use a sepetate process or thread to serve it */
    while (1)
    { /* main accept() loop */
		
		time_t timer;
		char buffer[26];
		struct tm* tm_info;
    
		timer = time(NULL);
		tm_info = localtime(&timer);
		strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
		
        sin_size = sizeof(struct sockaddr_in);
        if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr,
                             &sin_size)) == -1)
        {
            perror("accept");
            continue;
        }
        
        ///Print time and connection messages        
        printf("%s - connection received from %s\n", buffer,
            inet_ntoa(their_addr.sin_addr));

        if (!fork())
        { /* this is the child process */
            if (send(new_fd, "Hello, world!\n", 14, 0) == -1)
                perror("send");
            close(new_fd);
            exit(0);
        }
        close(new_fd); /* parent doesn't need this */

        while (waitpid(-1, NULL, WNOHANG) > 0)
            ; /* clean up child processes */
    }
}
