#include "table.h"

#ifndef work_file
void create_table(table* t, int msize1, int msize2) {
	t->msize1 = msize1;
	t->msize2 = msize2;
	t->ks1 = (KS1*)calloc(msize1, sizeof(KS1));
	t->ks2 = (KS2**)calloc(msize2, sizeof(KS2*));
}

int add_e(table* t, item* info) {
	if (info == NULL || t->ks1 == NULL || t->ks2 == NULL)
		return UN;
	int k = 0, flag1 = 0, flag2 = 0;
	KS1* p1 = t->ks1, * p = NULL;
	for (int i = 0; i < t->msize1; i++) {
		if (compar(info->key1, p1->key) && p1->busy == 1) {
			flag1 = 1;
			break;
		}
		if (p1->busy == 0 && !flag2) {
			flag2 = 1;
			k = i;
			p = p1;
		}
		p1++;
	}
	if (flag1)
		return DB;
	if (!flag2)
		return OF;

	int a = hashf(info->key2, t->msize2);
	item* j = NULL;
	if (t->ks2[a] == NULL) {
		j = (item*)calloc(1, sizeof(item));
		KS2* l = (KS2*)calloc(1, sizeof(KS2));
		j->info = info->info;
		j->ind1 = info->ind1;
		j->p2 = info->p2;
		j->key1 = info->key1;
		j->key2 = info->key2;
		l->info = j;
		l->key = info->key2;
		l->next = NULL;
		t->ks2[a] = l;
		j->p2 = t->ks2[a];
	}
	else {
		KS2* p2 = t->ks2[a];
		if (compar(p2->key, info->key2))
			return DB;
		while (p2->next != NULL) {
			p2 = p2->next;
			if (compar(p2->key, info->key2))
				return DB;
		}
		j = (item*)calloc(1, sizeof(item));
		KS2* l = (KS2*)calloc(1, sizeof(KS2));
		j->info = info->info;
		j->key1 = info->key1;
		j->key2 = info->key2;
		l->info = j;
		l->key = info->key2;
		l->next = t->ks2[a];
		t->ks2[a] = l;
		j->p2 = t->ks2[a];
	}

	p->busy = 1;
	p->key = info->key1;
	j->ind1 = k;
	p->info = j;

	return OK;
}

int scan_KS1(table* t, string* key1, item* info) {
	if (t->ks1 != NULL && key1 != NULL) {
		for (int i = 0; i < t->msize1; i++) {
			if (compar(key1, t->ks1[i].key)) {
				if (strcp(t->ks1[i].info->key1, &info->key1))
					return UN;
				if (strcp(t->ks1[i].info->key2, &info->key2))
					return UN;
				if (strcp(t->ks1[i].info->info, &info->info))
					return UN;
				info->p2 = t->ks1[i].info->p2;
				info->ind1 = t->ks1[i].info->ind1;
				return OK;
			}
		}
	}
	return UN;
}

int scan_KS2(table* t, string* key2, item* info) {
	if (t->ks2 != NULL && key2 != NULL) {
		int a = hashf(key2, t->msize2);
		KS2* p = t->ks2[a];
		while (p != NULL) {
			if (compar(p->key, key2)) {
				if (strcp(p->info->key1, &info->key1))
					return UN;
				if (strcp(p->info->key2, &info->key2))
					return UN;
				if (strcp(p->info->info, &info->info))
					return UN;
				info->p2 = p->info->p2;
				info->ind1 = p->info->ind1;
				return OK;
			}
			p = p->next;
		}
	}
	return UN;
}

int scan(table* t, item* info, string* key1, string* key2) {
	if (key1 != NULL && key2 != NULL) {
		item* info1, * info2;
		info1 = (item*)calloc(1, sizeof(item));
		info2 = (item*)calloc(1, sizeof(item));
		int a, b;
		a = scan_KS1(t, key1, info1);
		b = scan_KS2(t, key2, info2);
		if (a == OK && b == OK && compar(info1->info, info2->info)) {
			info->info = info1->info;
			info->ind1 = info1->ind1;
			info->p2 = info1->p2;
			info->key1 = info1->key1;
			info->key2 = info1->key2;
			free_i(info2);
			free(info1);
			return OK;
		}
		free_i(info1);
		free_i(info2);
	}
	return UN;
}

int del_e_KS1(table* t, string* key1) {
	if (key1 != NULL && t->ks1 != NULL)
		for (int i = 0; i < t->msize1; i++) {
			if (t->ks1[i].busy && compar(t->ks1[i].key, key1)) {
				t->ks1[i].busy = 0;
				int a = hashf(t->ks1[i].info->key2, t->msize2);
				KS2** p = &t->ks2[a], * p1 = t->ks1[i].info->p2, * p2 = t->ks2[a];
				if (t->ks2[a] == t->ks1[i].info->p2)
					*p = t->ks2[a]->next;
				else {
					while (p2->next != t->ks1[i].info->p2)
						p2 = p2->next;
					p2->next = p1->next;
				}
				free_i(t->ks1[i].info);
				free(p1);
				return OK;
			}
		}
	return UN;
}

