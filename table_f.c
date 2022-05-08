#include "table.h"
#ifdef work_file
void create_table(table* t, int msize1, int msize2) {
	FILE* file = NULL;
	int flag = 1;
	file = fopen(t->file_name->string, "rb+");
	if (file == NULL) {
		file = fopen(t->file_name->string, "wb+");
		flag = 0;
	}
	if (!flag) {
		fwrite(&msize1, sizeof(int), 1, file);
		fwrite(&msize2, sizeof(int), 1, file);
		t->ks1 = sizeof(int) * 4;
		fwrite(&t->ks1, sizeof(int), 1, file);
		t->ks2 = t->ks1 + 3 * sizeof(int) * msize1;
		fwrite(&t->ks2, sizeof(int), 1, file);
		t->msize1 = msize1;
		t->msize2 = msize2;
	}
	else {
		fread(&t->msize1, sizeof(int), 1, file);
		fread(&t->msize2, sizeof(int), 1, file);
		fread(&t->ks1, sizeof(int), 1, file);
		fread(&t->ks2, sizeof(int), 1, file);
	}
	if (!flag) {
		int b = sizeof(int) * 4 + 3 * sizeof(int) * msize1 + 3 * sizeof(int) * msize2 - 1;
		fseek(file, b, SEEK_SET);
		b = 0;
		fwrite(&b, sizeof(int), 1, file);
	}
	fclose(file);
}

int add_e(table* t, item* info) {
	FILE* file = NULL;
	file = fopen(t->file_name->string, "rb+");
	if (file == NULL)
		return UN;
	int a = t->ks1, flag = 0, k1 = 0;
	for (int i = 0; i < t->msize1; i++) {
		fseek(file, a, SEEK_SET);
		int b;
		fread(&b, sizeof(int), 1, file);
		if (b == 0 && !flag) {
			flag = 1;
			k1 = ftell(file) - sizeof(int);
		}
		if (b == 1) {
			int c;
			fread(&c, sizeof(int), 1, file);
			fseek(file, c, SEEK_SET);
			fread(&c, sizeof(int), 1, file);
			string* key = (string*)calloc(1, sizeof(string));
			key->size = c;
			key->string = (char*)calloc(c + 1, sizeof(char));
			fread(key->string, sizeof(char), c + 1, file);
			if (compar(key, info->key1)) {
				fclose(file);
				free_s(key);
				return DB;
			}
			free_s(key);
		}
		a += 3 * sizeof(int);
	}
	if (!flag) {
		fclose(file);
		return OF;
	}

	fseek(file, t->ks2 + sizeof(int) * hashf(info->key2, t->msize2), SEEK_SET);
	int buffer, k2;
	fread(&buffer, sizeof(int), 1, file);
	if (buffer == 0) {
		fseek(file, 0, SEEK_END);
		buffer = 0;
		fwrite(&buffer, sizeof(int), 1, file);
		fwrite(&buffer, sizeof(int), 1, file);
		fwrite(&buffer, sizeof(int), 1, file);
		k2 = ftell(file) - 3 * sizeof(int);
	}
	else {
		fseek(file, buffer, SEEK_SET); // ������� � ������ ������
		int c = 0, d = 0;
		do {
			fread(&c, sizeof(int), 1, file); // ������� ������������ �����
			fread(&d, sizeof(int), 1, file); // ������� ������������ ���� ��������
			fseek(file, c, SEEK_SET); // ������� �� ����
			fread(&c, sizeof(int), 1, file); // ������� ������ �����
			string* key = (string*)calloc(1, sizeof(string)); // �������� ������ ��� ����
			key->size = c;
			key->string = (char*)calloc(c + 1, sizeof(char));
			fread(key->string, sizeof(char), c + 1, file); // ��������� ����
			if (compar(key, info->key2)) {
				free_s(key);
				fclose(file);
				return DB;
			}
			free_s(key);
			fseek(file, d, SEEK_SET);
		} while (d != 0);
		fseek(file, 0, SEEK_END);
		int b = 0;
		fwrite(&b, sizeof(int), 1, file);
		fwrite(&buffer, sizeof(int), 1, file);
		fwrite(&b, sizeof(int), 1, file);
		k2 = ftell(file) - 3 * sizeof(int);
	}
	fseek(file, t->ks2 + sizeof(int) * hashf(info->key2, t->msize2), SEEK_SET);
	fwrite(&k2, sizeof(int), 1, file);

	fseek(file, k1, SEEK_SET);
	int off_set[3];
	buffer = 1;
	fwrite(&buffer, sizeof(int), 1, file);
	fseek(file, 0, SEEK_END);
	fwrite(&info->key1->size, sizeof(int), 1, file);
	off_set[0] = ftell(file) - sizeof(int);
	fwrite(info->key1->string, sizeof(char), info->key1->size + 1, file);
	fwrite(&info->key2->size, sizeof(int), 1, file);
	off_set[1] = ftell(file) - sizeof(int);
	fwrite(info->key2->string, sizeof(char), info->key2->size + 1, file);
	fwrite(&info->info->size, sizeof(int), 1, file);
	off_set[2] = ftell(file) - sizeof(int);
	fwrite(info->info->string, sizeof(char), info->info->size + 1, file);
	fwrite(&k1, sizeof(int), 1, file);
	fwrite(&k2, sizeof(int), 1, file);
	fseek(file, k1 + sizeof(int), SEEK_SET);
	fwrite(&off_set[0], sizeof(int), 1, file);
	fwrite(&off_set[2], sizeof(int), 1, file);
	fseek(file, k2, SEEK_SET);
	fwrite(&off_set[1], sizeof(int), 1, file);
	fseek(file, sizeof(int), SEEK_CUR);
	fwrite(&off_set[2], sizeof(int), 1, file);
	fclose(file);
	return OK;
}

