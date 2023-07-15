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

	size_t	totlen = strlen(string),
			frolen = strlen(from),
			to_len = strlen(to),
			from_count = instances(string, from); // get count of how many snr's we need to do...

	// reallocate memory...
	int delta = ((long long)to_len - (long long)frolen);
	size_t newsize = (size_t)((long long)totlen + (delta * (long long)from_count));

	//newsize = totlen;
	char* res = (char*)malloc(sizeof(char) * (newsize + 1));
	if (res == NULL) {
		puts("error: insufficient heap memory for new string...");
		return NULL;
	}
	res[newsize] = '\0'; // nul-terminated at initialization. WTF WHY DOES THIS SUDDENLY WORK???!!?!!?
	char* restmp = res;
	for (char* tmp = string; *tmp; tmp++) {

		if (__strcmp_g_inplace(tmp, from)) {
			for (char* to_tmp = to; *to_tmp; to_tmp++) {
				*restmp = *to_tmp;
				restmp++;
			}
			for (int i = 1; i < strlen(from); i++)
				tmp++; // increment pointer on `from` string for each char that was read off the `to` string
		}

		else {
			*restmp = *tmp;
			restmp++;
		}
	}
	
	return res;
}