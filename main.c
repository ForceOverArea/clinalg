#include "shunting.h"

int main(void) {
	char expr[] = "1 + 2 ^ 3";
	
	long double res = eval_str(expr);

	printf("%Lf\n", res);

	return 0;
}