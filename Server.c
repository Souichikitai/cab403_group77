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
//#include <linux/module.h>

#define BACKLOG 10 /* how many pending connections queue will hold */
#define MAXDATASIZE 256 /* max number of bytes we can get at once */
#define THREADS_NUM 5

// pthread_t *thread_pool;
pthread_t thread_pool[THREADS_NUM];
pid_t entry_pid[THREADS_NUM];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_new = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int sockfd, new_fd, servnumbyte;            /* listen on sock_fd, new connection on new_fd */
int send_fd;
int send_of_length;
struct sockaddr_in my_addr;    /* my address information */
struct sockaddr_in their_addr; /* connector's address information */
socklen_t sin_size;
typedef struct entry entry_t;
typedef struct pid_entry_value pentry_t;

struct pid_entry_value {
	// entry_t* first_head;
	// pentry_t entry_pid;
    unsigned long bytes;
    char* file_name;
    char* arguments;
	char* time;
    

    // char* time;
    // struct entry *next;
};

struct entry {
	// entry_t* first_head;
    pid_t entry_pid;
	
	pentry_t pid_elements;

	struct entry *next;
};


// entry_t * first_head1[THREADS_NUM];


entry_t * first_head1;

entry_t *current = NULL;

char buf[MAXDATASIZE];
void* connection_handler(int *client_socket);
char * whattime();
void *thread_controller(void *arg);
void kill_child(int signum);
void Kill_All(int sig);
void get_memory_usage(pid_t pid_value);
char *testprint(entry_t* head, int num);
int check_contents_inside_linked_list(entry_t* first_head);

// // linked list for part E


int pid_counter = 0;
entry_t* insert_entry(entry_t* first_head, pid_t _pid_, unsigned long _bytes_, char* _time_, char* _file_name, char* _arguments_){
	// pthread_mutex_lock(&mutex_new);
	// printf("I am inserting\n");
	entry_t *link = (entry_t*) malloc(sizeof(entry_t));

	link->entry_pid = _pid_;
	link->pid_elements.bytes = _bytes_;
	link->pid_elements.time = _time_;
	link->pid_elements.file_name = _file_name;
	link->pid_elements.arguments = _arguments_;
	//printf("%s\n",_file_name);
	//point to previous first node
	link->next = first_head;

	// point first to new first node
	first_head = link;
	
	
	return first_head;
	// pthread_mutex_unlock(&mutex_new);
}






// void kill_child_die(int sig);
int sleeptimes;
int waitflag;
int waitflag2;
void *thread_status;

char *arrray_log;
char *file_name1;
char *argv_item;

int log_state;
int o_state;
int t_state;

int log_index;
int o_index;
int t_index;
int excuted_file_index;

int log_location;
int o_location;
int t_location;


pid_t pidc;
/*#define MYPORT 54321 the port users will be connecting to */
//static int flags=0;

int main(int argc, char *argv[])
{
	
    // thread_pool = (pthread_t *)malloc(THREADS_NUM*sizeof(thread_pool));
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

	signal(SIGTERM, kill_child);
	signal(SIGINT, Kill_All);


	
	
	
    /* repeat: accept, send, close the connection */
    /* for every accepted connection, use a sepetate process or thread to serve it */
    while (1)
    { /* main accept() loop */
		//first_head1 = NULL;
		// pthread_mutex_lock(&mutex);
		// append_que(&sockfd);
		// pthread_mutex_unlock(&mutex);

		// for(int i =0; i < THREADS_NUM; i++){
		// 	pthread_join(thread_pool[i], NULL);
		// }

		
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

			int *pclient;
			//int last_type;//
			pclient = &new_fd;
			//pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &last_type);//
			//pthread_cleanup_push((void(*)(void *))pthread_mutex_lock, (void *)&mutex);//
			pthread_mutex_lock(&mutex);
			append_que(pclient);
			// pthread_join(thread_pool[0], NULL);
			//pthread_cond_wait(&cond, &mutex);//
			pthread_mutex_unlock(&mutex);
			//pthread_setcanceltype(last_type, NULL);//
			//pthread_cleanup_pop(0);//


			//continue;
		}else{
			perror("accept");
		}


		//pthread_create(&t1, NULL, connection_handler, pclient);
    }

		// for(int i =0; i < THREADS_NUM; i++){
		// 	pthread_join(thread_pool[i], NULL);
		// }

	// free(thread_pool);
	// pthread_exit(NULL);
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

