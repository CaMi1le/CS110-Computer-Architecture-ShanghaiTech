/*
 * File: heap.h
 *
 *   Declaration of a min priority queue data structure and library functions
 *     manipulating the priority queue. Feel free to add, remove, or modify
 *     these declarations to serve your algorithm.
 *
 * Jose @ ShanghaiTech University
 *
 */

#ifndef _HEAP_H_
#define _HEAP_H_

#include <pthread.h>
#include "node.h"

/* Define initial capacity to be 1000. */
#define INIT_CAPACITY 1000

/*
 * Structure of a min prority queue (min heap) of cell nodes.
 *
 */
typedef bool (*LESS_THAN)(node_t *, node_t *, int channel);
typedef void (*ITEM_MOVED)(node_t *, int position);

typedef struct heap_t
{
    int channel;        /* Which index channel of node held by this heap */

    node_t **nodes;     /* Array of node pointers. */
    int size;           /* Current size. */
    int capacity;       /* Temporary capacity. */

    /* Callback slots */
    LESS_THAN less;     /* Less comparison operator*/

} heap_t;

/* Function prototypes. */
heap_t *heap_init(int channel, LESS_THAN less);
void heap_destroy (heap_t *h);
int heap_insert (heap_t *h, node_t *n);
int heap_update (heap_t *h, node_t *n);
node_t *heap_extract(heap_t *h);
node_t *heap_peek(heap_t *h);

#if 0
/*
 * A special structure of quad-branche heap, which has high parallelity in
 * insertion and update with multithreading safty, while extraction and
 * deletion require serial execution.
 */
typedef struct quadheap_t
{
    int minBranch;
    int size;

    heap_t* branches[4];

    int channel;
    LESS_THAN less;     /* Less comparison operator*/

    /* Four mutex lock for each branch */
    pthread_spinlock_t mutexBranch[4];

} quadheap_t;

/* Function prototypes. */
quadheap_t* quadheap_init(int channel, LESS_THAN less);
void quadheap_destroy(quadheap_t* q);
void quadheap_insert(quadheap_t* q, int b, node_t* n);
void quadheap_update(quadheap_t* q, int b, node_t* n);
node_t* quadheap_extract(quadheap_t* q);
node_t* quadheap_peek(quadheap_t* q);
#endif

#endif
