#pragma once

struct __snode {
	struct __snode* next;
	struct __snode* prev;
	char* data;
};

typedef struct __snode snode;

typedef struct {
	snode* head;
	snode* last;
} DoublyLinkedList;

DoublyLinkedList* new_doubly_linked_list(void);

snode* __new_snode(char* string);

void push_to_doubly_linked_list(DoublyLinkedList* d, char* string);

void push_back_to_doubly_linked_list(DoublyLinkedList* d, char* string);

char* pop_from_doubly_linked_list(DoublyLinkedList* d);

char* pop_back_from_doubly_linked_list(DoublyLinkedList* d);

void print_doubly_linked_list(DoublyLinkedList* d);

void destroy_doubly_linked_list(DoublyLinkedList* d);

DoublyLinkedList* copy_doubly_linked_list(DoublyLinkedList* d);

struct __ldnode {
	struct __ldnode* next;
	struct __ldnode* prev;
	long double data;
};

typedef struct __ldnode ldnode;

ldnode* __new_ldnode(long double value);

void push_to_doubly_linked_list_ld(DoublyLinkedList* d, long double value);

void push_back_to_doubly_linked_list_ld(DoublyLinkedList* d, long double value);

long double pop_from_doubly_linked_list_ld(DoublyLinkedList* d);

long double pop_back_from_doubly_linked_list_ld(DoublyLinkedList* d);

void print_doubly_linked_list_ld(DoublyLinkedList* d);

