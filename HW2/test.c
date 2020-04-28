
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
	doubll_item* begin = NULL;
	int d = 0;
	int data[] = { 7, 4, 9, 15, 36, 3, 42, 6, 107 };
	doubll_item* items[sizeof(data)/sizeof(int)];
	doubll_item* items2[sizeof(data)/sizeof(int)];

	doubll list;
	list_init(&list);
	for(i = 0; i < sizeof(data)/sizeof(int); i++){
		/*printf("%d\n", (int)(list_end(&list)->prev == list_head(&list)));*/
		items[i] = insert_list(&list, list_end(&list)->prev, &data[i], sizeof(int));
	}
	printf("%lu\n", list_size(&list));
	print_list(&list);

	for(i = 0; i < sizeof(data)/sizeof(int); i++){
		d = data[i] * 2;

		begin = list_head(&list);
		while(begin != list_end(&list)){
			if(begin == items[i]){
				break;
			}
			begin = begin->next;
		}
		/*if(begin != list_end(&list)) printf("%d\n", *(int*)begin->data);
		print_list(&list);*/
		items2[i] = insert_list(&list, items[i], &d, sizeof(int));

	}
	printf("%lu\n", list_size(&list));
	print_list(&list);

	remove_item(&list, items[0]);
	remove_item(&list, items[4]);
	remove_item(&list, items[8]);
	printf("%lu\n", list_size(&list));
	print_list(&list);

	printf("-----------------\n");
	sort_list(&list, lessInt);
	printf("%lu\n", list_size(&list));
	print_list(&list);

	purge_list(&list);
	/*printf("%lu\n", list_size(&list));
	print_list(&list);*/

	return 0;
}
