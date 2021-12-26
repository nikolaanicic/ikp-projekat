#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "queue.h"

void init_queue(Node** head)
{
	*head = NULL;
}


void push(Node** head, Node* new_node)
{
	if (new_node == NULL)
	{
		return;
	}
	else if (*head == NULL)
	{
		*head = new_node;
		return;
	}

	new_node->next = (*head);
	(*head)->next = new_node;
}

void free_node(Node** node)
{
	if (*node == NULL)
		return;

	free(*node);
	*node = NULL;
}


Node pop(Node** head) 
{
	if (*head == NULL)
		return Node();

	Node* tmp = *head;

	while ((*head)->next->next != NULL)
	{
		*head = (*head)->next;
	}

	Node retval = *((*head)->next);
	
	(*head)->next = NULL;
	free_node(&((*head)->next));
	*head = tmp;


	return retval;
}


void print_node(Node* head)
{
	print_message(head->data);
}

void print_queue(Node* head)
{
	if (head == NULL)
	{
		printf("\nList is empty");
	}
	else
	{
		while (head)
		{
			print_node(head);
			head = head->next;
		}
	}
}

void free_queue(Node** head)
{
	if (*head == NULL)
		return;

	while (*head)
	{
		Node* next = (*head)->next;
		free(*head);
		*head = NULL;
		*head = next;
	}
}

Node* get_new_node(MESSAGE* message)
{
	if (message == NULL)
	{
		printf("\n Message was null");
		return NULL;
	}

	Node* new_node = (Node*)malloc(sizeof(Node));

	if (new_node == NULL)
	{
		printf("\nFailed to allocate new node memory");
		return NULL;
	}

	memcpy(&(new_node->data), message, sizeof(MESSAGE));
	new_node->next = NULL;

	return new_node;
}