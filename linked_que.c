#include <stdlib.h>
#include "linked_que.h"


node_t* head = NULL;
node_t* tail = NULL; 

void append_que(int *csocket){
    node_t * node_new = malloc(sizeof(node_t));
    node_new->csocket = csocket;
    node_new->next = NULL;
    if(tail == NULL){
        head = node_new;
    } else{
        tail->next = node_new;
    }
    tail = node_new;
}

int* remove_que(){
    if(head == NULL){
        return NULL;
    } else {
        int *result = head->csocket;
        node_t *temp=head;
        head = head->next;
        if(head == NULL){
            tail = NULL;
        }
        free(temp);
        return result;
    }
}
