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

/*
Creates a new varmap pointer, returning NULL if it fails.

A varmap holds key-value pairs of the string-represented 
variables in a system of equations. I.e. binding a variable
`deltaTime` to a long double value `0.0001`.
*/
varmap* new_varmap(void) {
	varmap* res = (varmap*)malloc(sizeof(varmap));
	if (!res) {
		puts("error: insufficient heap memory for new varmap");
		return NULL;
	}
	res->len = 0;
	return res;
}

/*
Pushes a new variable `name` and its value `val` onto the 
variable map. This operation returns NULL if it fails.
*/
varmap* push_to_varmap(varmap* vm, char* name, long double val) {
	
	//printf("pushing val: %Lf...\n", val);
	
	varmap* tmp = (varmap*)realloc(vm, sizeof(varmap) + sizeof(vardef) * (vm->len + 1));
	if (!tmp) {
		puts("error: insufficient heap memory for new varmap");
		return vm;
	}
	vm = tmp;
	vm->vars[vm->len] = (vardef){ name, val };
	vm->len++;
	return vm;
}

/*
Prints a varmap's key-value pairs to stdout.
*/
void print_varmap(varmap* vm) {
	puts("{");
	for (int i = 0; i < vm->len-1; i++)
		printf("  %s : %Lf,\n", vm->vars[i].name, vm->vars[i].val);
	printf("  %s : %Lf\n", vm->vars[vm->len-1].name, vm->vars[vm->len-1].val);
	puts("}");
}