int scan_KS1(table* t, string* key1, item* info) {
	FILE* file = NULL;
	file = fopen(t->file_name->string, "rb");
	if (file == NULL)
		return UN;
	int a = t->ks1, buffer;
	for (int i = 0; i < t->msize1; i++) {
		fseek(file, a, SEEK_SET);
		fread(&buffer, sizeof(int), 1, file);
		if (buffer == 1) {
			fread(&buffer, sizeof(int), 1, file);
			fseek(file, buffer, SEEK_SET);
			string* s = (string*)calloc(1, sizeof(string));
			fread(&s->size, sizeof(int), 1, file);
			s->string = (char*)calloc(s->size + 1, sizeof(char));
			fread(s->string, sizeof(char), s->size + 1, file);
			if (compar(key1, s)) {
				info->key1 = s;
				s = (string*)calloc(1, sizeof(string));
				fread(&s->size, sizeof(int), 1, file);
				s->string = (char*)calloc(s->size + 1, sizeof(char));
				fread(s->string, sizeof(char), s->size + 1, file);
				info->key2 = s;
				s = (string*)calloc(1, sizeof(string));
				fread(&s->size, sizeof(int), 1, file);
				s->string = (char*)calloc(s->size + 1, sizeof(char));
				fread(s->string, sizeof(char), s->size + 1, file);
				info->info = s;
				fclose(file);
				return OK;
			}
			else 
				free_s(s);
		}
		a += 3 * sizeof(int);
	}
	fclose(file);
	return UN;
}

