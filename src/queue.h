#ifndef OS_LAB_3_QUEUE_H
#define OS_LAB_3_QUEUE_H

#include <stdbool.h>
#include <stdlib.h>

struct node {
	 int val;
	struct node * son;
	struct node * bro; 
};
 
typedef struct node Tree;


typedef struct QueueItem queue_item;
struct QueueItem {
    struct QueueItem* next;
    struct QueueItem* prev;
    Tree* value;
};

typedef struct Queue queue;
struct Queue {
    queue_item* head;
    queue_item* tail;
    size_t size;
};

void q_init(queue* q);
Tree* q_top(queue* q);
Tree* q_pop(queue* q);
size_t q_size(queue* q);
void q_push(queue* q, Tree* elem);
bool q_empty(queue* q);
void q_destroy(queue* q);

#endif 
