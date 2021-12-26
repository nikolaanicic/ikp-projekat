#ifndef _QUEUE_H
#define _QUEUE_H

#include "data_model.h"

typedef struct _queue_node_
{
	MESSAGE data;
	struct _queue_node_* next;
}Node;


void push(Node** head, Node* new_node);
Node pop(Node** head);
void print_queue(Node* head);
void free_queue(Node** head);
void init_queue(Node** head);
Node* get_new_node(MESSAGE* message);

#endif