int scan_KS2(table* t, string* key2, item* info) {
	FILE* file = NULL;
	file = fopen(t->file_name->string, "rb");
	if (file == NULL)
		return UN;
	fseek(file, t->ks2 + sizeof(int) * hashf(key2, t->msize2), SEEK_SET);
	int buffer, next;
	fread(&next, sizeof(int), 1, file);
	if (next != 0) {
		do {
			fseek(file, next, SEEK_SET);
			fread(&buffer, sizeof(int), 1, file);
			fread(&next, sizeof(int), 1, file);
			fseek(file, buffer, SEEK_SET);
			string* s = (string*)calloc(1, sizeof(string));
			fread(&s->size, sizeof(int), 1, file);
			s->string = (char*)calloc(s->size + 1, sizeof(char));
			fread(s->string, sizeof(char), s->size + 1, file);
			if (compar(key2, s)) {
				info->key2 = s;
				s = (string*)calloc(1, sizeof(string));
				fread(&s->size, sizeof(int), 1, file);
				s->string = (char*)calloc(s->size + 1, sizeof(char));
				fread(s->string, sizeof(char), s->size + 1, file);
				info->info = s;
				fread(&buffer, sizeof(int), 1, file);
				fseek(file, buffer + sizeof(int), SEEK_SET);
				fread(&buffer, sizeof(int), 1, file);
				fseek(file, buffer, SEEK_SET);
				s = (string*)calloc(1, sizeof(string));
				fread(&s->size, sizeof(int), 1, file);
				s->string = (char*)calloc(s->size + 1, sizeof(char));
				fread(s->string, sizeof(char), s->size + 1, file);
				info->key1 = s;
				fclose(file);
				return OK;
			}
			else
				free_s(s);
		} while (next != 0);
	}
	fclose(file);
	return UN;
}

int scan(table* t, item* info, string* key1, string* key2) {
	if (t != NULL && key1 != NULL && key2 != NULL) {
		item* info1, * info2;
		info1 = (item*)calloc(1, sizeof(item));
		info2 = (item*)calloc(1, sizeof(item));
		scan_KS1(t, key1, info1);
		scan_KS2(t, key2, info2);
		if (compar(info1->info, info2->info) && compar(info1->key1, info2->key1) && compar(info1->key2, info2->key2)) {
			info->info = info1->info;
			info->key1 = info1->key1;
			info->key2 = info1->key2;
			free(info1);
			free_i(info2);
			return OK;
		}
		free_i(info1);
		free_i(info2);
	}
	return UN;
}

int del_e_KS1(table* t, string* key1) {
	if (t != NULL && key1 != NULL) {
		FILE* file = NULL;
		file = fopen(t->file_name->string, "rb+");
		if (file == NULL)
			return UN;
		int a = t->ks1, buffer;
		for (int i = 0; i < t->msize1; i++) {
			fseek(file, a, SEEK_SET);
			fread(&buffer, sizeof(int), 1, file);
			if (buffer == 1) {
				fread(&buffer, sizeof(int), 1, file);
				fseek(file, buffer, SEEK_SET);
				string* s = (string*)calloc(1, sizeof(string));
				fread(&s->size, sizeof(int), 1, file);
				s->string = (char*)calloc(s->size + 1, sizeof(char));
				fread(s->string, sizeof(char), s->size + 1, file);
				if (compar(s, key1)) {
					string* s2 = (string*)calloc(1, sizeof(string));
					fread(&s2->size, sizeof(int), 1, file);
					s2->string = (char*)calloc(s2->size + 1, sizeof(char));
					fread(s2->string, sizeof(char), s2->size + 1, file);
					fread(&buffer, sizeof(int), 1, file);
					fseek(file, sizeof(char) * (buffer + 1) + sizeof(int), SEEK_CUR);
					int c;
					fread(&c, sizeof(int), 1, file);
					fseek(file, c + sizeof(int), SEEK_SET);
					int next;
					fread(&next, sizeof(int), 1, file);
					fseek(file, t->ks2 + sizeof(int) * hashf(s2, t->msize2), SEEK_SET); // ��� ������ ���� s
					fread(&buffer, sizeof(int), 1, file);
					if (buffer == c) {
						fseek(file, t->ks2 + sizeof(int) * hashf(s2, t->msize2), SEEK_SET);
						fwrite(&next, sizeof(int), 1, file);
					}
					else {
						while (buffer != c) {
							fseek(file, buffer + sizeof(int), SEEK_SET);
							fread(&buffer, sizeof(int), 1, file);
						}
						fseek(file, -4, SEEK_CUR);
						fwrite(&next, sizeof(int), 1, file);
					}

					fseek(file, a, SEEK_SET);
					int b = 0;
					fwrite(&b, sizeof(int), 1, file);
					free_s(s);
					free_s(s2);
					fclose(file);
					return OK;
				}
				free_s(s);
			}
			a += 3 * sizeof(int);
		}
		fclose(file);
	}
	return UN;
}

