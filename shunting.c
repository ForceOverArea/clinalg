#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "dlinklist.h"
#include "stringmanip.h"

/*
Returns a doubly linked list of the substrings 
delimited by spaces in a given string.
*/
DoublyLinkedList* words(char* expr) {
	DoublyLinkedList* res = new_doubly_linked_list();
	if (res == NULL)
		return NULL;

	char*	spaced = replace(expr, "(", " ( ");
			spaced = replace(spaced, ")", " ) ");
			spaced = replace(spaced, "+", " + ");
			spaced = replace(spaced, "-", " - ");
			spaced = replace(spaced, "*", " * ");
			spaced = replace(spaced, "/", " / ");
			spaced = replace(spaced, "^", " ^ ");
			spaced = replace(spaced, ",", " , ");

	char* ctx = NULL;							// initialize context variable
	char delim[] = " ";
	char* token = strtok_s(expr, delim, &ctx);	// initialize first token

	while (token) {
		push_back_to_doubly_linked_list(res, token);	// push to dll if not null
		token = strtok_s(NULL, delim, &ctx);			// get next token value
	}

	return res;
}

/*
Grant's string comparison function

same as strcmp in "string.h", but allows room for
NULLptr checks prior to calling strcmp.
*/
bool strcmp_g(char* s1, char* s2) {

	if (s1 == NULL || s2 == NULL)
		return false;

	if (strcmp(s1, s2) != 0)
		return false;

	return true;
}

/*
See definition for strcmp_g

This function does the same operation, but on all
elements of the NULL-TERMINATED char* array 
(i.e. char**), returning true if any of those 
strings are a match for the first argument.
*/
bool strcmp_g_batch(char* str, char** strs) {
	while (*strs) {
		if (strcmp_g(str, *strs))
			return true;
		strs++;
	}
	return false;
}

/*
Returns the precedence of the operator passed to the function.
*/
unsigned prec(char* op) {
	const char* add[] = { "+", "-", NULL };
	const char* mul[] = { "*", "/",	NULL };
	const char* exp[] = { "^",		NULL };
	
	if (strcmp_g_batch(op, add))
		return 2;

	else if (strcmp_g_batch(op, mul))
		return 3;

	else if (strcmp_g_batch(op, exp))
		return 4;

	else
		return 0;
}

const char* operators[] = { "+", "-", "*", "/", "^", NULL };
const char* functions[] = {
	"sin",		"cos",		"tan",
	"arcsin",	"arccos",	"arctan",
	"log",		"ln",		"sqrt",
	"exp",
	"(",
	NULL
};

/*
Shunting yard algorithm for converting an expression in infix notation
to an equivalent expression in postfix notation (i.e. reverse polish notation).
The postfix expression can then be evaluated to a numerical value via a postfix
stack evaluator algorithm.
*/
DoublyLinkedList* shunting_yard(DoublyLinkedList* infix) {

	DoublyLinkedList* stack = new_doubly_linked_list(); // only push/pop
	DoublyLinkedList* queue = new_doubly_linked_list(); // only push BACK/pop

	while (infix->head) { // "while there are tokens to be read..."
		char* token = pop_from_doubly_linked_list(infix);
		//printf("token: %s\n", token);

		//printf("infix: ");
		//print_doubly_linked_list(infix);

		//printf("stack: ");
		//print_doubly_linked_list(stack);

		//printf("queue: ");
		//print_doubly_linked_list(queue);

		// "if the token is an operator..."
		if ( strcmp_g_batch(token, operators) ) {
			//puts("operator");
			char* o1 = token;
			char* o2 = stack->head;
			bool prec_check = (
				// "o2 has greater precedence than o1 or 
				// (o1 and o2 have the same precedence and 
				// o1 is left associative)"
				prec(o2) > prec(o1) || ( prec(o1) == prec(o2) && o1 != "^" )
			);
				
			while (o2 && !strcmp_g(o2, "(") && prec_check) {
				push_back_to_doubly_linked_list(
					queue,
					pop_from_doubly_linked_list(stack)
				);
			}
			push_to_doubly_linked_list(stack, token);
		}

		// "if the token is a function or left parenthesis..."
		else if ( strcmp_g_batch(token, functions) ) {
			//puts("function");
			push_to_doubly_linked_list(stack, token);
		}

		// "if the token is a comma..."
		else if ( strcmp(token, ",") == 0 ) {
			//puts("comma");
			while (stack->head && stack->head->data != "(") {
				push_back_to_doubly_linked_list(
					queue,
					pop_from_doubly_linked_list(stack)
				);
			}
		}

		// "if the token is a right parenthesis..."
		else if ( strcmp(token, ")") == 0 ) {
			//puts("r. parenthesis");
			while (stack->head != NULL && strcmp(stack->head->data, "(") != 0) {
				//printf("%s\n", stack->head->data);
				push_back_to_doubly_linked_list(
					queue,
					pop_from_doubly_linked_list(stack)
				);
			}
			// stack->head->data == "("
			pop_from_doubly_linked_list(stack);									// discard left parenthesis
			if (stack->head != NULL && strcmp_g_batch(stack->head->data, functions)) {	// move any following function call to the queue					
				push_back_to_doubly_linked_list(
					queue,
					pop_from_doubly_linked_list(stack)
				);
			}
		}

		// "if the token is a number..."
		else {
			//puts("number");
			push_back_to_doubly_linked_list(queue, token);
		}
	}

	while (stack->head != NULL) {
		if ( stack->head->data && // extra check to prevent warning from VS
			strcmp(stack->head->data, "(") == 0 ) {
			
			puts("error: extra parenthesis found in operator stack... aborting shunting yard algorithm");
			// free all malloced memory...
			destroy_doubly_linked_list(stack);
			destroy_doubly_linked_list(queue);
			destroy_doubly_linked_list(infix);
			return NULL;
		}
		push_back_to_doubly_linked_list(
			queue,
			pop_from_doubly_linked_list(stack)
		);
	}

	destroy_doubly_linked_list(infix);
	destroy_doubly_linked_list(stack);

	return queue;
}

