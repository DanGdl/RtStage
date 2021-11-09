/*
 * linked_list.c
 *
 *  Created on: Nov 24, 2020
 *      Author: max
 */
#include "queue.h"
#include <stdlib.h>

Queue_t* create_queue() {
	Queue_t *l = (Queue_t*) malloc(sizeof(Queue_t));
	l -> head = NULL;
	l -> last = NULL;
	l -> size = 0;
	return l;
}

int size(const Queue_t *list){
	return list -> size;
}

Node_t* create_node(int data, Node_t *prev) {
	Node_t *node = (Node_t*) malloc(sizeof(Node_t));
	node -> next = NULL;
	node -> prev = prev;
	node -> data = data;
	return node;
}

void add(Queue_t *list, int data) {
	if (list -> head == NULL) {
		Node_t *node = create_node(data, NULL);

		list -> head = node;
		list -> last = node;
	} else {
		Node_t *node = create_node(data, list -> last);

		list -> last -> next = node;
		list -> last = node;
	}
	list -> size += 1;
}


void clear(Queue_t *list) {
	Node_t *node = list -> last;
	while (node != NULL) {
		Node_t *tmp = node;
		node = node -> prev;

		if (tmp != NULL) {
			free(tmp);
		}

		if (node != NULL) {
			node -> next = NULL;
		}
		list -> last = node;
	}
	list -> head = NULL;
	list -> last = NULL;
	list -> size = 0;
}

int remove_last(Queue_t *list) {
	Node_t * node = list -> last;
	int data = node -> data;
	list -> last = node -> prev;
	if (list -> last == NULL) {
		list -> head = NULL;
	} else {
		list -> last -> next = NULL;
	}
	// node -> data = NULL;
	free(node);
	list -> size -= 1;
	return data;
}

