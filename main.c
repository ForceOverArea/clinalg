#include <stdio.h>
#include "stupidmath.h"
#include "clinalg.h"
#include "shunting.h"

void sub(void) {
	char expr2[] = "i+j=4";

	DoublyLinkedList* sys = new_doubly_linked_list();
	//push_to_doubly_linked_list(sys, expr1);
	push_to_doubly_linked_list(sys, expr2);

	print_doubly_linked_list(sys);

	matrix* j = jacobian(sys);

	if (j)
		print_matrix(j);
	else
		puts("jacobian construction failed...");
}

int main(void) {

	char expr1[] = "i-j=9";
	
	DoublyLinkedList* d = shunting_yard(words(functionify(expr1)));

	varmap* vm = vars(d);

	long double err = __remaining_soln_error(d, vm),
		dydx = ddx(d, vm, "j");

	printf("%Lf\n%Lf\n", err, dydx);

	return 0;
}