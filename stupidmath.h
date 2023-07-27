#pragma once
#include <stdlib.h>
#include <stdbool.h>
#include "clinalg.h"
#include "dlinklist.h"

#define free_s(x) free(x); x = NULL

typedef struct {
	char* name;
	long double val;
} vardef;

typedef struct {
	size_t len;
	vardef vars[];
} varmap;

varmap* new_varmap(void);

varmap* push_to_varmap(varmap* vm, char* name, long double val);

void print_varmap(varmap* vm);

typedef struct {
	size_t len;
	DoublyLinkedList* eqns[];
} SystemOfEquations;

char* functionify(char* equation);

varmap* vars(DoublyLinkedList* d);

bool varmap_contains(varmap* vm, char* pat);

long double index_varmap(varmap* vm, char* key);

long double __remaining_soln_error(DoublyLinkedList* postfix, varmap* vars);

long double ddx(DoublyLinkedList* postfix, varmap* vars, char* wrt);

SystemOfEquations* new_system(void);

void push_to_system_vector(SystemOfEquations* s, DoublyLinkedList* d);

void print_system_of_equations(SystemOfEquations* s);

matrix* jacobian(DoublyLinkedList* sys);