void get_memory_usage(pid_t pid_value){

	//first_head1 = NULL;
	//entry_t* first_head = NULL;
	// pthread_mutex_lock(&mutex);

	

	char buff[512];
    FILE *f;
    sprintf(buff, "/proc/%d/maps", pid_value);
	f = fopen(buff, "r");
    if(f == NULL){
		printf("No pid\n");
		// fclose(f);
	}else{
		//printf("test\n");
		int zero_counter = 0;

		//long hex_sum;
		

		while (fgets(buff, 512, f)) {
			size_t _start, _end; 
			size_t offset, _major, _minor;
			size_t inode;	

			unsigned long start_hex_result;
			unsigned long end_hex_result;
			unsigned long divided_hex_result;	

			
			char time[4];
			int scan_result = sscanf(buff, "%12lx-%12lx %c%c%c%c %lx %lx:%lx %lu", &_start, &_end, &time[0],&time[1],&time[2],&time[3], &offset, &_major, &_minor,&inode);
			
			if (scan_result != 10){
				printf("Broken\n");
				break;	
			}
			if(inode==0){
				zero_counter++;	
				if(zero_counter==2){
					// printf("%d\n", pid_value);
					char pStart[256] = "";
					char pEnd[256] = "";
					snprintf(pStart, sizeof(pStart), "%12lx", _start);
					snprintf(pEnd, sizeof(pEnd), "%12lx", _end);
					
					start_hex_result = strtoul(pStart, NULL, 16);
					end_hex_result = strtoul(pEnd, NULL, 16);
					divided_hex_result = end_hex_result - start_hex_result;
					
					//printf("%ld\n",divided_hex_result);
					pthread_mutex_lock(&mutex_new);
					first_head1 = insert_entry(first_head1, pid_value, divided_hex_result, whattime(), file_name1, argv_item);
					pthread_mutex_unlock(&mutex_new);
					
					break;
				}
				
				//break;
			}  
		}
		fclose(f);
	}
	// pthread_mutex_unlock(&mutex);

}