/*
Rearranges an equation algebraically to an expression form 
that should evaluate to 0 when solved. For example, `x = -3`
would become `x - (-3)`, which would evaluate to 0 when 
`x` = -3.
*/
char* functionify(char* equation) {
	
	DoublyLinkedList* res = new_doubly_linked_list();
	if (!res)
		return NULL;

	if (instances(equation, "=") != 1) {
		puts("error: equation must contain exactly one `=` char");
		printf("equation is: %s\n", equation);
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

/*
Returns a DoublyLinkedList of variables in the given 
postfix-formatted DoublyLinkedList. For example, 
vars(`x` <=> `y` <=> `3` <=> `-` <=> `-`) would return
`x` <=> `y`.
*/
varmap* vars(DoublyLinkedList* d) {

	varmap* vm = new_varmap();

	for (snode* tmp = d->head; tmp; tmp = tmp->next) {
	
		long double decimal = strtold(tmp->data, NULL);
		if (!decimal) {
			bool is_zero = true; // assume '0.000...' until proven to be a variable
			if (!strcmp_g_batch(tmp->data, functions) && !strcmp_g_batch(tmp->data, operators)) {
				for (char* strtmp = tmp->data; *strtmp; strtmp++)
					if (*strtmp != '0' && *strtmp != '.')
						is_zero = false; // string is not '0.000...' if it contains anything other than '0' or '.'
			}

			// push a new variable set to 1 to the varmap
			if (!is_zero)
				vm = push_to_varmap(vm, tmp->data, 1);
		}
	}
	//init_varmap(vm);

	return vm;
}

/*
Returns a boolean statement specifying whether a 
variable `pat` exists in a varmap `vm`.
*/
bool varmap_contains(varmap* vm, char* pat) {

	for (int i = 0; i < vm->len; i++) {
		//printf("checking index %d of varmap for variable '%s'...\n", i, pat);
		if (strcmp(vm->vars[i].name, pat) == 0)
			return true;
	}
	return false;
}

/*
Returns the long double value of a string-represented
variable in the given varmap `vm`.
*/
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
	for (snode* tmp = pfcpy->head; tmp; tmp = tmp->next) {
		if (varmap_contains(vars, tmp->data)) {
			char decimal[50];
			long double val = index_varmap(vars, tmp->data);	// get the decimal value
			snprintf(decimal, 50, "%Lf", val);					// put a string version of the decimal in `decimal`

			//puts("pluggin' and chuggin'. Here's ur proof in the pudding...");
			//print_doubly_linked_list(pfcpy);
			tmp->data = decimal;								// assign long double string to node `tmp`
			//print_doubly_linked_list(pfcpy);
			//puts("");
		}	
	}
	//print_doubly_linked_list(pfcpy);
	return postfix_evaluator(pfcpy); // this destroys pfcpy, but not the postfix passed to the function
}

/*
Returns the derivative of an expression w.r.t. the variable `wrt`
*/
long double ddx(DoublyLinkedList* postfix, varmap* vars, char* wrt) {
	
	const long double dx = 1e-3;
	varmap* dvars = new_varmap();
	if (!dvars)
		return 0;

	for (int i = 0; i < vars->len; i++) {
		if (strcmp(vars->vars[i].name, wrt) == 0) {
			//printf("detected %s as wrt.\n", wrt);
			dvars = push_to_varmap(dvars, vars->vars[i].name, vars->vars[i].val + dx);	// push changed variable
		} else
			dvars = push_to_varmap(dvars, vars->vars[i].name, vars->vars[i].val);		// push normal value 
	}

	//print_varmap(dvars);
	long double dres = __remaining_soln_error(postfix, dvars),
		res = __remaining_soln_error(postfix, vars);


	//printf("wrt %s:ddx = %Lf\n\n", wrt, (dres - res)/dx); 
	return ((dres - res) / dx);
}

typedef struct {
	size_t len;
	DoublyLinkedList* eqns[];
} SystemOfEquations;

SystemOfEquations* new_system(void) {
	SystemOfEquations* res = (SystemOfEquations*)malloc(sizeof(SystemOfEquations) + sizeof(DoublyLinkedList*));
	if (!res) {
		puts("error: insufficient heap memory for new system of equations");
		return NULL;
	}
	res->len = 0;
	res->eqns[0] = NULL;
	return res;
}

SystemOfEquations* push_to_system_vector(SystemOfEquations* s, DoublyLinkedList* d) {
	if (!s) {
		puts("error: given system of equations pointer is NULL");
		return s;
	}

	size_t	newlen = s->len + 1,
			newsize = sizeof(SystemOfEquations) + (newlen * sizeof(DoublyLinkedList*));
	SystemOfEquations* tmp = (SystemOfEquations*)realloc(s, newsize);

	if (!tmp) {
		puts("error: insufficient heap memory for new equation in system");
		printf("equation postfix: "); print_doubly_linked_list(d);
		return s;
	}
	s = tmp;

	s->eqns[s->len] = d;
	s->len++;
	return s;
}

void print_system_of_equations(SystemOfEquations* s) {
	puts("{");
	for (DoublyLinkedList* tmp = s->eqns; tmp < s->len; tmp++)
		print_doubly_linked_list(tmp);
	puts("}");
}

matrix* jacobian(DoublyLinkedList* sys) {

	SystemOfEquations* rpn_soe = new_system();
	if (!rpn_soe)
		return NULL;
	for (snode* tmp = sys->head; tmp; tmp = tmp->next) {
		DoublyLinkedList* pf = shunting_yard(words(functionify(tmp->data)));
		rpn_soe = push_to_system_vector(rpn_soe, pf);
	}

	//puts("established system vector correctly");

	varmap* ivars = vars(rpn_soe->eqns[0]);
	//puts("ivars:");
	//print_varmap(ivars);

	if (ivars->len != rpn_soe->len) {
		puts("error: system of equations is improperly constrained. (independent variable issue)");
		printf("DOF: %zu; EQS: %zu\n", ivars->len, rpn_soe->len);
		return NULL;
	}

	//puts("established ivars correctly");

	for (int i = 1; i < rpn_soe->len; i++) {
		//printf("equation index: %d\n", i);
		varmap* tmpvars = vars(rpn_soe->eqns[i]);
		//puts("tmpvars:");
		//print_varmap(tmpvars);

		//printf("len of tmpvars = %zu\n", tmpvars->len);
		if (tmpvars->len != rpn_soe->len) {
			puts("error: system of equations is improperly constrained. (equation variables mismatch)");
			printf("DOF: %zu; EQS: %zu\n", tmpvars->len, rpn_soe->len);
			return NULL;
		}
		for (int j = 0; j < tmpvars->len; j++) {
			if (!varmap_contains(ivars, tmpvars->vars[j].name)) {
				puts("error: found extra variable in system");
				return NULL;
			}
		}
		free_s(tmpvars);
		tmpvars = NULL;
	}

	//puts("checked constraints... no issues");

	// if function reaches this point, system should be NxN
	matrix* jacobian = new_nxn(rpn_soe->len);
	for (int i = 0; i < rpn_soe->len; i++) {
		for (int j = 0; j < ivars->len; j++) {
			//printf("evaluating ddx for eqn %d w.r.t. %s...\n", i, ivars->vars[j].name);
			//printf("equation is: "); print_doubly_linked_list(rpn_soe->eqns[i]);
			jacobian->data[i]->data[j] = ddx(
				rpn_soe->eqns[i], 
				ivars,
				ivars->vars[j].name
			);
		}
	}

	//puts("created matrix successfully");

	return jacobian;
}