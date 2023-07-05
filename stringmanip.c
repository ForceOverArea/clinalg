#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

bool __strcmp_g_inplace(char* start, char* target) {
	if (start == NULL || target == NULL)
		return false;

	while (*target) {

		if (*start && *start != *target)
			return false;

		target++;
		start++;
	}
	return true;
}

size_t instances(char* string, char* match) {
	size_t res = 0;
	while (*string) {
		if (__strcmp_g_inplace(string, match))
			res++;
		string++;
	}
	return res;
}

char* replace(char* string, char* from, char* to) {

	// assert nothing is a NULLptr
	if (string == NULL || from == NULL || to == NULL)
		return NULL;

	size_t totlen = strlen(string);
	size_t frolen = strlen(from);
	size_t to_len = strlen(to);
	
	// get count of how many snr's we need to do...
	size_t from_count = instances(string, from);

	// reallocate memory...
	size_t newsize = totlen + ((long long)to_len - (long long)frolen) * from_count;

	char* res = (char*)malloc(string, newsize);
	if (res == NULL) {
		puts("error: insufficient heap memory for new string...");
		return NULL;
	}

	char* restmp = res;
	char* tmp = string;
	while (*tmp) {

		if (__strcmp_g_inplace(tmp, from)) {
			
			char* fromtmp = from;
			while (*fromtmp) {

			}

		}

		else {
			*restmp = *tmp;
			tmp++;
			restmp++;
		}
	}

	return res;
}