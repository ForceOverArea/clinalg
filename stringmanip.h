#pragma once
#include <stdlib.h>
#include <stdbool.h>

bool __strcmp_g_inplace(char* start, char* target);

size_t instances(char* string, char* match);

char* replace(char* string, char* from, char* to);