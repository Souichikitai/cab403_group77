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


void * connection_handler(int client_socket){

}


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
    //char buf2[MAXDATASIZE];
    

    
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
        printf("Recieved: %s\n",buf);
        
        char indent[] = " ";
		char *token = strtok(buf, indent);
		int len = sizeof(buf)/sizeof(buf[0]);

		
		
		char *arrray[len];
		
		int counter = 0;

		int log_state = 0;
		int o_state = 0;
		int t_state = 0;

		int log_index = 0;
		int o_index = 0;
		int t_index = 0;
		int excuted_file_index = 0;

		int log_location = 0;
		int o_location = 0;
		int t_location = 0;

		
        while(token != NULL){
			arrray[counter] = token;
			//str[i] = token;
			token = strtok(NULL, indent);
			//printf("%s\n",token);

			
			///printf("%d: %s\n", i,arrray[i]);
			counter++;
		}

		for(int i = 0; i < counter; i++){

			if(strcmp(arrray[i], "-o") == 0){
				o_state = 1;
				o_index += 2;
				o_location = i;
				excuted_file_index+=2;
			}
			if(strcmp(arrray[i], "-log") == 0){
				log_state = 1;
				log_index += 2 + o_index;
				log_location = i;
				excuted_file_index+=2;
			}

			if(strcmp(arrray[i], "-t") == 0){
				t_state = 1;
				t_index += 2 + log_index;
				t_location = i;
				excuted_file_index+=2;
			}
		}

		//int len1 = sizeof(arrray)/sizeof(arrray[0]);
		//printf("%d\n",len);
		
		
        
       /* if ((servnumbyte = recv(new_fd, buf2, MAXDATASIZE, 0)) == -1)
    	{
        	perror("recv");
        	//exit(1);       	
    	}
	    
        buf2[servnumbyte] = '\0';
        //printf(": %s\n",buf2);
        * /
        */
        
        ///print out which file currently attempting with given arguments
		if(log_state == 1){
			printf("We need log\n");
		}
		if(o_state == 1){
			printf("We need o\n");
		}
		if(t_state == 1){
			printf("We need t\n");
		}

		if(counter >= 4){
			printf("%s - attempting to execute %s: %s\n", buffer, arrray[excuted_file_index], arrray[excuted_file_index+1]);
		}else{

			printf("%s - attempting to execute %s\n", buffer, arrray[excuted_file_index]);
		}
        

        /*crate a new fork */
		pid_t pid;
		pid = fork();
		int value = 0;
		
		if(pid < 0){
			perror("Failed");
		}
		/* this is the child process */
		if(pid == 0){

			// printf("%d\n",excuted_file_index);
			// printf("%d\n",log_index);
			//printf("   %d    \n", o_state);
			
			if(o_state != 1){

				value = execlp(arrray[excuted_file_index], arrray[excuted_file_index+1], NULL);

				if(value == -1){
					//perror("execlp failed");
					if(counter >= 4){
						printf("%s - could not execute %s %s\n", buffer, arrray[excuted_file_index], arrray[log_index+1]);	
					}else {
						printf("%s - could not execute %s\n", buffer, arrray[excuted_file_index]);
					}
					
					continue;
				}
			}
			if(o_state == 1){	
				int saved_stdout_o = dup(1);
				int file1 = open(arrray[o_location+1], O_WRONLY | O_CREAT | O_APPEND, 0777);
				
				if(file1 == -1){
					perror("Failed");
					return 2;
				}		
				//fprintf(stdout,"has been executed: %d\n", file);
						
				dup2(file1, 1);
				

				value = execlp(arrray[excuted_file_index], arrray[excuted_file_index+1], NULL);
				if(value == -1){
					perror("execlp failed");
				}

				close(file1);

				//redirect to terminal
				dup2(saved_stdout_o, 1);
				close(saved_stdout_o);
			
			}
			/*close(value);
			close(sockfd);*/
			//exit(1);	
		}
		/*parent*/
		if(pid > 0){						
			//printf("here1   %d    \n", o_state);
						
			int status;
			
			if(waitpid(pid, &status, 0)==-1){
				perror("waitpid failed");
			}
			
			printf("%s - %s %s has been executed with pid %d\n", buffer, arrray[excuted_file_index], arrray[excuted_file_index+1], pid);
			sleep(5);
			if(WIFEXITED(status)){
				const int es = WEXITSTATUS(status);
				printf("%s - %d has terminated with status code %d\n", buffer, pid, es);	
			}
			//wait(NULL);	
			


			if(log_state == 1){	

				int saved_stdout = dup(1);

				int file = open(arrray[log_location+1], O_WRONLY | O_CREAT | O_APPEND, 0777);
				
				if(file == -1){
					perror("Failed");
					return 2;
				}		
				//fprintf(stdout,"has been executed: %d\n", file);
						
				dup2(file, 1);
				printf("%s - %s %s has been executed with pid %d\n", buffer, arrray[log_index], arrray[log_index+1], pid);	
				log_state = 0;
				close(file);

				//redirect to terminal
				dup2(saved_stdout, 1);
				close(saved_stdout);
				
			}
			
		}		
        close(new_fd); /* parent doesn't need this */

        while (waitpid(-1, NULL, WNOHANG) > 0)
            ;  /*clean up child processes*/ 
            
    }
}
