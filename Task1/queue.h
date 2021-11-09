/*
 * linked_list.h
 *
 *  Created on: Nov 23, 2020
 *      Author: max
 */
#ifndef QUEUE
#define QUEUE

typedef struct Node {
	struct Node *next;
	struct Node *prev;
	int data;
} Node_t;

typedef struct Queue {
	struct Node *head;
	struct Node *last;
	unsigned int size;
} Queue_t;

Queue_t* create_queue();

int size(const Queue_t *list);

void add(Queue_t *list, int data);

void clear(Queue_t *list);

int remove_last(Queue_t *list);

#endif
