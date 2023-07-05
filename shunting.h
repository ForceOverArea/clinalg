#pragma once
#include <stdbool.h>
#include "dlinklist.h"

bool strcmp_g(char* s1, char* s2);

DoublyLinkedList* words(char* expr);

DoublyLinkedList* shunting_yard(DoublyLinkedList* infix);

long double postfix_evaluator(DoublyLinkedList* rpn);

long double eval_str(char* expr);