int del_d_KS1(table* t, string* key1, string* key2) {
	if (key1 != NULL && key2 != NULL) {
		int flag = 0;
		for (int i = 0; i < t->msize1; i++)
			if (t->ks1[i].busy) {
				if (strcmp(key1->string, t->ks1[i].key->string) < 0 && strcmp(key2->string, t->ks1[i].key->string) > 0) {
					del_e_KS1(t, t->ks1[i].key);
					flag = 1;
				}
			}
		if (flag)
			return OK;
	}
	return UN;
}

int del_e_KS2(table* t, string* key2) {
	if (key2 != NULL) {
		int a = hashf(key2, t->msize2), flag = 0;
		if (t->ks2[a] == NULL)
			return UN;
		KS2* p1 = t->ks2[a], * p2 = t->ks2[a], ** p3 = &t->ks2[a];
		do {
			if (compar(p1->key, key2)) {
				flag = 1;
				t->ks1[p1->info->ind1].busy = 0;
				free_i(p1->info);
				break;
			}
			p1 = p1->next;
		} while (p1 != NULL);
		if (flag) {
			if (p1 == p2)
				*p3 = p1->next;
			else {
				while (p2->next != p1)
					p2 = p2->next;
				p2->next = p1->next;

			}
			free(p1);
			return OK;
		}
	}
	return UN;
}

int del_e(table* t, string* key1, string* key2) {
	if (key1 != NULL && key2 != NULL) {
		item* info1, * info2; int a, b;
		info1 = (item*)calloc(1, sizeof(item));
		info2 = (item*)calloc(1, sizeof(item));
		a = scan_KS1(t, key1, info1);
		b = scan_KS2(t, key2, info2);
		if (a == OK && b == OK && compar(info1->info, info2->info)) {
			del_e_KS2(t, info1->key1);
			free_i(info1);
			free_i(info2);
			return OK;
		}
		free_i(info1);
		free_i(info2);
	}
	return UN;
}

int reorg(table* t) {
	if (t->ks1 != NULL) {
		KS1* p1 = &t->ks1[0], * p2 = &t->ks1[t->msize1 - 1];
		int k = 0;
		while (p1 < p2) {
			int flag1 = 0, flag2 = 0;
			if (p1->busy == 0)
				flag1 = 1;
			if (p2->busy == 1)
				flag2 = 1;
			if (flag1 && flag2) {
				p1->key = p2->key;
				p1->busy = 1;
				p1->info = p2->info;
				p1->info->ind1 = k;
				p2->busy = 0;
				p2->info = NULL;
				p2->key = NULL;
				flag1 = 0;
				flag2 = 0;
			}
			if (!flag1) {
				p1++;
				k++;
			}
			if (!flag2)
				p2--;
		}
		return OK;
	}
	return UN;
}

int print_table_KS1(table* t) {
	if (t->ks1 != NULL) {
		printf("key1 key2 info\n");
		for (int i = 0; i < t->msize1; i++) {
			if (t->ks1[i].busy) {
				print_string(t->ks1[i].info->key1);
				printf(" ");
				print_string(t->ks1[i].info->key2);
				printf(" ");
				print_string(t->ks1[i].info->info);
				printf("\n");
			}
		}
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
		i->info = NULL;
		i->key1 = NULL;
		i->key2 = NULL;
		free(i);
	}
}

void free_table(table* t) {
	if (t != NULL) {
		for (int i = 0; i < t->msize2; i++) {
			if (t->ks2[i] != NULL) {
				KS2* p1 = t->ks2[i];
				while (p1 != NULL) {
					t->ks1[p1->info->ind1].busy = 0;
					free_i(p1->info);
					KS2* p2 = p1;
					p1 = p1->next;
					free(p2);
				}
			}
		}
		free(t->ks1);
		free(t->ks2);
		t->ks1 = NULL;
		t->ks2 = NULL;
	}
}

int print_table_KS2(table* t) {
	if (t->ks2 != NULL) {
		for (int i = 0; i < t->msize2; i++) {
			if (t->ks2[i] != NULL)
				printf("KS2[%d]:\nkey1 key2 hash info\n", i);
			else
				printf("KS2[%d]\nNULL\n", i);
			KS2* p = t->ks2[i];
			while (p != NULL) {
				print_string(p->info->key1);
				printf(" ");
				print_string(p->key);
				printf(" %d ", hashf(p->key, t->msize2));
				print_string(p->info->info);
				printf("\n");
				p = p->next;
			}
		}
		return OK;
	}
	return UN;
}

int hashf(string* s, int m) {
	if (s == NULL)
		return 0;
	int r = 0;
	for (int i = 0; i <= s->size; i++)
		r += (int)(s->string[i]);
	return r % m;
}
#endif