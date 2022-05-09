#include <locale.h>
#include "table.h"

int scanfs(int* a, int b);
int errors_get(int a, string* s);
int errors_add(int a);
int choice(table* t, int a);

int errors_get(int a, string* s) {
	switch (a) {
	case CZ:
		return CZ;
	case OF:
		printf("The line is too long! Try again: ");
		errors_get(get_s(&s), s);
	case OK:
		return OK;
	}
	return UN;
}

int errors_add(int a) {
	switch (a) {
	case DB:
		printf("Duplicate detected, it is not possible to add this element!\n");
		return DB;
	case OF:
		printf("Overflow of one of the key spaces!\n");
		return DB;
	case UN:
		printf("Something is wrong with the table or the information you want to add.\n");
		return UN;
	case OK:
		return OK;
	}
	return UN;
}

int choice(table* t, int a) {
	item l;
	item* g = NULL;
	string* key1 = NULL, * key2 = NULL, * info = NULL;
	int b;
	switch (a) {
	case 1:
		printf("Enter key1: ");
		if (errors_get(get_s(&key1), key1) == CZ)
			return CZ;
		printf("Enter key2: ");
		if (errors_get(get_s(&key2), key2) == CZ)
			return CZ;
		printf("Enter information: ");
		if (errors_get(get_s(&info), info) == CZ)
			return CZ;
		l.info = info;
		l.key1 = key1;
		l.key2 = key2;
		//system("cls");
		if (errors_add(add_e(t, &l)) == OK)
			printf("The element has been added successfully!\n");
#ifdef work_file
		free_s(key1);
		free_s(key2);
		free_s(info);
#else
		else {
			free_s(key1);
			free_s(key2);
			free_s(info);
		}
#endif
		return OK;
	case 2:
		printf("Enter key1: ");
		if (errors_get(get_s(&key1), key1) == CZ)
			return CZ;
		printf("Enter key2: ");
		if (errors_get(get_s(&key2), key2) == CZ)
			return CZ;
		//system("cls");
		g = (item*)calloc(1, sizeof(item));
		if (scan(t, g, key1, key2) == OK) {
			printf("Here's what was found on your key:\nkey1: ");
			print_string(g->key1);
			printf("\nkey2: ");
			print_string(g->key2);
			printf("\ninfo: ");
			print_string(g->info);
			printf("\n");
		}
		else
			printf("Nothing was found for your key.\n");
		free_i(g);
		free_s(key1);
		free_s(key2);
		return OK;
	case 3:
		printf("Enter key1: ");
		if (errors_get(get_s(&key1), key1) == CZ)
			return CZ;
		printf("Enter key2: ");
		if (errors_get(get_s(&key2), key2) == CZ)
			return CZ;
		//system("cls");
		if (del_e(t, key1, key2) == OK)
			printf("The item was successfully deleted!\n");
		else
			printf("An element with such a key was not found.\n");
		free_s(key1);
		free_s(key2);
		return OK;
	case 4:
		printf("Choose in which key space we will work \n1. The first \n2. Second\nEnter the number: ");
		scanfs(&b, 0);
		while (b != 1 && b != 2) {
			printf("You didn't enter 1 or 2, try again: ");
			scanfs(&b, 0);
		}
		if (b == 1) {
			printf("Enter key1: ");
			if (errors_get(get_s(&key1), key1) == CZ)
				return CZ;
			//system("cls");
			g = (item*)calloc(1, sizeof(item));
			if (scan_KS1(t, key1, g) == OK) {
				printf("Here's what was found on your key:\nkey1: ");
				print_string(g->key1);
				printf("\nkey2: ");
				print_string(g->key2);
				printf("\ninfo: ");
				print_string(g->info);
				printf("\n");
			}
			else
				printf("Nothing was found on your key.\n");
			free_s(key1);
		}
		else {
			printf("Enter key2: ");
			if (errors_get(get_s(&key2), key2) == CZ)
				return CZ;
			system("cls");
			g = (item*)calloc(1, sizeof(item));
			if (scan_KS2(t, key2, g) == OK) {
				printf("Here's what was found on your key:\nkey1: ");
				print_string(g->key1);
				printf("\nkey2: ");
				print_string(g->key2);
				printf("\ninfo: ");
				print_string(g->info);
				printf("\n");
			}
			else
				printf("Nothing was found on your key.\n");
			free_s(key2);
		}
		free_i(g);
		return OK;
	case 5:
		printf("Choose in which key space we will work \n1. The first \n2. Second\nEnter the number: ");
		scanfs(&b, 0);
		while (b != 1 && b != 2) {
			printf("You didn't enter 1 or 2, try again: ");
			scanfs(&b, 0);
		}
		if (b == 1) {
			printf("Enter key1: ");
			if (errors_get(get_s(&key1), key1) == CZ)
				return CZ;
			//system("cls");
			if (del_e_KS1(t, key1) == OK)
				printf("The item was successfully deleted.\n");
			else
				printf("Nothing was found on your key.\n");
			free_s(key1);
		}
		else {
			printf("Enter key2: ");
			if (errors_get(get_s(&key2), key2) == CZ)
				return CZ;
			//system("cls");
			if (del_e_KS2(t, key2) == OK)
				printf("The item was successfully deleted.\n");
			else
				printf("Nothing was found on your key.\n");
			free_s(key2);
		}
		return OK;
	case 6:
		print_table_KS1(t);
		return OK;
	case 7:
		printf("Enter key1: ");
		if (errors_get(get_s(&key1), key1) == CZ)
			return CZ;
		printf("Enter key2: ");
		if (errors_get(get_s(&key2), key2) == CZ)
			return CZ;
		//system("cls");
		if (del_d_KS1(t, key1, key2) == OK)
			printf("The items was successfully deleted.\n");
		else
			printf("Nothing was found on your key.\n");
		free_s(key1);
		free_s(key2);
		return OK;
	case 8:
		reorg(t);
		printf("Первое пространство ключей реаорганизовано.\n");
		return OK;
	case 9:
		print_table_KS2(t);
		return OK;
	default:
		printf("You didn't enter a number in the possible range \n");
		return OK;
	}
	return CZ;
}

