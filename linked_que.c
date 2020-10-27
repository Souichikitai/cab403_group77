#include <stdlib.h>
#include "linked_que.h"
#include <stdio.h>

node_t* head = NULL;
node_t* tail = NULL; 

int counter =0;


/*Appending a node to linked list*/
void append_que(int *c_socket){
    node_t * node_new = malloc(sizeof(node_t));
    node_new->c_socket = c_socket;
    node_new->next = NULL;
    if(tail == NULL){
        head = node_new;
    } else{
        tail->next = node_new;
    }
    tail = node_new;
    counter++;
}

/*Deleting a node from the linked list*/
int* remove_que(){
    if(head == NULL){
        return NULL;
    } else {
        int *result = head->c_socket;
        node_t *temp=head;
        head = head->next;
        if(head == NULL){
            tail = NULL;
        }
        free(temp);

        return result;
    }
}