int del_e_KS2(table* t, string* key2) {
	if (t != NULL && key2 != NULL) {
		FILE* file = NULL;
		file = fopen(t->file_name->string, "rb+");
		if (file == NULL)
			return UN;
		fseek(file, t->ks2 + sizeof(int) * hashf(key2, t->msize2), SEEK_SET);
		int buffer, next;
		fread(&buffer, sizeof(int), 1, file);
		next = buffer;
		if (buffer != 0) {
			do {
				int c = next;
				fseek(file, next, SEEK_SET);
				fread(&buffer, sizeof(int), 1, file);
				fread(&next, sizeof(int), 1, file);
				fseek(file, buffer, SEEK_SET);
				string* s = (string*)calloc(1, sizeof(string));
				fread(&s->size, sizeof(int), 1, file);
				s->string = (char*)calloc(s->size + 1, sizeof(char));
				fread(s->string, sizeof(char), s->size + 1, file);
				if (compar(s, key2)) {
					fread(&buffer, sizeof(int), 1, file);
					fseek(file, sizeof(char) * (buffer + 1), SEEK_CUR);
					fread(&buffer, sizeof(int), 1, file);
					fseek(file, buffer, SEEK_SET);
					buffer = 0;
					fwrite(&buffer, sizeof(int), 1, file);

					fseek(file, t->ks2 + sizeof(int) * hashf(key2, t->msize2), SEEK_SET);
					fread(&buffer, sizeof(int), 1, file);
					while (c != buffer) {
						fseek(file, buffer + sizeof(int), SEEK_SET);
						fread(&buffer, sizeof(int), 1, file);
					}
					fseek(file, -4, SEEK_CUR);
					fwrite(&next, sizeof(int), 1, file);
					free_s(s);
					fclose(file);
					return OK;
				}
				free_s(s);
			} while (next != 0);
		}
		fclose(file);
	}
	return UN;
}

int del_e(table* t, string* key1, string* key2) {
	if (t != NULL && key1 != NULL && key2 != NULL) {
		item* info = (item*)calloc(1, sizeof(item));
		if (scan(t, info, key1, key2) == OK) {
			del_e_KS2(t, key2);
			free_i(info);
			return OK;
		}
		free_i(info);
	}
	return UN;
}

int del_d_KS1(table* t, string* key1, string* key2) {
	if (t != NULL) {
		FILE* file = NULL;
		file = fopen(t->file_name->string, "rb+");
		if (file == NULL)
			return UN;
		int a = t->ks1, buffer, flag = 0;
		for (int i = 0; i < t->msize1; i++) {
			fseek(file, a, SEEK_SET);
			fread(&buffer, sizeof(int), 1, file);
			if (buffer == 1) {
				fread(&buffer, sizeof(int), 1, file);
				fseek(file, buffer, SEEK_SET);
				string* s = (string*)calloc(1, sizeof(string));
				fread(&s->size, sizeof(int), 1, file);
				s->string = (char*)calloc(s->size + 1, sizeof(char));
				fread(s->string, sizeof(char), s->size + 1, file);
				s->string = (char*)realloc(s->string, s->size + 2);
				s->string[s->size + 1] = '\0';
				if (strcmp(key1->string, s->string) < 0 && strcmp(key2->string, s->string) > 0) {
					flag = 1;
					string* s2 = (string*)calloc(1, sizeof(string));
					fread(&s2->size, sizeof(int), 1, file);
					s2->string = (char*)calloc(s2->size + 1, sizeof(char));
					fread(s2->string, sizeof(char), s2->size + 1, file);
					fread(&buffer, sizeof(int), 1, file);
					fseek(file, sizeof(char) * (buffer + 1) + sizeof(int), SEEK_CUR);
					int c;
					fread(&c, sizeof(int), 1, file);
					fseek(file, c + sizeof(int), SEEK_SET);
					int next;
					fread(&next, sizeof(int), 1, file);
					fseek(file, t->ks2 + sizeof(int) * hashf(s2, t->msize2), SEEK_SET); 
					fread(&buffer, sizeof(int), 1, file);
					if (buffer == c) {
						fseek(file, t->ks2 + sizeof(int) * hashf(s2, t->msize2), SEEK_SET);
						fwrite(&next, sizeof(int), 1, file);
					}
					else {
						while (buffer != c) {
							fseek(file, buffer + sizeof(int), SEEK_SET);
							fread(&buffer, sizeof(int), 1, file);
						}
						fseek(file, -4, SEEK_CUR);
						fwrite(&next, sizeof(int), 1, file);
					}

					fseek(file, a, SEEK_SET);
					int b = 0;
					fwrite(&b, sizeof(int), 1, file);
					free_s(s2);
				}
				free_s(s);
			}
			a += 3 * sizeof(int);
		}
		fclose(file);
		if (flag)
			return OK;
	}
	return UN;
}