void* connection_handler(int *p_thread_client_socket){

	

	int fd = *p_thread_client_socket; 
        
	if ((servnumbyte = recv(fd, buf, MAXDATASIZE, 0)) == -1)
    {
        perror("recv");
        //exit(1);       	
    }
	    
	    
    buf[servnumbyte] = '\0';
        
    char indent[] = " ";
	char *token = strtok(buf, indent);
	int len = sizeof(buf)/sizeof(buf[0]);

	char *arrray[len];

	int counter = 0;

	log_state = 0;
	o_state = 0;
	t_state = 0;

	log_index = 0;
	o_index = 0;
	t_index = 0;
	excuted_file_index = 0;

	log_location = 0;
	o_location = 0;
	t_location = 0;
	
		
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

	if(strcmp(arrray[0], "mem")!=0){
		file_name1 = arrray[excuted_file_index];
		argv_item = arrray[excuted_file_index+1];
        /*crate a new fork */
		pid_t pid;
		pid_counter++;
		pid = fork();
		pidc = pid;
		int value = 0;
		
		if(pid < 0){
			perror("Failed");
		}
		/* this is the child process */
		if(pid == 0){
			
			
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
					kill(getpid(), SIGKILL);
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
				if(value == -1){
					kill(getpid(), SIGKILL);
				}
			
			}
			
			
			//printf("%s - %s %s has been executed with pid %d\n", buffer, arrray[excuted_file_index], arrray[excuted_file_index+1], pid);
			/*close(value);
			close(sockfd);*/
			//exit(1);	
		}
		/*parent*/
		if(pid > 0){
			// printf("parent pid: %d\n", pid);
			// printf("parent getppid: %d\n", getppid());
			// printf("parent getpid: %d\n", getpid());

			//printf("here1   %d    \n", o_state);
			int status;

			if(t_state){
				sleeptimes = atoi(arrray[t_location+1]);
			}
			if(!t_state){
				sleeptimes = 10;
			}
			for(int i = 0; i < sleeptimes; i++){
				
				

				if(waitpid(pid, &status, WNOHANG)==0){
					
					sleep(1);
					get_memory_usage(pid);
					waitflag = 1;
					waitflag2 = 1;
				}else
				{
					waitflag = 0;
					waitflag2 = 0;
				}
								
			}
			if(waitflag){
				if((kill(pid, SIGTERM)) == -1){
					printf("error\n");
				}else if((kill(pid, SIGTERM)) == 0)
				{	
					raise(SIGTERM);
					for(int i = 0; i < 5; i++){
						//get_memory_usage(pidc);
						if(waitpid(pid, &status, WNOHANG)==0){
							sleep(1);
							get_memory_usage(pid);
							//printf("Z\n");
							waitflag2 = 1;
						}else if(waitpid(pid, &status, WNOHANG)==-1){
							//sleep(1);
							//printf("Error\n");
							waitflag2 = 0;
						}else
						{
							waitflag2 = 0;
							//printf("Yeah\n");
						}	
					}
				}
					
			}

			//printf("%d\n",waitflag2);
			if(waitflag2){
				if((kill(pid, SIGKILL))== -1){
					printf("error\n");
				}else if((kill(pid, SIGKILL))== 0)
				{
					if(!log_state){
						printf("%s - sent SIGKILL to %d\n", whattime(), pid);
					}
					if(log_state){
						int saved_stdout = dup(1);
						int file = open(arrray_log, O_WRONLY | O_CREAT | O_APPEND, 0777);				
						if(file == -1){
							perror("Failed");
							//return 2;
						}								
						dup2(file, 1);
						printf("%s - sent SIGKILL to %d\n", whattime(), pid);
						close(file);
						//redirect to terminal
						dup2(saved_stdout, 1);
						close(saved_stdout);
					}
					
				}

			}

			if((!waitflag)||(!waitflag2)){
				if(!log_state){
					//printf("status: %d\n", status);
					if(WIFEXITED(status)){
						const int es = WEXITSTATUS(status);
						printf("%s - %d has terminated with status code %d\n", whattime(), pid, es);		
					}
					if(WIFSIGNALED(status)){
						const int es1 = WTERMSIG(status);
						printf("%s - %d has terminated with status code %d\n", whattime(), pid, es1);
					}								
				}
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
						printf("%s - %d has terminated with status code %d\n", whattime(), pid, es);	
					}
					if(WIFSIGNALED(status)){
						const int es1 = WTERMSIG(status);
						printf("%s - %d has terminated with status code %d\n", whattime(), pid, es1);
					}
					log_state = 0;
					close(file);
					//redirect to terminal
					dup2(saved_stdout, 1);
					close(saved_stdout);	
				}
			}


			

			while (waitpid(-1, NULL, WNOHANG) > 0){
				;  /*clean up child processes*/ 
			}
		
		}
	}else{
		// testprint(first_head1);
		// pthread_mutex_lock(&mutex);
		int flag;
		
		flag = check_contents_inside_linked_list(first_head1);
		
		
		if(flag == 1){
			for(int i = 0; i<pid_counter;i++){
				char *send_item;
				send_item = testprint(first_head1, i);
				printf("%s\n", send_item);
				if (send(fd, send_item, (size_t)&send_of_length, 0) == -1){
					perror("send");	
				}	
			// exit(0);
			}
			close(fd);
		}else{
			printf("Nothing is in linked list\n");
		}
		
        // pthread_mutex_unlock(&mutex);
	}
	close(fd); 
	close(new_fd);
	/* parent doesn't need this */
	
	return 0;
}

struct entry* delete_first()
{
	/* data */
	entry_t *temp = first_head1;

	first_head1 = first_head1->next;
	if(first_head1 == NULL){
		current = NULL;
	}

	// free(temp);

	return temp;
	
};

int check_contents_inside_linked_list(entry_t* first_head) 
{ 
    entry_t* current = first_head;  // Initialize current 
	if(current == NULL){
		return 0;
	}
    // while (current != NULL) 
    // { 
    //     if (current->entry_pid == pid_v) 
    //         return 1; 
    //     current = current->next; 
    // } 
    return 1; 
}




