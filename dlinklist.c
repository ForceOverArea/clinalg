#include <stdio.h>
#include <stdlib.h>

struct __snode {
	struct __snode* next;
	struct __snode* prev;
	char* data;
};

typedef struct __snode snode;

typedef struct {
	void* head;
	void* last;
} DoublyLinkedList;

/*
Constructs a new DoublyLinkedList pointer with no elements.
*/
DoublyLinkedList* new_doubly_linked_list(void) {
	DoublyLinkedList* d = (DoublyLinkedList*)malloc(sizeof(DoublyLinkedList));
	if (d != NULL) {
		d->head = NULL;
		d->last = NULL;
	}
	return d;
}

snode* __new_snode(char* string) {
	snode* n = (snode*)malloc(sizeof(snode));
	if (n != NULL) {
		n->next = NULL;
		n->prev = NULL;
		n->data = string;
	}
	return n;
}

/*
Pushes a new string value onto the doubly linked list.
*/
void push_to_doubly_linked_list(DoublyLinkedList* d, char* string) {
	snode* n = __new_snode(string);
	if (n == NULL) {
		puts("error: insufficient heap memory for new node");
		return;
	}

	if (d->head == NULL) {
		d->head = n;
		d->last = n;

	}
	else {
		n->next = d->head;					// next element is the old head
		((snode *)d->head)->prev = n;		// head prev is new node
		d->head = n;						// new list head is new node
	}
}

/*
Pushes a new string value to the END of the doubly linked list.
*/
void push_back_to_doubly_linked_list(DoublyLinkedList* d, char* string) {
	snode* n = __new_snode(string);
	if (n == NULL) {
		puts("error: insufficient heap memory for new node");
		return;
	}

	if (d->last == NULL) {
		d->last = n;
		d->head = n;

	}
	else {
		snode* old = d->last;
		d->last = n;			// push onto back of list
		n->prev = old;			// make prev element the old list last
		old->next = n;			// make the old list last's next the new node
	}
}

/*
Removes the first item from a doubly linked list, returning its contained value.
*/
char* pop_from_doubly_linked_list(DoublyLinkedList* d) {

	// get the head and move it to the next node
	snode* resnode = d->head;
	d->head = ((snode *)d->head)->next;
	if (d->head)
		((snode *)d->head)->prev = NULL;

	// get the important data from the node
	char* res = resnode->data;

	// free the node struct pointer and destroy the old data
	free(resnode);
	resnode = NULL;

	return res;
}

/*
Removes the last item from a doubly linked list, returning its contained value.
*/
char* pop_back_from_doubly_linked_list(DoublyLinkedList* d) {

	// get the head and move it to the next node
	snode* resnode = d->last;
	d->last = ((snode *)d->last)->prev;
	if (d->last)
		((snode *)d->last)->next = NULL;

	// get the important data from the node
	char* res = resnode->data;

	// free the node struct pointer and destroy the old data
	free(resnode);
	resnode = NULL;

	return res;
}

/*
Prints all the values in a doubly linked list from first to last.
*/
void print_doubly_linked_list(DoublyLinkedList* d) {
	snode* node = d->head;

	while (node) {
		printf("%s <=> ", node->data);
		node = node->next;
	}
	puts("NULL");
}

/*
Frees all memory tied up in a doubly linked list.
*/
void destroy_doubly_linked_list(DoublyLinkedList* d) {
	snode* node = d->head;

	while (node) {

		// get a temporary variable for the node and increment the pointer
		snode* tmp = node;
		node = node->next;

		// free the temp pointer and set to NULL for safety
		free(tmp);
		tmp = NULL;
	}

	free(d);
	d = NULL;
}

struct __ldnode {
	struct __ldnode* next;
	struct __ldnode* prev;
	long double data;
};

typedef struct __ldnode ldnode;

ldnode* __new_ldnode(long double value) {
	ldnode* n = (ldnode*)malloc(sizeof(ldnode));
	if (n != NULL) {
		n->next = NULL;
		n->prev = NULL;
		n->data = value;
	}
	return n;
}

/*
Pushes a new long double value onto the doubly linked list.
*/
void push_to_doubly_linked_list_ld(DoublyLinkedList* d, long double value) {
	snode* n = __new_ldnode(value);
	if (n == NULL) {
		puts("error: insufficient heap memory for new node");
		return;
	}

	if (d->head == NULL) {
		d->head = n;
		d->last = n;

	}
	else {
		n->next = d->head;					// next element is the old head
		((ldnode*)d->head)->prev = n;		// head prev is new node
		d->head = n;						// new list head is new node
	}
}

/*
Pushes a new long double value to the END of the doubly linked list.
*/
void push_back_to_doubly_linked_list_ld(DoublyLinkedList* d, long double value) {
	ldnode* n = __new_ldnode(value);
	if (n == NULL) {
		puts("error: insufficient heap memory for new node");
		return;
	}

	if (d->last == NULL) {
		d->last = n;
		d->head = n;

	}
	else {
		ldnode* old = d->last;
		d->last = n;			// push onto back of list
		n->prev = old;			// make prev element the old list last
		old->next = n;			// make the old list last's next the new node
	}
}

/*
Removes the first item from a doubly linked list, returning its contained value.
*/
long double pop_from_doubly_linked_list_ld(DoublyLinkedList* d) {

	// get the head and move it to the next node
	ldnode* resnode = d->head;
	d->head = ((ldnode*)d->head)->next;
	if (d->head)
		((ldnode*)d->head)->prev = NULL;

	// get the important data from the node
	long double res = resnode->data;

	// free the node struct pointer and destroy the old data
	free(resnode);
	resnode = NULL;

	return res;
}

/*
Removes the last item from a doubly linked list, returning its contained value.
*/
long double pop_back_from_doubly_linked_list_ld(DoublyLinkedList* d) {

	// get the head and move it to the next node
	ldnode* resnode = d->last;
	d->last = ((ldnode*)d->last)->prev;
	if (d->last)
		((ldnode*)d->last)->next = NULL;

	// get the important data from the node
	long double res = resnode->data;

	// free the node struct pointer and destroy the old data
	free(resnode);
	resnode = NULL;

	return res;
}

/*
Prints all the values in a doubly linked list from first to last.
*/
void print_doubly_linked_list_ld(DoublyLinkedList* d) {
	ldnode* node = d->head;

	while (node) {
		printf("%Lf <=> ", node->data);
		node = node->next;
	}
	puts("NULL");
}