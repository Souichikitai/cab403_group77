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

#define BACKLOG 10      /* how many pending connections queue will hold */
#define MAXDATASIZE 256 /* max number of bytes we can get at once */
#define THREADS_NUM 5   /* max number of threads we can get at once */

// pthread_t *thread_pool;
pthread_t thread_pool[THREADS_NUM]; /* Linked list Entry */
pid_t entry_pid[THREADS_NUM];       /* Linked list Entry */


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_new = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int sockfd, new_fd, servnumbyte;  /* listen on sock_fd, new connection on new_fd */
int send_fd;                      /*file descriptor-store all data from server to client */                     
int send_of_length;               /* length of value sent to client  */
int send_of_length_pid;           /*Used for t sending memeory history to client   */
struct sockaddr_in my_addr;       /* my address information */
struct sockaddr_in their_addr;    /* connector's address information */
socklen_t sin_size;                       /* Socket size */
typedef struct entry entry_t;             /* Linked list Entry */
typedef struct pid_entry_value pentry_t;  /* Linked list PID value */

struct pid_entry_value {
    unsigned long bytes;
    char* file_name;
    char* arguments;
	char* time;
};

struct entry {
    pid_t entry_pid;
	pentry_t pid_elements;
    struct entry *next;
};

entry_t * first_head1 = NULL;
entry_t *current = NULL;

char buf[MAXDATASIZE];
void* connection_handler(int *client_socket);
char * whattime();
void *thread_controller(void *arg);
void kill_child(int signum);
void Kill_All(int sig);
void get_memory_usage(pid_t pid_value);
char * testprint(entry_t* head);
int check_contents_inside_linked_list(entry_t* first_head);
int check_contents_inside_linked_list2(entry_t* first_head, pid_t pid_v);
void printAll(entry_t *first_head);
char * print_specific_pid(entry_t *first_head, char * new_pid);
void find_latest_linked_list(entry_t *first_head);


// // linked list for part E


int pid_counter = 0;
entry_t* insert_entry(entry_t* first_head, pid_t _pid_, unsigned long _bytes_, char* _time_, char* _file_name, char* _arguments_){
	entry_t *link = (entry_t*) malloc(sizeof(entry_t));

	link->entry_pid = _pid_;
	link->pid_elements.bytes = _bytes_;
	link->pid_elements.time = _time_;
	link->pid_elements.file_name = _file_name;
	link->pid_elements.arguments = _arguments_;
	link->next = first_head;
	first_head = link;

	return first_head;

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
    
    /*Server prints in terminal that it is now live*/
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
		sin_size = sizeof(struct sockaddr_in);
		if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) != -1)
		{
			    printf("%s - connection received from %s\n", whattime(),
    			inet_ntoa(their_addr.sin_addr));
			
			int *pclient;
			pclient = &new_fd;
			pthread_mutex_lock(&mutex);
			append_que(pclient);
		    pthread_mutex_unlock(&mutex);
		
		}else{
			perror("accept");
		}


	
    }

		
	close(sockfd);

	return 0;
}

/*Time*/
char * whattime(){

	static char buffer[26];

	time_t timer;
	
	struct tm* tm_info;
    
	timer = time(NULL);
	tm_info = localtime(&timer);
	strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
	return buffer;
}

/*Time*/
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

