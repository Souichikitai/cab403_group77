#ifndef LINKED_QUE_
#define LINKED_QUE_

struct node{
    struct node *next;
    int *c_socket;
};

typedef struct node node_t;

void append_que(int *c_socket);
int* remove_que();

#endif