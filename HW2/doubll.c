
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "doubll.h"

/* Initialize doubly linked list. 
   There will be no testcases that list is NULL. */
void list_init (doubll *list)
{
	/* Initialize head */
	list->head.prev = NULL;
	list->head.next = &list->tail;
	list->head.data = NULL;
	list->head.size = 0;
	/* Initialize tail */
	list->tail.prev = &list->head;
	list->tail.next = NULL;
	list->tail.data = NULL;
	list->tail.size = 0;
	/* Initialize size */
	list->items = 0;
}

static bool is_list_valid(doubll *list)
{
	/* The head shoud not contain prev, while tail should not contain next */
	bool valid = false;
	if(list){
		valid = (list->head.prev == NULL) && (list->head.next != NULL) && (list->tail.prev != NULL) && (list->tail.next == NULL) ;
	}
	return valid;
}

/* For the following 3 functions, return NULL on error.
   Return the first element of the list. */
doubll_item *list_begin (doubll *list)
{
	doubll_item* item = NULL;
	if(is_list_valid(list)) item = list->head.next;
	return item;
}

/* Return list's head. */
doubll_item *list_head (doubll *list)
{
	doubll_item* item = NULL;
	if(is_list_valid(list)) item = &list->head;
	return item;
}

/* Return list's tail. */
doubll_item *list_end (doubll *list)
{
	doubll_item* item = NULL;
	if(is_list_valid(list)) item = &list->tail;
	return item;
}

/* Return number of elements in a list. 
   Return -1 on error. */
size_t list_size (doubll *list)
{
	size_t size = (size_t)-1;
	if(is_list_valid(list)){
		size = list->items;
	}
	return size;
}

/* Insert a new item in the list after the item item. The data is copied. The
   new item is returned. Returns NULL on error.
   If the marco CHECK_LIST is set your code has to check if the item item is
   actually member of the list. This check is slow...
   If the marco CHECK_LIST is not set your code shall not perform that check
   but rather run very fast. */
doubll_item *insert_list (doubll *list, doubll_item *item, void *data, size_t size)
{
	doubll_item* new_item = NULL;
	bool check = true;
#ifdef CHECK_LIST
	doubll_item* begin = NULL;
#endif
	/* Insertion after tail are not allowed */
	if(is_list_valid(list) && item && item->next && data && size > 0){
#ifdef CHECK_LIST
		check = false;
		/* Insertion  after head is also allowed */
		begin = &list->head;
		while(begin != &list->tail){
			if(begin == item){
				check = true;
				break;
			}
			begin = begin->next;
		}
#endif
		if(check){
			/* Memory allocation */
			new_item = (doubll_item*)malloc(sizeof(doubll_item));
			/* Maintain link list structure */
			new_item->prev = item;
			new_item->next = item->next;
			item->next->prev = new_item;
			item->next = new_item;
			/* Copy data */
			new_item->size = size;
			new_item->data = (void*)malloc(size);
			memcpy(new_item->data, data, size);

			list->items++;
		}
	}
	return new_item;
}

/* Remove the item from the list and returns the next item. Returns NULL on
   error. 
   If the marco CHECK_LIST is set your code has to check if the item to be
   removed is actually member of the list. This check is slow...
   If the marco CHECK_LIST is not set your code shall not perform that check
   but rather run very fast. */
doubll_item *remove_item (doubll *list, doubll_item *item)
{
	doubll_item* next_item = NULL;
	bool check = true;
#ifdef CHECK_LIST
	doubll_item* begin = NULL;
#endif

	/* Head and tail are not allowed to be removed */
	if(is_list_valid(list) && item && item->prev && item->next && item->data){
#ifdef CHECK_LIST
		check = false;
		/* Removal is not allowed on head and tail, hence head is not involved */
		begin = list->head.next;
		while(begin != &list->tail){
			if(begin == item){
				check = true;
				break;
			}
			begin = begin->next;
		}
#endif
		if(check){
			next_item = item->next;
			/* Maintain link list structure */
			item->prev->next = item->next;
			item->next->prev = item->prev;
			/* Destory data */
			free(item->data);
			item->data = NULL;
			item->size = 0;
			/* Destroy the item itself */
			free(item);
			list->items--;
		}
	}
	return next_item;
}

/* Purge all items from the list.
   There will be no testcases that list is NULL. */
void purge_list (doubll *list)
{
	if(is_list_valid(list)){
		doubll_item* item = list->head.next;
		doubll_item* next = NULL;
		while(item != &list->tail){
			next = item->next;
			/* Destory data */
			free(item->data);
			item->data = NULL;
			item->size = 0;
			/* Destroy the item itself */
			free(item);
			item = next;
		}
		/* Restore the structure of head and tail with an empty list body */
		list->head.next = &list->tail;
		list->tail.prev = &list->head;
		list->items = 0;
	}
}

/* Helper functions for sorting algorithm */
static void swap_item(doubll_item** item_0, doubll_item** item_1)
{
	doubll_item *s = *item_0, *t = *item_1;
	doubll_item* tmp = NULL;
	/* Exchange prev field */
	tmp = s->prev;
	s->prev = t->prev;
	if(t->prev) t->prev->next = s;
	t->prev = tmp;
	if(tmp) tmp->next = t;
	/* Exchange next field */
	tmp = s->next;
	s->next = t->next;
	if(t->next) t->next->prev = s;
	t->next = tmp;
	if(tmp) tmp->prev = t;
	/* Exchange two pointers of items */
	(*item_0) = t;
	(*item_1) = s;
}

static doubll_item* partition(doubll_item** first, doubll_item** last, list_less_func *less)
{
	doubll_item* left = (*first)->prev;
	doubll_item* right = (*last)->next;
	void* pivot = (*last)->data;	/* Select the last one as pivot by default */
	while(true){
		/* Loop until left->data >= pivot */
		do{ left = left->next; }while(less(left->data, pivot) && left != right);
		if(left == right) break;
		/* Loop until right->data <= data */
		do{ right = right->prev; }while(less(pivot, right->data) && left != right);
		if(left == right) break;
		/* Update two end points in first priority */
		if(left == *first) (*first) = right;
		if(right == *last) (*last) = left;
		/* Swap two items. Note the values
		   of left and right are both exchanged*/
		swap_item(&left, &right);
	}
	return left;
}

static void quick_sort(doubll_item* first, doubll_item* last, list_less_func *less)
{
	doubll_item* middle = NULL;
	if(first != last){
		middle = partition(&first, &last, less);
		quick_sort(first, middle->prev, less);
		quick_sort(middle, last, less);
	}
}

/* Sort the list in nondecreasing according to comparison function less.
   There is no strict time and space complexity requirements on the algorithm
   you choose, but you had better think of an algorithm that runs in O(n lg n)
   time and O(1) space in the number of elements in list.
   There will be no testcases that list or less is NULL. */
void sort_list (doubll *list, list_less_func *less)
{
	/* Empty list will never be sorted */
	if(is_list_valid(list) && less && list->items > 0) quick_sort(list->head.next, list->tail.prev, less);
}