/*Time*/
void get_memory_usage(pid_t pid_value){
	char buff[512];
    FILE *f;
    sprintf(buff, "/proc/%d/maps", pid_value); 
	       f = fopen(buff, "r");
    if(f == NULL){
		printf("No pid\n");
	}else{
		int zero_counter = 0;

	
		

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
				    char pStart[256] = "";
					char pEnd[256] = "";
					snprintf(pStart, sizeof(pStart), "%12lx", _start);
					snprintf(pEnd, sizeof(pEnd), "%12lx", _end);
					
					start_hex_result = strtoul(pStart, NULL, 16);
					end_hex_result = strtoul(pEnd, NULL, 16);
					divided_hex_result = end_hex_result - start_hex_result;
					
				
					pthread_mutex_lock(&mutex_new);
					char * this_time = malloc(19);
					sprintf(this_time, "%s", whattime());
					first_head1 = insert_entry(first_head1, pid_value, divided_hex_result, this_time, file_name1, argv_item);
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

/*Time*/
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

		if(strcmp(arrray[i], "-o") == 0){ /*-o*/
			o_state = 1;
			o_index += 2;
			o_location = i;
			excuted_file_index+=2;
		}
		if(strcmp(arrray[i], "-log") == 0){ /*Log*/
			log_state = 1;
			log_index += 2 + o_index;
			log_location = i;
			excuted_file_index+=2;
		}

		if(strcmp(arrray[i], "-t") == 0){  /*Time*/
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
			
			/*Informs User on status of log file Execution and prints on terminal*/
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
					perror("Failed");/*Informs User on status of log file that has failed to open log file*/
					//return 2;
				}								
				dup2(file, 1);
				if(counter >= 4){ /*Informs User on status of log file Execution,prints on terminal and saves lines in log file*/
					printf("%s - attempting to execute %s: %s\n", whattime(), arrray[excuted_file_index], arrray[excuted_file_index+1]);
				}else{
					printf("%s - attempting to execute %s\n", whattime(), arrray[excuted_file_index]);
				}	
				printf("%s - %s %s has been executed with pid %d\n", whattime(), arrray[excuted_file_index], arrray[excuted_file_index+1], getpid());
				close(file);           /*Closes File*/
				
				//redirect to terminal
				
			    dup2(saved_stdout, 1);/*Saves standard output*/
				close(saved_stdout);  /*Saves standard output and closes*/
			}
			
			if(o_state != 1){

				value = execlp(arrray[excuted_file_index], arrray[excuted_file_index+1], NULL);

				/*Informs User on status of log file Execution and prints on terminal*/
				if(value == -1){
					//perror("execlp failed");
					if(counter >= 4){
						printf("%s - could not execute %s %s\n", whattime(), arrray[excuted_file_index], arrray[log_index+1]);	
					}else {
						printf("%s - could not execute %s\n", whattime(), arrray[excuted_file_index]);
					}
					kill(getpid(), SIGKILL); /*Exection unsuccessfull SIGKILL is called to terminate program*/
					//continue;
					
				}
			}
			if(o_state == 1){	
				int saved_stdout_o = dup(1);
				int file1 = open(arrray[o_location+1], O_WRONLY | O_CREAT | O_APPEND, 0777);
				
				if(file1 == -1){
					perror("Failed");
				}				
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
		}
		/*parent*/
		if(pid > 0){
		
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
						if(waitpid(pid, &status, WNOHANG)==0){
							sleep(1);
							get_memory_usage(pid);
							waitflag2 = 1;
						}else if(waitpid(pid, &status, WNOHANG)==-1){
							waitflag2 = 0;
						}else
						{
							waitflag2 = 0;
							
						}	
					}
				}
					
			}

			
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

		int flag;
		flag = check_contents_inside_linked_list(first_head1);
		
		if(arrray[1] != NULL){
			char * send_specific_pid;
			send_specific_pid = print_specific_pid(first_head1, arrray[1]);
			printf("%s\n", send_specific_pid);
			if (send(fd, send_specific_pid, (size_t)&send_of_length_pid, 0) == -1){
				perror("send");	
			}
		}else
		{
			/* code */
			if(flag == 1){
				
				char *send_item;
				send_item = testprint(first_head1);
				
				printf("%s\n", send_item);
				if (send(fd, send_item, (size_t)&send_of_length, 0) == -1){
					perror("send");	
				}	
				// exit(0);
				
				close(fd);	
			}else{
				printf("Nothing is in linked list\n");
			}
		}
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


void find_latest_linked_list(entry_t *first_head){

	entry_t * current_node = first_head;

	

	char * current_pid = malloc(6);
	

	while (current_node != NULL)
	{
		

	
		if(strcmp(whattime(), current_node->pid_elements.time)==0){
			sprintf(current_pid, "%d", current_node->entry_pid);
			printf("%s %ld\n", current_pid, current_node->pid_elements.bytes);
		}

		
		current_node = current_node->next;
	}

	
	
}


void printAll(entry_t *first_head){
	entry_t* current_node = first_head;
	while(current_node != NULL){
		printf("%s %ld %d\n", current_node->pid_elements.time, current_node->pid_elements.bytes, current_node->entry_pid);	
	
		current_node = current_node->next;
	}
}


