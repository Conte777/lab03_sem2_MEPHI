#include <stdio.h>
#include <stdlib.h>

typedef enum {
	OK1 = 0,
	OF1 = 1,
	UN1 = 2,
	DB1 = 3,
	CZ1 = 4
}errors1;

typedef struct string {
	int size;
	char* string;
}string;

int get_s(string** s);

int compar(string* s1, string* s2);

void free_s(string* s);

int strcp(string* s1, string** s2);

void print_string(string* s);