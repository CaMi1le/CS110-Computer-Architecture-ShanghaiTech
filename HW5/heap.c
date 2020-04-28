/*
 * File: heap.c
 *
 *   Implementation of library functions manipulating a min priority queue.
 *     Feel free to add, remove, or modify these library functions to serve
 *     your algorithm.
 *
 * Jose @ ShanghaiTech University
 *
 */

#include <stdlib.h>     /* malloc, free */
#include <limits.h>     /* INT_MAX */

#include "heap.h"

/*
 * Initialize a min heap. The heap is constructed using array-based
 *   implementation. Returns the pointer to the new heap.
 *
 */
heap_t* heap_init(int channel, LESS_THAN less)
{
    heap_t *h = malloc(sizeof(heap_t));
    h->channel = channel;
    h->size = 0;
    h->capacity = INIT_CAPACITY;    /* Initial capacity = 1000. */
    h->nodes = malloc(INIT_CAPACITY * sizeof(node_t *));
    h->nodes[0] = node_init(0, 0, NONE); /* Remember to initial dummy node */
    h->less = less;
    return h;
}

/*
 * Delete the memory occupied by the min heap H.
 *
 */
void heap_destroy (heap_t *h)
{
    node_destroy(h->nodes[0]);
    free(h->nodes);
    free(h);
}

/*
 * Insert a node N into the min heap H.
 *
 */
int heap_insert (heap_t *h, node_t *n)
{
    int cur = ++h->size;    /* Index 0 lays dummy node, so increment first. */

    /* If will exceed current capacity, doubles the capacity. */
    if (h->size == h->capacity) {
        h->capacity *= 2;
        h->nodes = realloc(h->nodes, h->capacity * sizeof(node_t *));
    }

    h->nodes[h->size] = n;
    while (h->less(n, h->nodes[cur / 2], h->channel) && cur >= 2) {
        h->nodes[cur] = h->nodes[cur / 2];
        h->nodes[cur]->heap_id[h->channel] = cur;
        cur /= 2;
    }
    h->nodes[cur] = n;
    n->heap_id[h->channel] = cur;
    return cur;
}

/*
 * Update the min heap H in case that node N has changed its f-score.
 *
 */
int heap_update (heap_t *h, node_t *n)
{
    int cur = n->heap_id[h->channel];
    while (h->less(n, h->nodes[cur / 2], h->channel) && cur >= 2) {
        h->nodes[cur] = h->nodes[cur / 2];
        h->nodes[cur]->heap_id[h->channel] = cur;
        cur /= 2;
    }
    h->nodes[cur] = n;
    n->heap_id[h->channel] = cur;
    return cur;
}

/*
 * Extract the root (i.e. the minimum node) in min heap H. Returns the pointer
 *   to the extracted node.
 *
 */
node_t* heap_extract (heap_t *h)
{
    node_t *ret = h->nodes[1];
    node_t *last = h->nodes[h->size--];
    int cur, child;
    for (cur = 1; 2 * cur <= h->size; cur = child) {
        child = 2 * cur;
        if (child < h->size && h->less(h->nodes[child + 1], h->nodes[child], h->channel))
            child++;
        if (h->less(h->nodes[child], last, h->channel)) {
            h->nodes[cur] = h->nodes[child];
            h->nodes[cur]->heap_id[h->channel] = cur;
        } else
            break;
    }
    h->nodes[cur] = last;
    last->heap_id[h->channel] = cur;
    return ret;
}

/*
 * Extract the root (i.e. the minimum node) in min heap H.
 * This doesn't remove the root item from the heap
 */
node_t* heap_peek(heap_t *h)
{
    return h->nodes[1];
}

#if 0
/* 
 * Create 4 heap as the branches.
 * Initialize minBranch with -1, meaning no minimum branch selected.
 */
quadheap_t* quadheap_init(int channel, LESS_THAN less)
{
    int i;

    quadheap_t* q = malloc(sizeof(quadheap_t));

    /* No minimum branch selected */
    q->minBranch = -1;
    q->size = 0;
    q->channel = channel;
    q->less = less;

    /* New four heaps and create their mutex lock */
    for(i = 0; i < 4; i++){
        q->branches[i] = heap_init(channel, less);
        pthread_spinlock_init(&q->mutexBranch[i], NULL);
    }

    return q;
}

/* Destroy the quadheap 
 * Four heaps should be free here
 */
void quadheap_destroy(quadheap_t* q)
{
    int i;
    for(i = 0; i < 4; i++){
        heap_destroy(q->branches[i]);
        pthread_spinlock_destroy(&q->mutexBranch[i]);
    }
    free(q);
}

/* Insert a node to a specified branch of quadheap.
 * - The method is thread safe if and only if different threads
 * use different branch.
 * - The minBranch might be unset if any root of branches is
 * changed. For thread synchronization, it will not be updated immediately.
 */
void quadheap_insert(quadheap_t* q, int b, node_t* n)
{
    int pos = heap_insert(q->branches[b], n);
    if(pos == 1){   /* If the node is inserted to the root */
        q->minBranch = -1;
    }
    q->size++;
}

/* Update a node of a specified branch of quadheap.
 * - The method is thread safe if and only if different threads
 * use different branch.
 * - The minBranch might be unset if any root of branches is
 * changed. For thread synchronization, it will not be updated immediately.
 */
void quadheap_update(quadheap_t* q, int b, node_t* n)
{
    int pos;
    pthread_spin_lock(&q->mutexBranch[b]);
    pos = heap_update(q->branches[b], n);
    if(pos == 1){   /* If the node is inserted to the root */
        q->minBranch = -1;
    }
    pthread_spin_unlock(&q->mutexBranch[b]);
}

/* Helper: Return the minimum branch */
static int _helper_min_branch(quadheap_t* q)
{
    int i, min = -1;
    node_t *cur = NULL, *incoming;
    for(i = 0; i < 4; i++){
        if(q->branches[i]->size == 0) continue;

        incoming = heap_peek(q->branches[i]);
        if(cur == NULL || (cur != NULL && q->less(incoming, cur, q->channel))){
            cur = incoming;
            min = i;
        }
    }
    return min;
}

/* Extract the minimum node from the 4 branches.
 * - If the minBranch is set, return it immediately.
 * - If not, compare the roots of 4 branches, and obtain the minimum node.
 * The minBranch will not be set, for any extraction will remove the root
 * of a branch, causing unset.
 */
node_t* quadheap_extract(quadheap_t* q)
{
    int b;
    node_t* min = NULL;
    if(q->minBranch >= 0) b = q->minBranch;
    else b = _helper_min_branch(q);
    if(b >= 0){
        min = heap_extract(q->branches[b]);

        q->minBranch = -1;
        q->size--;
    }

    return min;
}

/* Have a peek at the top of each heap. Note that the minimum
 * branch will be selected at this stage.
 */
node_t* quadheap_peek(quadheap_t* q)
{
    int b;
    node_t* min;
    if(q->minBranch >= 0) b = q->minBranch;
    else b = _helper_min_branch(q);
    min = heap_peek(q->branches[b]);
    q->minBranch = b;
    return min;
}
#endif