int reorg(table* t) {
	if (t != NULL) {
		FILE* file = NULL;
		file = fopen(t->file_name->string, "rb+");
		if (file == NULL)
			return UN;
		int p1 = t->ks1, p2 = t->ks2 - 3 * sizeof(int), buffer1, buffer2;
		while (p1 < p2) {
			fseek(file, p1, SEEK_SET);
			fread(&buffer1, sizeof(int), 1, file);
			fseek(file, p2, SEEK_SET);
			fread(&buffer2, sizeof(int), 1, file);
			if (buffer1 == 0 && buffer2 == 1) {
				int key, info;
				fread(&key, sizeof(int), 1, file);
				fread(&info, sizeof(int), 1, file);
				fseek(file, p2, SEEK_SET);
				fwrite(&buffer1, sizeof(int), 1, file);

				fseek(file, p1, SEEK_SET);
				fwrite(&buffer2, sizeof(int), 1, file);
				fwrite(&key, sizeof(int), 1, file);
				fwrite(&info, sizeof(int), 1, file);
				p1 += 3 * sizeof(int);
				p2 -= 3 * sizeof(int);
			}
			if (buffer1 == 1)
				p1 += 3 * sizeof(int);
			if (buffer2 == 0)
				p2 -= 3 * sizeof(int);
		}
		fclose(file);
	}
	return UN;
}

int print_table_KS1(table* t) {
	FILE* file = NULL;
	file = fopen(t->file_name->string, "rb");
	if (file == NULL)
		return UN;
	printf("key1 key2 info\n");
	int a = t->ks1;
	for (int i = 0; i < t->msize1; i++) {
		fseek(file, a, SEEK_SET);
		int busy;
		fread(&busy, sizeof(int), 1, file);
		if (busy == 1) {
			/*int off_set, off_set2;
			fread(&off_set, sizeof(int), 1, file);
			fread(&off_set2, sizeof(int), 1, file);
			fseek(file, off_set, SEEK_SET);
			string* key1 = (string*)calloc(1, sizeof(string));
			fread(&key1->size, sizeof(int), 1, file);
			key1->string = (char*)calloc(key1->size + 1, sizeof(char));
			fread(key1->string, sizeof(char), key1->size + 1, file);
			fseek(file, off_set2, SEEK_SET);
			string* info = (string*)calloc(1, sizeof(string));
			fread(&info->size, sizeof(int), 1, file);
			info->string = (char*)calloc(info->size + 1, sizeof(char));
			fread(info->string, sizeof(char), info->size + 1, file);
			fread(&off_set, sizeof(int), 1, file);
			fread(&off_set, sizeof(int), 1, file);
			fseek(file, off_set, SEEK_SET);
			fread(&off_set, sizeof(int), 1, file);
			string* key2 = (string*)calloc(1, sizeof(string));
			fread(&key2->size, sizeof(int), 1, file);
			key2->string = (char*)calloc(key2->size + 1, sizeof(char));
			fread(key2->string, sizeof(char), key2->size + 1, file);
			print_string(key1);
			printf(" ");
			print_string(key2);
			printf(" ");
			print_string(info);
			printf("\n");
			free_i(key1);
			free_i(key2);
			free_i(info);*/
			int off_set;
			fread(&off_set, sizeof(int), 1, file);
			fseek(file, off_set, SEEK_SET);
			string* s = NULL;
			for (int j = 0; j < 3; j++) {
				s = (string*)calloc(1, sizeof(string));
				fread(&s->size, sizeof(int), 1, file);
				s->string = (char*)calloc(s->size + 1, sizeof(char));
				fread(s->string, sizeof(char), s->size + 1, file);
				print_string(s);
				printf(" ");
				free_s(s);
			}
			printf("\n");
		}
		a += 3 * sizeof(int);
	}
	fclose(file);
	return OK;
}

