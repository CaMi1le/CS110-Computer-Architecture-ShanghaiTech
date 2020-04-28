/*
 * ShanghaiTech University
 * 
 * Computer Architecture I
 * 
 * HW 2
 */
#ifndef DOUBLL_H
#define DOUBLL_H

#include <stdbool.h>
#include <stddef.h>

/* Header file for a doubly linked list.
   A list item (doubll_item) has a pointer to the previous and next items (NULL
   if non-existent). 
   It also has a pointer to the data (owned by the list item) and the size of
   that data.
   The header elements of a list have a NULL poiter for data and 0 for size. */

typedef struct doubll_item doubll_item;

struct doubll_item
{
  doubll_item *prev;
  doubll_item *next;
  void *data;
  size_t size;
};

/* Our doubly linked lists have two header elements: the "head"
   just before the first element and the "tail" just after the
   last element.  The "prev" link of the front header is null, as
   is the "next" link of the back header.  Their other two links
   point toward each other via the interior elements of the list.
   The "items" is a counter for the total numbers of items in the list (Header
   elements does not count as an item).

   An empty list looks like this:

                      +------+     +------+
                  <---| head |<--->| tail |--->
                      +------+     +------+

   A list with two elements in it looks like this:

        +------+     +-------+     +-------+     +------+
    <---| head |<--->|   1   |<--->|   2   |<--->| tail |<--->
        +------+     +-------+     +-------+     +------+
*/

/* The doubly linked list. */
typedef struct doubll doubll;

struct doubll
{
	doubll_item head;
	doubll_item tail;
	size_t items;
};

/* Initialize doubly linked list. 
   There will be no testcases that list is NULL. */
void list_init (doubll *list);

/* For the following 3 functions, return NULL on error.
   Return the first element of the list. */
doubll_item *list_begin (doubll *list);

/* Return list's head. */
doubll_item *list_head (doubll *list);

/* Return list's tail. */
doubll_item *list_end (doubll *list);

/* Return number of elements in a list. 
   Return -1 on error. */
size_t list_size (doubll *list);

/* Insert a new item in the list after the item item. The data is copied. The
   new item is returned. Returns NULL on error.
   If the marco CHECK_DLLIST is set your code has to check if the item item is
   actually member of the list. This check is slow...
   If the marco CHECK_DLLIST is not set your code shall not perform that check
   but rather run very fast. */
doubll_item *insert_list (doubll *list, doubll_item *item,
                          void *data, size_t size);

/* Remove the item from the list and returns the next item. Returns NULL on
   error. 
   If the marco CHECK_DLLIST is set your code has to check if the item to be
   removed is actually member of the list. This check is slow...
   If the marco CHECK_DLLIST is not set your code shall not perform that check
   but rather run very fast. */
doubll_item *remove_item (doubll *list, doubll_item *item);

/* Purge all items from the list.
   There will be no testcases that list is NULL. */
void purge_list (doubll *list);

typedef bool list_less_func (const void *a, const void *b);

/* Sort the list in nondecreasing according to comparison function less.
   There is no strict time and space complexity requirements on the algorithm
   you choose, but you had better think of an algorithm that runs in O(n lg n)
   time and O(1) space in the number of elements in list.
   There will be no testcases that list or less is NULL. */
void sort_list (doubll *list, list_less_func *less);

#endif /* doubll.h */
