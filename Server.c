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
#include <pthread.h>
#include <signal.h>
#include "linked_que.h"

#define BACKLOG 10 /* how many pending connections queue will hold */
#define MAXDATASIZE 256 /* max number of bytes we can get at once */
#define THREADS_NUM 5

pthread_t thread_pool[THREADS_NUM];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int sockfd, new_fd, servnumbyte;            /* listen on sock_fd, new connection on new_fd */

struct sockaddr_in my_addr;    /* my address information */
struct sockaddr_in their_addr; /* connector's address information */
socklen_t sin_size;

char buf[MAXDATASIZE];
void* connection_handler(int *client_socket);
char * whattime();
void *thread_controller(void *arg);
void kill_child(int signum);
void Kill_All(int sig);
void kill_child_die(int sig);
int sleeptimes;
int waitflag;
pid_t pidc;
/*#define MYPORT 54321 the port users will be connecting to */
//static int flags=0;

int main(int argc, char *argv[])
{
    
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

	/*Initializing 5 threads*/
	for(int i =0; i < THREADS_NUM; i++){
		pthread_create(&thread_pool[i], NULL, thread_controller, &sockfd);
	}




    /* repeat: accept, send, close the connection */
    /* for every accepted connection, use a sepetate process or thread to serve it */
    while (1)
    { /* main accept() loop */
		signal(SIGINT, Kill_All);
		// pthread_mutex_lock(&mutex);
		// append_que(&sockfd);
		// pthread_mutex_unlock(&mutex);

		// for(int i =0; i < THREADS_NUM; i++){
		// 	pthread_join(thread_pool[i], NULL);
		// }
		signal(SIGTERM, kill_child);
		signal(SIGKILL, kill_child_die);

		sin_size = sizeof(struct sockaddr_in);
		if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) != -1)
		{
			    printf("%s - connection received from %s\n", whattime(),
    			inet_ntoa(their_addr.sin_addr));
			
			// pthread_t t;
			// //pthread_t t1;
			// int *pclient = malloc(sizeof(int));
			// *pclient = sockfd;
			// //connection_handler(pclient);         
			// pthread_create(&t, NULL, connection_handler, NULL);

			int *pclient = malloc(sizeof(int));
			*pclient = new_fd;
			pthread_mutex_lock(&mutex);
			append_que(pclient);
			pthread_mutex_unlock(&mutex);


			//continue;
		}else{
			perror("accept");
		}

		//pthread_create(&t1, NULL, connection_handler, pclient);
    }

	//free(sockfd);
	close(sockfd);

	return 0;
}

char * whattime(){

	static char buffer[26];

	time_t timer;
	
	struct tm* tm_info;
    
	timer = time(NULL);
	tm_info = localtime(&timer);
	strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
	return buffer;
}


void *thread_controller(void *arg){
	while(1){
		
		int *pclient;
		pthread_mutex_unlock(&mutex);
		pclient = remove_que();
		pthread_mutex_unlock(&mutex);

		if(pclient != NULL){
			connection_handler(pclient);
		}
	}
}

void kill_child(int signum){
	// clock_t start_time = clock();
	// if(((clock()- start_time)/CLOCKS_PER_SEC) == 10){
	// kill(pid, SIGTERM);
	// 	printf("Time up\n");
	// }
	//printf("Kill flags is %d\n", flags);
	//if(flags == 1){
		//printf("%d\n", pids);
		//kill(pids, SIGKILL);
	printf("%s - sent SIGTERM to %d\n", whattime(), pidc);
	printf("%s - %d has terminated with status code 0\n", whattime(), pidc);
	
	//sleep(5);
	//if(){
		//kill(pid, SIGKILL);
	//}
	//}
	//printf("%d\n", pids);
	//kill(pids, SIGCONT);
	//printf("%s - sent SIGTERM to %d\n", whattime(), pids);
	//printf("%s - %d has terminated with status code 0\n", whattime(), pids);
	//return 0;
}
void kill_child_die(int sig){
	printf("%s - sent SIGTERM to %d\n", whattime(), pidc);
	printf("%s - sent SIGKILL to %d\n", whattime(), pidc);
}
void Kill_All(int sig) {
	//free(buf);
	//free(thread_pool);

		printf("\n%s - recieved SIGINT\n", whattime());
		printf("%s - Cleaning up and terminating...\n", whattime());
		exit(1);
		// return 0;
	}