int print_table_KS2(table* t) {
	if (t != NULL) {
		FILE* file = NULL;
		file = fopen(t->file_name->string, "rb");
		if (file == NULL)
			return UN;
		for (int i = 0; i < t->msize2; i++) {
			fseek(file, t->ks2 + sizeof(int) * i, SEEK_SET);
			int buffer, next;
			fread(&buffer, sizeof(int), 1, file);
			next = buffer;
			if (buffer == 0)
				printf("KS2[%d]\nNULL\n", i);
			else {
				printf("KS[%d]\nkey1 key2 hash info\n", i);
				do {
					fseek(file, next, SEEK_SET);
					fread(&buffer, sizeof(int), 1, file);
					fread(&next, sizeof(int), 1, file);
					fseek(file, buffer, SEEK_SET);
					string* key1 = (string*)calloc(1, sizeof(string)), * key2 = (string*)calloc(1, sizeof(string)), * info = (string*)calloc(1, sizeof(string));
					fread(&key2->size, sizeof(int), 1, file);
					key2->string = (char*)calloc(key2->size + 1, sizeof(char));
					fread(key2->string, sizeof(char), key2->size + 1, file);
					fread(&info->size, sizeof(int), 1, file);
					info->string = (char*)calloc(info->size + 1, sizeof(char));
					fread(info->string, sizeof(char), info->size + 1, file);
					fread(&buffer, sizeof(int), 1, file);
					fseek(file, buffer + sizeof(int), SEEK_SET);
					fread(&buffer, sizeof(int), 1, file);
					fseek(file, buffer, SEEK_SET);
					fread(&key1->size, sizeof(int), 1, file);
					key1->string = (char*)calloc(key1->size + 1, sizeof(char));
					fread(key1->string, sizeof(char), key1->size + 1, file);
					print_string(key1);
					printf(" ");
					print_string(key2);
					printf(" %d ", hashf(key2, t->msize2));
					print_string(info);
					printf("\n");
					free_s(key1);
					free_s(key2);
					free_s(info);
				} while (next != 0);
			}
		}
		fclose(file);
		return OK;
	}
	return UN;
}

void free_i(item* i) {
	if (i != NULL) {
		if (i->info != NULL)
			free_s(i->info);
		if (i->key1 != NULL)
			free_s(i->key1);
		if (i->key2 != NULL)
			free_s(i->key2);
		free(i);
		i->info = NULL;
		i->key1 = NULL;
		i->key2 = NULL;
	}
}

int hashf(string* s, int m) {
	if (s == NULL)
		return 0;
	int r = 0;
	for (int i = 0; i <= s->size; i++)
		r += (int)(s->string[i]);
	return r % m;
}

void free_table(table* t) {
	if (t != NULL)
		if (t->file_name != NULL)
			if (t->file_name->string != NULL)
				free_s(t->file_name);
}