int scanfs(int* a, int b) {
	int c;
	do {
		c = scanf("%d", a);
		if (c < 0)
			return 1;
		if ((b == 1 && *a <= 0) || c == 0) {
			printf("Error, try again: ");
			scanf("%*c");
		}
	} while ((b == 1 && *a <= 0) || c == 0);
	return 0;
}

int main() {
	setlocale(LC_ALL, "Rus");
	int flag = 1, msize1 = 1, msize2 = 1, a = 0, b = 0, flag1 = 0;
	table t;
#ifdef work_file
	printf("Enter the path to the file: ");
	get_s(&t.file_name);
	FILE* file = NULL;
	file = fopen(t.file_name->string, "rb");
	if (file == NULL)
		printf("Couldn't find a file with that name, creating a new \n");
	else {
		printf("Found a file with this name, open \n");
		flag1 = 1;
		fclose(file);
	}
#endif
	if (!flag1) {
		printf("Enter the size of the first key space: ");
		a = scanfs(&msize1, 1);
		if (a == 1)
			flag = 0;
		printf("Enter the size of the second key space: ");
		if (scanfs(&msize2, 1) && a == 1)
			flag = 0;
	}
	if (flag)
		create_table(&t, msize1, msize2);
	while (flag) {
		printf("\nEnter the number of the item to be executed\n0. Exit\n1. Add item to the table\n2. Scan in the table by the composite key \n3. Deleting an element in the table by the composite key \n4. Search in the element in the table by any given key \n5. Deleting from the table an element specified by any given key\n6. Print table by KS1\n7. Deleting item by the range of given key without reorganizing the table \n8. Reorganization of the first key space\n9. Print table by KS2\nEnter the number: ");
		b = scanf("%d", &a);
		//system("cls");
		if (b > 0 && a > 0 && a < 10) {
			if (choice(&t, a) == CZ)
				flag = 0;
		}
		else {
			if (b == 0) {
				printf("You didn't enter a number, try again: ");
				scanf("%*c");
			}
			else
				if (a == 0 || b < 0)
					flag = 0;
		}
	}
#ifdef work_file
	reorg_file(&t);
#endif
	if (msize1 > 0 && msize2 > 0)
		free_table(&t);
	return 0;
}