#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "clinalg.h"
#include "shunting.h"
#include "dlinklist.h"
#include "stringmanip.h"

/*
`Safe` free

Frees the pointer at `x` and sets its value to NULL
*/
#define free_s(x) free(x); x = NULL

typedef struct {
	char* name;
	long double val;
} vardef;

typedef struct {
	size_t len;
	vardef vars[];
} varmap;

varmap* new_varmap(void) {
	varmap* res = (varmap*)malloc(sizeof(varmap));
	if (!res) {
		puts("error: insufficient heap memory for new varmap");
		return NULL;
	}
	res->len = 0;
	return res;
}

void push_to_varmap(varmap* vm, char* name, long double val) {
	varmap* tmp = (varmap*)realloc(vm, sizeof(vardef) * (vm->len + 1));
	if (!tmp) {
		puts("error: insufficient heap memory for new varmap");
		return NULL;
	}
	vm = tmp;
	vardef new = { name, val };
	vm->vars[vm->len] = new;
	vm->len++;
}

char* functionify(char* equation) {
	
	DoublyLinkedList* res = new_doubly_linked_list();
	if (!res)
		return NULL;

	if (instances(equation, "=") != 1) {
		puts("error: equation must contain exactly one `=` char");
		return NULL;
	}

	// setup for strtok_s...
	char* ctx = NULL;
	char delim[] = "=";

	// gather the lhs, rhs...
	char* token = strtok_s(equation, delim, &ctx);
	char* lhs = token;
	token = strtok_s(NULL, delim, &ctx);
	char* rhs = token;

	// malloc a new char* for the lhs, rhs, & extra chars...
	// `%s-(%s)\0`
	//    12  34
	size_t totlen = (long long)strlen(lhs) + (long long)strlen(rhs) + (size_t)4;
	char* expr = (char*)malloc(sizeof(char) * totlen);
	if (!expr) {
		puts("error: insufficient heap memory for rearranged expression");
		return NULL;
	}

	expr[totlen - 1] = '\0';
	char* restmp = expr;
	for (char* tmp = lhs; *tmp; tmp++) {
		*restmp = *tmp;
		restmp++;
	}

	*restmp = '-';
	restmp++;
	*restmp = '(';
	restmp++;

	for (char* tmp = rhs; *tmp; tmp++) {
		*restmp = *tmp;
		restmp++;
	}
	*restmp = ')';

	return expr;
}

varmap* vars(DoublyLinkedList* d) {

	varmap* vm = new_varmap();

	for (snode* tmp = d; tmp; tmp = tmp->next) {
	
		long double decimal = strtold(tmp->data, NULL);
		if (!decimal) {

			bool not_zero = false;
			for (char* strtmp = tmp->data; *strtmp; strtmp++) {
				if (strcmp(*strtmp, "0") != 0 || strcmp(*strtmp, ".") != 0)
					not_zero = true;
			}

			// push a new variable set to 1 to the varmap
			if (!not_zero)
				push_to_varmap(vm, tmp->data, 1);
		}
	}

	return vm;
}

bool varmap_contains(varmap* vm, char* pat) {
	for (int i = 0; i < vm->len; i++) {
		if (strcmp(vm->vars[i].name, pat) == 0)
			return true;
	}
	return false;
}

long double index_varmap(varmap* vm, char* key) {
	for (int i = 0; i < vm->len; i++) {
		if (strcmp(vm->vars[i].name, key) == 0) {
			return vm->vars[i].val;
		}
	}
	return 0;
}

/*
Returns the error in an equation for a given attempted solution
*/
long double __remaining_soln_error(DoublyLinkedList* postfix, varmap* vars) {
	DoublyLinkedList* pfcpy = copy_doubly_linked_list(postfix);
	for (snode* tmp = pfcpy; tmp; tmp = tmp->next) {
		if (varmap_contains(vars, tmp->data)) {
			char decimal[50];
			long double val = index_varmap(vars, tmp->data);	// get the decimal value
			snprintf(decimal, 50, "%Lf", val);					// put a string version of the decimal in `decimal`

			tmp->data = decimal;								// assign long double string to node `tmp`
		}	
	}
	return postfix_evaluator(pfcpy);
}

/*
Returns the derivative of an expression w.r.t. the variable `wrt`
*/
long double ddx(DoublyLinkedList* postfix, varmap* vars, char* wrt) {
	
	const long double dx = 1e-6;

	varmap* dvars = new_varmap();
	if (!dvars)
		return 0;

	for (int i = 0; i < vars->len; i++) {
		if (strcmp(vars->vars[i].name, wrt) == 0)
			push_to_varmap(dvars, vars->vars[i].name, vars->vars[i].val + dx);	// push changed variable
		else
			push_to_varmap(dvars, vars->vars[i].name, vars->vars[i].val);		// push normal value
	}

	long double res = __remaining_soln_error(postfix, vars),
		dres = __remaining_soln_error(postfix, dvars);

	return (dres - res) / dx;
}

typedef struct {
	size_t len;
	DoublyLinkedList* eqns[];
} system;

system* new_system(void) {
	system* res = (system*)malloc(sizeof(system));
	if (!res) {
		puts("error: insufficient heap memory for new system of equations");
		return NULL;
	}
	res->len = 0;
	return res;
}

void push_to_system_vector(system* s, DoublyLinkedList* d) {
	system* tmp = (system*)realloc(s, sizeof(system) + sizeof(DoublyLinkedList*) * (s->len + 1));
	if (!tmp) {
		puts("error: insufficient heap memory for new equation in system");
		return;
	}
	s = tmp;
	s->eqns[s->len] = d;
	s->len++;
}

matrix* jacobian(DoublyLinkedList* sys) {

	system* rpn_soe = new_system();
	if (!rpn_soe)
		return NULL;

	for (snode* tmp = sys->head; tmp; tmp = tmp->next) {
		push_to_system_vector(
			rpn_soe, 
			shunting_yard(words(functionify(tmp)))
		);
	}

	varmap* ivars = vars(rpn_soe->eqns[0]);

	if (ivars->len != rpn_soe->len) {
		puts("error: system of equations is improperly constrained");
		return NULL;
	}

	for (int i = 1; i < rpn_soe->len; i++) {
		varmap* tmpvars = vars(rpn_soe->eqns[i]);
		if (tmpvars->len != rpn_soe->len) {
			puts("error: system of equations is improperly constrained");
			return NULL;
		}
		for (int j = 0; j < tmpvars->len; j++) {
			if (!varmap_contains(ivars, tmpvars->vars[j].name)) {
				puts("error: found extra variable in system");
				return NULL;
			}
		}
		free_s(tmpvars);
	}

	// if function reaches this point, system should be NxN
	matrix* jacobian = new_nxn(rpn_soe->len);
	for (int i = 0; i < rpn_soe->len; i++) {
		for (int j = 0; j < ivars->len; j++) {
			jacobian->data[i]->data[j] = ddx(
				rpn_soe->eqns[i], 
				ivars,
				ivars->vars[j].name
			);
		}
	}

	return jacobian;
}