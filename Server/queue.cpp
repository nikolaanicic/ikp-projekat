#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "queue.h"


/*
	Ova funkcija inicijalizuje glavu reda
*/
void init_queue(Node** head)
{
	*head = NULL;
}


/*
	Ova funkcija dodaje novi cvor na kraj reda
	Dodavanje se vrsi na pocetak liste

*/
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
	(*head) = new_node;
}



/*
	Ova funkcija oslobadja jedan cvor jednostruke liste

*/
void free_node(Node** node)
{
	if (*node == NULL)
		return;

	free(*node);
	*node = NULL;
}


/*
	Ova funkcija preuzima cvor iz liste i brise ga
	Preuzimanje cvora se vrsi sa kraja liste da bi ona funkcionisala kao red jer se dodavanja vrsi na pocetak
	
	Preuzimanje se vrsi iterativno kako ne bi doslo do prelazenja memorije steka pri radu sa velikim listama

	Povratna vrednost:
		cvor sa reda ili prazan cvor ako je head NULL

*/

Node pop(Node** head) 
{
	if (*head == NULL)
		return Node();

	Node* tmp = *head;
	if ((*head)->next == NULL)
	{
		Node retval = **head;
		free_node(head);
		return retval;
	}

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


/*
	Ova funkcija ispisuje cvor 

*/

void print_node(Node* head)
{
	print_message(head->data);
}


/*
	Ova funkcija ispisuje celu listu
*/

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


/*
	Ova funkcija brise ceo queue

*/
void free_queue(Node** head)
{
	if (*head == NULL)
		return;

	while (*head != NULL)
	{
		Node* next = (*head)->next;
		free(*head);
		*head = NULL;
		*head = next;
	}
}


/*
	Ova funkcija alocira novi cvor za queue i ubacuje poruku u njega

	Povratna vrednost:
		pokazivac na novi cvor liste ili NULL

*/

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