int reorg_file(table* t) {
	if (t != NULL) {
		FILE* file1 = NULL, * file2;
		file1 = fopen(t->file_name->string, "rb");
		if (file1 == NULL)
			return UN;
		file2 = fopen("buffer.bin", "wb+");
		if (file2 == NULL)
			return UN;
		for (int i = 0; i < 4; i++) {
			int buffer;
			fread(&buffer, sizeof(int), 1, file1);
			fwrite(&buffer, sizeof(int), 1, file2);
		}
		int b = sizeof(int) * 4 + 3 * sizeof(int) * t->msize1 + 3 * sizeof(int) * t->msize2 - 1;
		fseek(file2, b, SEEK_SET);
		b = 0;
		fwrite(&b, sizeof(int), 1, file2);
		int a = t->ks1, buffer;
		for (int i = 0; i < t->msize1; i++) {
			fseek(file1, a, SEEK_SET);
			fseek(file2, a, SEEK_SET);
			fread(&buffer, sizeof(int), 1, file1);
			if (buffer == 0) {
				for (int j = 0; j < 3; j++)
					fwrite(&buffer, sizeof(int), 1, file2);
			}
			else {
				fwrite(&buffer, sizeof(int), 1, file2);
				int k1 = a, k2, offset[3];
				fread(&buffer, sizeof(int), 1, file1);
				fseek(file1, buffer, SEEK_SET);
				string* key1, * key2, * info;
				key1 = (string*)calloc(1, sizeof(string));
				key2 = (string*)calloc(1, sizeof(string));
				info = (string*)calloc(1, sizeof(string));
				fread(&key1->size, sizeof(int), 1, file1);
				key1->string = (char*)calloc(key1->size + 1, sizeof(char));
				fread(key1->string, sizeof(char), key1->size + 1, file1);
				fread(&key2->size, sizeof(int), 1, file1);
				key2->string = (char*)calloc(key2->size + 1, sizeof(char));
				fread(key2->string, sizeof(char), key2->size + 1, file1);
				fread(&info->size, sizeof(int), 1, file1);
				info->string = (char*)calloc(info->size + 1, sizeof(char));
				fread(info->string, sizeof(char), info->size + 1, file1);
				fseek(file2, 0, SEEK_END);
				buffer = 0;
				for (int i = 0; i < 3; i++)
					fwrite(&buffer, sizeof(int), 1, file2);
				k2 = ftell(file2) - 3 * sizeof(int);
				fwrite(&key1->size, sizeof(int), 1, file2);
				offset[0] = ftell(file2) - sizeof(int);
				fwrite(key1->string, sizeof(char), key1->size + 1, file2);
				fwrite(&key2->size, sizeof(int), 1, file2);
				offset[1] = ftell(file2) - sizeof(int);
				fwrite(key2->string, sizeof(char), key2->size + 1, file2);
				fwrite(&info->size, sizeof(int), 1, file2);
				offset[2] = ftell(file2) - sizeof(int);
				fwrite(info->string, sizeof(char), info->size + 1, file2);
				fwrite(&k1, sizeof(int), 1, file2);
				fwrite(&k2, sizeof(int), 1, file2);
				fseek(file2, k1 + sizeof(int), SEEK_SET);
				fwrite(&offset[0], sizeof(int), 1, file2);
				fwrite(&offset[2], sizeof(int), 1, file2);
				fseek(file2, k2, SEEK_SET);
				fwrite(&offset[1], sizeof(int), 1, file2);
				fseek(file2, sizeof(int), SEEK_CUR);
				fwrite(&offset[2], sizeof(int), 1, file2);
				fseek(file2, t->ks2 + sizeof(int) * hashf(key2, t->msize2), SEEK_SET);
				fread(&buffer, sizeof(int), 1, file2);
				if (buffer == 0) {
					fseek(file2, -4, SEEK_CUR);
					fwrite(&k2, sizeof(int), 1, file2);
				}
				else {
					int k3 = buffer;
					fseek(file2, k2 + sizeof(int), SEEK_SET);
					fwrite(&k3, sizeof(int), 1, file2);
					fseek(file2, t->ks2 + sizeof(int) * hashf(key2, t->msize2), SEEK_SET);
					fwrite(&k2, sizeof(int), 1, file2);
				}
				free_s(key1);
				free_s(key2);
				free_s(info);
			}
			a += 3 * sizeof(int);
		}
		fclose(file1);
		fclose(file2);
		remove(t->file_name->string);
		rename("buffer.bin", t->file_name->string);
		return OK;
	}
	return UN;
}
#endif
