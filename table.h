#include <stdio.h>
#include <stdlib.h>
#include "string1.h"
#include <string.h>
//#define work_file

typedef enum {
	OK = 0,
	OF = 1,
	UN = 2,
	DB = 3,
	CZ = 4
}errors;

typedef struct item {
	string* info;
	string* key1;
	string* key2;
#ifndef work_file
	int ind1;
	void* p2;
#endif
}item;

#ifndef work_file
typedef struct KS1 {
	int busy;
	string* key;
	item* info;
}KS1;

typedef struct KS2 {
	string* key;
	struct KS2* next;
	item* info;
}KS2;
#endif

typedef struct table {
#ifndef work_file
	KS1* ks1;
	KS2** ks2;
#else
	int ks1, ks2;
	string* file_name;
#endif
	int msize1;
	int msize2;
}table;

void create_table(table* t, int msize1, int msize2);

int add_e(table* t, item* info);

int scan_KS1(table* t, string* key1, item* info);

int scan_KS2(table* t, string* key2, item* info);

int scan(table* t, item* info, string* key1, string* key2);

int del_e_KS1(table* t, string* key1);

int del_d_KS1(table* t, string* key1, string* key2);

int del_e_KS2(table* t, string* key2);

int del_e(table* t, string* key1, string* key2);

int reorg(table* t);

int print_table_KS1(table* t);

int print_table_KS2(table* t);

int hashf(string* s, int m);

void free_i(item* a);

void free_table(table* t);
#ifdef work_file
int reorg_file(table* t);
#endif
