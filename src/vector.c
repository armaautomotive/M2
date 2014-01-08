#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vector.h"
 
void vector_init(vector *v)
{
	v->data = NULL;
	v->size = 0;
	v->count = 0;
}
 
int vector_count(vector *v)
{
	return v->count;
}
 
int vector_add(vector *v, void *e)
{
	if (v->size == 0) {
		v->size = 10;
		v->data = malloc(sizeof(void*) * v->size);
		memset(v->data, '\0', sizeof(void) * v->size);
	}
 
	// condition to increase v->data:
	// last slot exhausted
	if (v->size == v->count) {
		v->size *= 2;
		v->data = realloc(v->data, sizeof(void*) * v->size);
	}
 
	v->data[v->count] = e;
	v->count++;
	return v->count-1;
}
 
void vector_set(vector *v, int index, void *e)
{
	if (index >= v->count) {
		return;
	}
 
	v->data[index] = e;
}
 
void *vector_get(vector *v, int index)
{
	if (index >= v->count) {
		return NULL;
	}
 
	return v->data[index];
}
 
void vector_delete(vector *v, int index)
{
	if (index >= v->count) {
		return;
	}
 
	v->data[index] = NULL;
 
	int i, j;
	void **newarr = (void**)malloc(sizeof(void*) * v->count * 2);
	for (i = 0, j = 0; i < v->count; i++) {
		if (v->data[i] != NULL) {
			newarr[j] = v->data[i];
			j++;
		}		
	}
 
	free(v->data);
 
	v->data = newarr;
	v->count--;
}
 
void vector_free(vector *v)
{
	free(v->data);
}

/* 
int main(void)
{
	vector v;
	vector_init(&v);
 
	int a = vector_add(&v, "emil");
	printf(" first item index %d \n", a);
	a = vector_add(&v, "hannes");
	printf(" second %d \n", a);
	vector_add(&v, "lydia");
	vector_add(&v, "olle");
	vector_add(&v, "erik");
 
	int i;
	printf("first round:\n");
	for (i = 0; i < vector_count(&v); i++) {
		printf("%s\n", (char *)vector_get(&v, i));
	}
 
	vector_delete(&v, 1);
	vector_delete(&v, 3);
 
	printf("second round:\n");
	for (i = 0; i < vector_count(&v); i++) {
		printf("%s\n", (char*)vector_get(&v, i));
	}
 
	vector_free(&v);
 
	return 0;
}
*/