long double postfix_evaluator(DoublyLinkedList* rpn) {
	
	DoublyLinkedList* stack = new_doubly_linked_list();
	
	while (rpn->head) {

		char* token = pop_from_doubly_linked_list(rpn);

		// token is a function
		if (strcmp_g_batch(token, functions)) {
			long double value = pop_from_doubly_linked_list_ld(stack);

			if (strcmp_g(token, "sin")) {
				push_to_doubly_linked_list_ld(stack, sinl(value));
			}

			else if (strcmp_g(token, "cos")) {
				push_to_doubly_linked_list_ld(stack, cosl(value));
			}

			else if (strcmp_g(token, "tan")) {
				push_to_doubly_linked_list_ld(stack, tanl(value));
			}

			else if (strcmp_g(token, "arcsin")) {
				push_to_doubly_linked_list_ld(stack, asinl(value));
			}

			else if (strcmp_g(token, "arccos")) {
				push_to_doubly_linked_list_ld(stack, acosl(value));
			}

			else if (strcmp_g(token, "arctan")) {
				push_to_doubly_linked_list_ld(stack, atanl(value));
			}

			else if (strcmp_g(token, "log")) {
				push_to_doubly_linked_list_ld(stack, log10l(value));
			}

			else if (strcmp_g(token, "ln")) {
				push_to_doubly_linked_list_ld(stack, logl(value));
			}

			else if (strcmp_g(token, "sqrt")) {
				push_to_doubly_linked_list_ld(stack, sqrtl(value));
			}

			else if (strcmp_g(token, "exp")) {
				push_to_doubly_linked_list_ld(stack, expl(value));
			}

			else if (strcmp_g(token, "(")) {
				puts("error: found left parenthesis in postfix stack. aborting postfix evaluation...");
				return (long double)NAN;
			}

			else {
				printf("error: found unknown function '%s'. aborting postfix evaluation...\n", token);
				return (long double)NAN;
			}

		}

		// token is an operator/binary function
		else if (strcmp_g_batch(token, operators)) {
			long double second_val = pop_from_doubly_linked_list_ld(stack);
			long double first_val = pop_from_doubly_linked_list_ld(stack);

			switch (*token) {
			case '+':
				push_to_doubly_linked_list_ld(stack, first_val + second_val);
				break;
			case '-':
				push_to_doubly_linked_list_ld(stack, first_val - second_val);
				break;
			case '*':
				push_to_doubly_linked_list_ld(stack, first_val * second_val);
				break;
			case '/':
				push_to_doubly_linked_list_ld(stack, first_val / second_val);
				break;
			case '^':
				push_to_doubly_linked_list_ld(stack, powl(first_val, second_val));
				break;
			default:
				printf("error: found unknown binary operator: '%c'. aborting postfix evaluation...\n", *token);
				return (long double)NAN;
				break;
			}
		}

		// token is a number (probably)
		else {
			long double value = strtold(token, NULL);
			push_to_doubly_linked_list_ld(stack, value);
		}
	}

	long double res = pop_from_doubly_linked_list_ld(stack);

	if (stack->head != NULL) {
		puts("error: leftover items in postfix stack. aborting postfix evaluation...");
		puts("error: leftover items in postfix stack. aborting postfix evaluation...");
	}
}

long double eval_str(char* expr) {

	return postfix_evaluator(
		shunting_yard(
			words(
				expr
			)
		)
	);
}