char *testprint(entry_t* head, int num){
	printf("counter: %d\n", pid_counter);
	// pthread_mutex_lock(&mutex);
	entry_t *current_node = head;
	char * send_value = "";
	int size_of_length = 0;

	

	if(current_node==NULL){
		printf("  sEmpty \n");
		return NULL;
	}else
	{
		printf("num: %d\n",num);
		// printf("here %s\n", current_node->pid_elements.file_name);
		char * mypid = malloc(6);
		printf("koko : : : %d\n", current_node->entry_pid);
		sprintf(mypid, "%d", current_node->entry_pid);
		
		char bytes_send[256] = "";
		snprintf(bytes_send, sizeof(bytes_send), "%ld", current_node->pid_elements.bytes);
		size_of_length += (strlen(mypid));
		size_of_length +=(strlen(bytes_send));
		size_of_length +=(strlen(current_node->pid_elements.file_name));
		size_of_length += 2;
		if(current_node->pid_elements.arguments != NULL){
			size_of_length+=strlen(current_node->pid_elements.arguments);
			size_of_length ++;
		}
		send_value = malloc(size_of_length);
		send_value[0] = '\0';
		send_of_length = size_of_length;
		strcat(send_value, mypid);
		strcat(send_value, " ");
		strcat(send_value, bytes_send);
		strcat(send_value, " ");
		strcat(send_value, (current_node->pid_elements.file_name));
		if((current_node->pid_elements.arguments) != NULL){
			strcat(send_value, " ");
			strcat(send_value, (char *)(current_node->pid_elements.arguments));
		}



				int test_stdout = dup(1);
				int file = open("test_list", O_WRONLY | O_CREAT | O_APPEND, 0777);				
				if(file == -1){
					perror("Failed");
					//return 2;
				}								
				dup2(file, 1);
				printf("%d\n", current_node->entry_pid);
				printf("%ld\n", current_node->pid_elements.bytes);
				printf("%s\n", current_node->pid_elements.time);	
				printf("%s\n", current_node->pid_elements.file_name);			
				close(file);
				//redirect to terminal
				dup2(test_stdout, 1);
				close(test_stdout);


		return send_value;
	}
	
   	//while ( current_node != NULL) {
    printf("pid: %d  bytes: %ld\n", current_node->entry_pid, current_node->pid_elements.bytes);
        //current_node = current_node->next;
    //}
	// pthread_mutex_unlock(&mutex);
}


void kill_child(int signum){
	if(!log_state){
		printf("%s - sent SIGTERM to %d\n", whattime(), pidc);
		//printf("%s - %d has terminated with status code 0\n", whattime(), pidc);
	}
	if(log_state){
		int saved_stdout = dup(1);
		int file = open(arrray_log, O_WRONLY | O_CREAT | O_APPEND, 0777);				
		if(file == -1){
			perror("Failed");
			//return 2;
		}								
		dup2(file, 1);
		printf("%s - sent SIGTERM to %d\n", whattime(), pidc);
		//printf("%s - %d has terminated with status code 0\n", whattime(), pidc);
		close(file);
		//redirect to terminal
		dup2(saved_stdout, 1);
		close(saved_stdout);
	}
	
}


void Kill_All(int sig) {
	if(!log_state){
		printf("\n%s - recieved SIGINT\n", whattime());
		printf("%s - Cleaning up and terminating...\n", whattime());
	}
	if(log_state){
		int saved_stdout = dup(1);
		int file = open(arrray_log, O_WRONLY | O_CREAT | O_APPEND, 0777);				
		if(file == -1){
			perror("Failed");
			//return 2;
		}								
		dup2(file, 1);
		printf("\n%s - recieved SIGINT\n", whattime());
		printf("%s - Cleaning up and terminating...\n", whattime());
		close(file);
		//redirect to terminal
		dup2(saved_stdout, 1);
		close(saved_stdout);
	}
	
	for (int i = 0; i < THREADS_NUM; i++) {
		pthread_cancel(thread_pool[i]);
		pthread_kill(thread_pool[i], SIGKILL);
		pthread_join(thread_pool[i], NULL);
	}

	exit(1);
	// return 0;
}