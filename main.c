#include "stupidmath.h"

int main(void) {
	char expr[] = "4=2+(2-3)"; // equals -2
	char* res = functionify(expr);
	printf("expression format: %s", res);
	return 0;
}