void* connection_handler(int *p_thread_client_socket){

	

	int fd = *p_thread_client_socket; 
        
	if ((servnumbyte = recv(fd, buf, MAXDATASIZE, 0)) == -1)
    {
        perror("recv");
        //exit(1);       	
    }
	    
	    
    buf[servnumbyte] = '\0';
    //printf("Recieved: %s\n",buf);
        
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
		token = strtok(NULL, indent);
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
		
        
    //print out which file currently attempting with given arguments
	if(log_state == 1){
		printf("We need log\n");
	}
	if(o_state == 1){
		printf("We need o\n");
	}
	if(t_state == 1){
		printf("We need t\n");
	}
			   	
        /*crate a new fork */
		pid_t pid;
		
		pid = fork();
		pidc = pid;
		int value = 0;
		
		if(pid < 0){
			perror("Failed");
		}
		/* this is the child process */
		if(pid == 0){
			printf("child pid: %d\n", pid);
			printf("child getppid: %d\n", getppid());
			printf("child getpid: %d\n", getpid());
			if(!log_state){
				if(counter >= 4){
					printf("%s - attempting to execute %s: %s\n", whattime(), arrray[excuted_file_index], arrray[excuted_file_index+1]);
				}else{
					printf("%s - attempting to execute %s\n", whattime(), arrray[excuted_file_index]);
				}		
				printf("%s - %s %s has been executed with pid %d\n", whattime(), arrray[excuted_file_index], arrray[excuted_file_index+1], getpid());
			}
			if(log_state){
				int saved_stdout = dup(1);
				int file = open(arrray[log_location+1], O_WRONLY | O_CREAT | O_APPEND, 0777);				
				if(file == -1){
					perror("Failed");
					//return 2;
				}								
				dup2(file, 1);
				if(counter >= 4){
					printf("%s - attempting to execute %s: %s\n", whattime(), arrray[excuted_file_index], arrray[excuted_file_index+1]);
				}else{
					printf("%s - attempting to execute %s\n", whattime(), arrray[excuted_file_index]);
				}	
				printf("%s - %s %s has been executed with pid %d\n", whattime(), arrray[excuted_file_index], arrray[excuted_file_index+1], getpid());
				close(file);
				//redirect to terminal
				dup2(saved_stdout, 1);
				close(saved_stdout);
			}
			
			if(o_state != 1){

				value = execlp(arrray[excuted_file_index], arrray[excuted_file_index+1], NULL);

				
				if(value == -1){
					//perror("execlp failed");
					if(counter >= 4){
						printf("%s - could not execute %s %s\n", whattime(), arrray[excuted_file_index], arrray[log_index+1]);	
					}else {
						printf("%s - could not execute %s\n", whattime(), arrray[excuted_file_index]);
					}
					
					//continue;
					
				}
			}
			if(o_state == 1){	
				int saved_stdout_o = dup(1);
				int file1 = open(arrray[o_location+1], O_WRONLY | O_CREAT | O_APPEND, 0777);
				
				if(file1 == -1){
					perror("Failed");
					//return 2;
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
			
			
			//printf("%s - %s %s has been executed with pid %d\n", buffer, arrray[excuted_file_index], arrray[excuted_file_index+1], pid);
			/*close(value);
			close(sockfd);*/
			//exit(1);	
		}
		/*parent*/
		if(pid > 0){
			printf("parent pid: %d\n", pid);
			printf("parent getppid: %d\n", getppid());
			printf("parent getpid: %d\n", getpid());

			//printf("here1   %d    \n", o_state);
			int status;
			pid_t pid2;
			pid2 = fork();

			if(pid2 < 0){
				perror("failed fork");
			}
			//parent process
			if(pid2 > 0){

				if(waitpid(pid, &status, 0)==-1){
					 perror("waitpid failed");
				}
				if(!log_state){
				//printf("%s - %s %s has been executed with pid %d\n", buffer, arrray[excuted_file_index], arrray[excuted_file_index+1], pid);
					if(WIFEXITED(status)){
					//flags = 0;
					// printf("		Test\n");
						waitflag = 1;
						const int es = WEXITSTATUS(status);
						printf("%s - %d has terminated with status code %d\n", whattime(), pid, es);
						
					}								
				}
				//printf("%d\n", flags);
				//printf("%s - %s %s has been executed with pid %d\n", buffer, arrray[excuted_file_index], arrray[excuted_file_index+1], pid);
			
				//wait(NULL);

				if(log_state == 1){	

					int saved_stdout = dup(1);

					int file = open(arrray[log_location+1], O_WRONLY | O_CREAT | O_APPEND, 0777);
				
					if(file == -1){
						perror("Failed");
						//return 2;
					}
						
					dup2(file, 1);
					//printf("%s - %s %s has been executed with pid %d\n", buffer, arrray[log_index], arrray[log_index+1], pid);
					if(WIFEXITED(status)){
						const int es = WEXITSTATUS(status);
						waitflag = 1;
						printf("%s - %d has terminated with status code %d\n", whattime(), pid, es);	
					}
					log_state = 0;
					close(file);

					//redirect to terminal
					dup2(saved_stdout, 1);
					close(saved_stdout);
				
				}
				kill(pid2, SIGKILL);			
			}
			//child process
			//NOT WORK
			if(pid2 == 0){
				if(t_state){
					sleeptimes = atoi(arrray[t_location+1]);
				}
				if(!t_state){
					sleeptimes = 10;
				}
				sleep(sleeptimes);
				//kill(pid2, SIGKILL);
				raise(SIGTERM);
				
				sleep(5);

				if((kill(pidc, SIGKILL))== -1){
					printf("errer\n");
				}else
				{
					// printf("Good\n");
					//printf("%s - sent SIGTERM to %d\n", whattime(), pidc);
				 	printf("%s - sent SIGKILL to %d\n", whattime(), pidc);
				}
				
			}
			//printf("Hi run?\n");
			while (waitpid(-1, NULL, WNOHANG) > 0)
            ;  /*clean up child processes*/ 

			
			
		}		
        close(fd); /* parent doesn't need this */

        while (waitpid(-1, NULL, WNOHANG) > 0)
            ;  /*clean up child processes*/ 
		
		return 0;
}