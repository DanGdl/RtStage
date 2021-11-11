/*
 * linked_list.c
 *
 *  Created on: Nov 24, 2020
 *      Author: max
 */
#include "queue.h"
#include <stdlib.h>

Queue_t* queue_create() {
	Queue_t* l = (Queue_t*) malloc(sizeof(Queue_t));
	l -> head = NULL;
	l -> last = NULL;
	l -> size = 0;
	return l;
}

int queue_size(const Queue_t *queue){
	return queue -> size;
}

Node_t* create_node(void* data, Node_t *prev) {
	Node_t* node = (Node_t*) malloc(sizeof(Node_t));
	node -> next = NULL;
	node -> prev = prev;
	node -> data = data;
	return node;
}

void queue_add(Queue_t *queue, void* data) {
    Node_t* last_node = NULL;
    if (queue -> head != NULL) {
        last_node = queue -> last;
    }
    Node_t* node = create_node(data, last_node);
	if (queue -> head == NULL) {
		queue -> head = node;
	} else {
		queue -> last -> next = node;
	}
	queue -> last = node;
	queue -> size += 1;
}

void* queue_remove(Queue_t *queue, Node_t* node) {
    Node_t* current = queue -> head;
    while (current != NULL && current != node) {
        current = current -> next;
    }
    Node_t* prev = current -> prev;
    Node_t* next = current -> next;
    if (prev != NULL) {
    	prev -> next = next;
    }
    if (next != NULL) {
    	next -> prev = prev;
    }
    if (current == queue -> last) {
		queue -> last = prev;
	}
    if (current == queue -> head) {
		queue -> head = next;
	}
    queue -> size -= 1;
    void* data = current -> data;
    current -> next = NULL;
    current -> prev = NULL;
    current -> data = NULL;
    free(current);
    return data;
}

void queue_clear(Queue_t* queue) {
	Node_t* node = queue -> last;
	while (node != NULL) {
		Node_t *tmp = node;
		node = node -> prev;

		if (tmp != NULL) {
			free(tmp);
		}

		if (node != NULL) {
			node -> next = NULL;
		}
		queue -> last = node;
	}
	queue -> head = NULL;
	queue -> last = NULL;
	queue -> size = 0;
}

void* remove_last(Queue_t* queue) {
	Node_t* node = queue -> last;
	void* data = node -> data;
	queue -> last = node -> prev;
	if (queue -> last == NULL) {
		queue -> head = NULL;
	} else {
		queue -> last -> next = NULL;
	}
	node -> data = NULL;
	free(node);
	queue -> size -= 1;
	return data;
}
