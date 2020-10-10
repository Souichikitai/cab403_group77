#ifndef LINKED_QUE_
#define LINKED_QUE_

struct node{
    struct node *next;
    int *csocket;
};

typedef struct node node_t;

void append_que(int *csocket);
int* remove_que();

#endif