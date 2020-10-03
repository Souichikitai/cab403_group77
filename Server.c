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
#include <fcntl.h>


/*#define MYPORT 54321 the port users will be connecting to */

#define BACKLOG 10 /* how many pending connections queue will hold */
#define MAXDATASIZE 256 /* max number of bytes we can get at once */

int main(int argc, char *argv[])
{
    int sockfd, new_fd, servnumbyte;            /* listen on sock_fd, new connection on new_fd */
    struct sockaddr_in my_addr;    /* my address information */
    struct sockaddr_in their_addr; /* connector's address information */
    socklen_t sin_size;
    char buf[MAXDATASIZE];
    char buf2[MAXDATASIZE];
    

    
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
        /*    
        if ((servnumbyte = recv(sockfd, buf, MAXDATASIZE, 0)) == -1)
        {
			perror("recv");
			//exit(1);
		}
		
		printf("%d\n",servnumbyte);
		*/
        if (!fork())
        { /* this is the child process */
            if (send(new_fd, "Hello, worldj!\n", 15, 0) == -1)
                perror("send");
            close(new_fd);
            exit(0);
        }
        
	if ((servnumbyte = recv(new_fd, buf, MAXDATASIZE, 0)) == -1)
    	{
        	perror("recv");
        	//exit(1);       	
    	}
	    
	    
        buf[servnumbyte] = '\0';
        //printf("Recieved: %s\n",buf);

        
        
        if ((servnumbyte = recv(new_fd, buf2, MAXDATASIZE, 0)) == -1)
    	{
        	perror("recv");
        	//exit(1);       	
    	}
	    
        buf2[servnumbyte] = '\0';
        //printf(": %s\n",buf2);
        
        ///print out which file currently attempting with given arguments
        printf("%s - attempting to execute %s: %s\n", buffer, buf, buf2);
        

        
        /*crate a new fork */
          
		pid_t pid;
		pid = fork();
		int value = 0;
		if(pid < 0){
			perror("Failed");
		}
		/* this is the child process */
		if(pid == 0){

			
			
			value = execlp(buf, buf2, NULL);
			
			
			if(value == -1){
				//perror("execlp failed");
				printf("%s - could not execute %s %s\n", buffer, buf, buf2);

				
				continue;
			}
			
			/*close(value);
			close(sockfd);*/
			//exit(1);	
		}
		/*parent*/
		if(pid > 0){						
			
						
			int status;
			
			
		    
			if(waitpid(pid, &status, 0)==-1){
				perror("waitpid failed");
			}
			printf("%s - %s %s has been executed with pid %d\n", buffer, buf, buf2, pid);
			sleep(5);
			if(WIFEXITED(status)){
				const int es = WEXITSTATUS(status);
				printf("%s - %d has terminated with status code %d\n", buffer, pid, es);	
			}
			//wait(NULL);		
			int file = open("log_file", O_WRONLY | O_CREAT | O_APPEND, 0777);
			
			if(file == -1){
				perror("Failed");
				return 2;
			}		
			//fprintf(stdout,"has been executed: %d\n", file);
					
			dup2(file, 1);
			printf("%s - %s %s has been executed with pid %d\n", buffer, buf, buf2, pid);	
			close(file);	
		}
		

		
        close(new_fd); /* parent doesn't need this */

        while (waitpid(-1, NULL, WNOHANG) > 0)
            ;  /*clean up child processes*/ 
            
    }
}
