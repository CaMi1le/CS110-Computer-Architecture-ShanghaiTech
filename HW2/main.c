
#include <stdio.h>
#include "doubll.h"

void print_list (doubll *list)
{
	doubll_item* item = list_begin(list);
	while(item != list_end(list)){
		int* data = (int*)item->data;
		printf("%d", *data);
		if(item != list_end(list)->prev) printf(" -> ");
		else printf("\n");
		item = item->next;
	}
}

bool lessInt (const void *a, const void *b)
{
	int i = *(int*)a, j = *(int*)b;
	return i < j;
}

int main(void)
{
	size_t i = 0;
	int data[] = { 34, 72, 18, 7, 53, 127, 521, 31, 10, 29, 18, 66 };
	doubll_item* items[sizeof(data)/sizeof(int)];

	doubll list;
	list_init(&list);

	for(i = 0; i < sizeof(data)/sizeof(int); i++){
		items[i] = insert_list(&list, list_end(&list)->prev, &data[i], sizeof(int));
	}
	printf("Size: %lu\n", list_size(&list));
	print_list(&list);

	remove_item(&list, items[0]);
	remove_item(&list, items[3]);
	remove_item(&list, items[7]);
	printf("Size: %lu\n", list_size(&list));
	print_list(&list);

	sort_list(&list, lessInt);
	printf("Size: %lu\n", list_size(&list));
	print_list(&list);

	purge_list(&list);
	printf("Size: %lu\n", list_size(&list));
	print_list(&list);

	return 0;
}