char * print_specific_pid(entry_t *first_head, char * new_pid){
	
	entry_t* current_node = first_head;
	char * send_values = "";
	int size_of_length = 0;
	char * mypid = malloc(6);
	char bytes_send[256] = "";
	while(current_node != NULL){
		
		sprintf(mypid, "%d", current_node->entry_pid);
		snprintf(bytes_send, sizeof(bytes_send), "%ld", current_node->pid_elements.bytes);
		if(strcmp(mypid, new_pid) == 0){
			printf("%s %ld %d\n", current_node->pid_elements.time, current_node->pid_elements.bytes, current_node->entry_pid);
			size_of_length+=strlen(current_node->pid_elements.time);
			size_of_length++;
			size_of_length+=strlen(bytes_send);
			size_of_length++;
			size_of_length+=strlen(mypid);
			send_of_length_pid = size_of_length;
			send_values = malloc(size_of_length);
			send_values[0] = '\0';
			strcat(send_values, current_node->pid_elements.time);
			strcat(send_values, " ");
			strcat(send_values, bytes_send);
			strcat(send_values, " ");
			strcat(send_values, mypid);
		}		
		current_node = current_node->next;
	}
	return send_values;
}





int check_contents_inside_linked_list(entry_t* first_head) 
{ 
    entry_t* current = first_head;  // Initialize current 
	if(current == NULL){
		return 0;
	}

	return 1;
   
}

int check_contents_inside_linked_list2(entry_t* first_head, pid_t pid_v) 
{ 
    entry_t* current = first_head;  // Initialize current 
	if(current == NULL){
		return 0;
	}
    while (current != NULL) 
    { 
        if (current->entry_pid == pid_v) 
            return 1; 
        current = current->next; 
    } 
    return 0; 
}




char * testprint(entry_t* head){

	entry_t *current_node = head;
	char * send_value = "";
	int size_of_length = 0;
	char * mypid = malloc(6);
	char bytes_send[256] = "";
	char * this_time = whattime();
	
	if(current_node==NULL){
		printf("  Empty \n");
		return NULL;
	}else{
		
		
		while (current_node != NULL){
			
			if(strcmp(current_node->pid_elements.time, this_time)==0){
				sprintf(mypid, "%d", current_node->entry_pid);
				snprintf(bytes_send, sizeof(bytes_send), "%ld", current_node->pid_elements.bytes);
			
				size_of_length += (strlen(mypid));
				size_of_length +=(strlen(bytes_send));
				
				size_of_length ++;
			
				send_value = malloc(size_of_length);
				send_value[0] = '\0';
				send_of_length = size_of_length;
				strcat(send_value, mypid);
				strcat(send_value, " ");
				strcat(send_value, bytes_send);
			
				
			}
			current_node = current_node->next;
		}
	
	free(mypid);

	return send_value;
	}
	

}



/*Executes when Children are Terminated*/
void kill_child(int signum){
	if(!log_state){
		printf("%s - sent SIGTERM to %d\n", whattime(), pidc);   /*Message is printed in the terminal after the children are terminated*/
	}
	if(log_state){
		int saved_stdout = dup(1);
		int file = open(arrray_log, O_WRONLY | O_CREAT | O_APPEND, 0777);				
		if(file == -1){
			perror("Failed");
			//return 2;
		}								
		dup2(file, 1);
		printf("%s - sent SIGTERM to %d\n", whattime(), pidc);   /*Message is printed in the terminal after the children are terminated and saved in log file */
		close(file);
		//redirect to terminal
		dup2(saved_stdout, 1);
		close(saved_stdout);
	}
	
}

/*Terminates program completely when Ctrl-C is input by the user*/
void Kill_All(int sig) {
	if(!log_state){
		printf("\n%s - recieved SIGINT\n", whattime());                  /*As soon as Ctrl-C is Input this Message is printed in the terminal */
		printf("%s - Cleaning up and terminating...\n", whattime());
	}
	if(log_state){
		int saved_stdout = dup(1);
		int file = open(arrray_log, O_WRONLY | O_CREAT | O_APPEND, 0777);				
		if(file == -1){
			perror("Failed");
		}								
		dup2(file, 1);
		printf("\n%s - recieved SIGINT\n", whattime());                 /*As soon as Ctrl-C is Input this Message is printed in the terminal and saved in log file */
		printf("%s - Cleaning up and terminating...\n", whattime());
		close(file);
		//redirect to terminal
		dup2(saved_stdout, 1);
		close(saved_stdout);
	}
	
	/* Clears Dynamic Memmory after SIGINT is signaled*/
	for (int i = 0; i < THREADS_NUM; i++) {
		pthread_cancel(thread_pool[i]);          /*Sends Cancellation request to thread*/
		pthread_kill(thread_pool[i], SIGKILL);   /*Terminates process*/
		pthread_join(thread_pool[i], NULL);
	}

	exit(1);
	// return 0;
}