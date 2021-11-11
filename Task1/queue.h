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
	void* data;
} Node_t;

typedef struct Queue {
	struct Node *head;
	struct Node *last;
	unsigned int size;
} Queue_t;

Queue_t* queue_create();

int queue_size(const Queue_t *queue);

void queue_add(Queue_t *queue, void* data);

void* queue_remove(Queue_t *queue, Node_t* data);

void queue_clear(Queue_t *queue);

void* remove_last(Queue_t *queue);

#endif
