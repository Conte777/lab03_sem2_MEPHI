#include "string1.h"

int get_s(string** s) {
	*s = (string*)calloc(1, sizeof(string));
	(*s)->size = -1;
	char a = 1;
	char* p = NULL;
	int b = scanf("%c", &a);
	if (b < 0)
		return CZ1;
	while (a == '\n' || a == ' ')
		scanf("%c", &a);
	while (a != ' ' && a != '\n' && b > 0) {
		(*s)->size++;
		if ((*s)->string == NULL)
			(*s)->string = (char*)malloc(sizeof(char));
		else {
			p = (*s)->string;
			(*s)->string = (char*)realloc((*s)->string, ((*s)->size + 1) * sizeof(char));
		}
		if ((*s)->string == NULL) {
			free(p);
			return OF1;
		}
		(*s)->string[(*s)->size] = a;
		b = scanf("%c", &a);
		if (b < 0)
			return CZ1;
	}
	(*s)->string = (char*)realloc((*s)->string, ((*s)->size + 2) * sizeof(char));
	(*s)->string[(*s)->size + 1] = '\0';
	return OK1;
}

int compar(string* s1, string* s2) {
	if (s1 == NULL || s2 == NULL)
		return 0;
	if (s1->string == NULL || s2->string == NULL)
		return 0;
	if (s1->size != s2->size)
		return 0;
	else
		for (int i = 0; i <= s1->size; i++)
			if (s1->string[i] != s2->string[i])
				return 0;
	return 1;
}

void free_s(string* s) {
	if (s != NULL) {
		if (s->string != NULL)
			free(s->string);
		s->string = NULL;
		s->size = -1;
		free(s);
	}
}

int strcp(string* s1, string** s2) {
	if (s1 == NULL)
		return UN1;
	*s2 = (string*)calloc(1, sizeof(string));
	(*s2)->string = (char*)calloc(s1->size + 1, sizeof(char));
	(*s2)->size = s1->size;
	for (int i = 0; i <= s1->size; i++) {
		(*s2)->string[i] = s1->string[i];
	}
	return OK1;
}

void print_string(string* s) {
	if (s != NULL)
		if (s->string != NULL)
			for (int i = 0; i <= s->size; i++)
				printf("%c", s->string[i]);
}