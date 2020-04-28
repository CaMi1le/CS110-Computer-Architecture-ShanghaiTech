/*
 * File: main.c
 *
 *   Main body of A* path searching algorithm on a block maze. The maze is
 *     given in a source file, whose name is put as a command-line argument.
 *     Then it should perform an A* search on the maze and prints the steps
 *     along the computed shortest path back to the file.
 *
 *     * Heuristic function chosen as the "Manhattan Distance":
 *
 *         heuristic(n1, n2) = |n1.x - n2.x| + |n1.y - n2.y|
 *
 *     * The whole procedure, including I/O and computing, will be time-
 *         ticked. So you are allowed to modify and optimize everything in
 *         this file and in all the libraries, as long as it satisfies:
 *
 *         1. It performs an A* path searching algorithm on the given maze.
 *
 *         2. It computes one SHORTEST (exactly optimal) path and prints the
 *              steps along the shortest path back to file, just as the
 *              original version.
 *
 *         3. Compiles with the given "Makefile". That means we are using
 *              (and only manually using) "pthread" for parallelization.
 *              Further parallelization techniques, such as OpenMP and SSE,
 *              are not required (and not allowed).
 *
 *         4. If there are multiple shortest paths, any one of them will be
 *              accepted. Please make sure you only print exactly one valid
 *              path to the file.
 *
 * Jose @ ShanghaiTech University
 *
 */

#ifndef __APPLE__

/* With which C standard (C89, C99, C11, etc.) you are compiling your code
 * decides which features are available. pthread_barrier is just an example
 * not present under C89. The following macro selects the version of POSIX, 
 * or the X/Open specification. Using 200112L for POSIX 2001, which introduced
 * pthread_barrier in.
 */
#define _POSIX_C_SOURCE 200112L

#endif

#include <stdlib.h>     /* NULL */
#include <assert.h>     /* assert */
#include <time.h>       /* clock */
#include <limits.h>     /* INT_MAX */

#include <pthread.h>    /* Multithreading */
#ifdef __APPLE__
#include "pthread_barrier.h"
#include "pthread_spinlock.h"
#endif

#include "heap.h"
#include "node.h"
#include "maze.h"
#include "compass.h"    /* The heuristic. */

/*****************************************************************************/

#ifdef __PERFORMANCE_METRIC__

#define SW_MAX_COUNT    5
#define SW_0            0
#define SW_1            1
#define SW_2            2        
#define SW_3            3    
#define SW_4            4

typedef struct stopwatch_t{
    time_t first;
    time_t last;
} stopwatch_t;

static stopwatch_t g_stopwatch[SW_MAX_COUNT];

void __stopwatch_reset(stopwatch_t *sw)
{
    if(sw != NULL) sw->first = sw->last = clock();
}

float __stopwatch_tick(stopwatch_t* sw, const char* message)
{
    float secs = -1.0f;
    if(sw != NULL){
        secs = (float)(clock() - sw->last) / (float)CLOCKS_PER_SEC;
        if(message != NULL) printf("%s: %f s\n", message, secs);
        else printf("%fs\n", secs);
        sw->last = clock();
    }
    return secs;
}

float __stopwatch_escape(stopwatch_t* sw, const char* message)
{
    float secs = -1.0f;
    if(sw != NULL){
        secs = (float)(clock() - sw->first) / (float)CLOCKS_PER_SEC;
        if(message != NULL) printf("%s: %fs\n", message, secs);
        else printf("%fs\n", secs);
    }
    return secs;
}

#define SW_RESET(N)         __stopwatch_reset(&g_stopwatch[N])
#define SW_TICK(N, S)       __stopwatch_tick(&g_stopwatch[N], S)
#define SW_ESCAPE(N, S)     __stopwatch_escape(&g_stopwatch[N], S)

#else

#define SW_RESET(N)
#define SW_TICK(N, S)
#define SW_ESCAPE(N, S)

#endif

/*****************************************************************************/

/* Local helper functions. */
static node_t *fetch_neighbour (maze_t *m, node_t *n, int direction);

/*
 * Arguments and settings for each thread
 */
typedef struct pnba_arguments_t{
    int id;
    node_t* start;
    node_t *goal;
} pnba_arguments_t;

/*
 * Multithread context, including shared information for
 * interaction and parallelism
 */
static struct pnba_context_t{
    bool finished;

    maze_t* maze;

    int F[2];
    int L;
    node_t* joint;

    /* Threads. Thread objects should be put on static data */
    pthread_t threads[2];
    /* Shared mutex lock for updating Lambda */
    pthread_mutex_t mutexL;
    /* Shared barrier lock to synchronize the initialization */
    pthread_barrier_t barrierInit;
    /* Shared barrier lock to synchronize each step(loop) in pnba */
    pthread_barrier_t barrierStep;

} g_cxt;

/* Output the cost of path and the joint point of two search process */
#ifdef __PERFORMANCE_METRIC__

void __print_pathinfo()
{
    printf("===== Path Info =====\n");
    printf("Joint: %d, %d\n", g_cxt.joint->x, g_cxt.joint->y);
    printf("Cost: %d \n", g_cxt.L);
    printf("=====================\n");
}

#define PRT_PATHINFO()      __print_pathinfo()

#else

#define PRT_PATHINFO()

#endif

/*****************************************************************************/

#ifdef __PARALLEL_NEIGHBOUR_EXP__

/*****************************************************************************/

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
        pthread_spin_init(&q->mutexBranch[i], PTHREAD_PROCESS_PRIVATE);
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
        pthread_spin_destroy(&q->mutexBranch[i]);
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

    /* if (q->channel == 0)
        printf("Insert - branch: %d, (%d, %d), fs: %d\n", b, n->x, n->y, n->fs[q->channel]);
    */
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
    /*if (q->channel == 0)
        printf("Update - branch: %d, (%d, %d), fs: %d\n", b, n->x, n->y, n->fs[q->channel]);
    */
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
    /*int i;*/
    int b;
    node_t* min = NULL;
    if(q->minBranch >= 0) b = q->minBranch;
    else b = _helper_min_branch(q);
    if(b >= 0){
        min = heap_extract(q->branches[b]);

        q->minBranch = -1;
        q->size--;
    }

    /*if (q->channel == 0){
        for (i = 0; i < 4; i++)
        {
            node_t *n;
            if (q->branches[i]->size == 0)
                continue;
            n = heap_peek(q->branches[i]);
            printf("Extract - branch: %d, (%d, %d), fs: %d, size: %d\n", i, n->x, n->y, n->fs[q->channel], q->branches[i]->size);
        }
        printf("Rt: %d\n", min->fs[q->channel]);
        printf("-------\n");
    }*/

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

/*****************************************************************************/

typedef struct neighbour_context_t{
    pnba_arguments_t*       pnba;
    bool                    stop;
    node_t*                 cur;
    quadheap_t*             q;
    /* Shared barrier lock to synchronize each step in exploration */
    pthread_barrier_t       barrierExploration;
} neighbour_context_t;

typedef struct neighbour_arguments_t{
    bool active;

    neighbour_context_t*    cxt;

    int                     branch;

    node_t*                 neighbour;
    int                     localL;

    /* Mutex lock for conditional waiting */
    pthread_mutex_t         mutexCond;
    /* Shared conditional lock to awake exploration */
    pthread_cond_t          condAwake;

} neighbour_arguments_t;

/*****************************************************************************/

void* explore_neighbours(void *arguments)
{
    neighbour_arguments_t* args = (neighbour_arguments_t*)arguments;
    neighbour_context_t* cxt = args->cxt;
    quadheap_t* q = cxt->q;
    /* Note that, here pid means partner id */
    int id = cxt->pnba->id, pid = 1 - id;
    int b = args->branch;
    int direction = args->branch;

    pthread_mutex_lock(&args->mutexCond);

    while(true){
        node_t *cur, *n;
        /* Wait for task coming in */
        /* printf("Start wait: %d, %d\n", id, b); */
        while(!args->active && !cxt->stop){
            pthread_cond_wait(&args->condAwake, &args->mutexCond);
        }
        /* printf("Pass %d, %d\n", id, b); */

        /* If stop, quit immediately*/
        if(cxt->stop){
            break;
        }

        /* Otherwise, exploration is activated */

        cur = cxt->cur;
        /*if (id == 0)
            printf("%d， %d： %d, %d, %d\n", id, b, cur->x, cur->y, cur->gs[id]);*/

        n = fetch_neighbour(g_cxt.maze, cur, direction);

        /* Restore L to infinity */
        args->localL = INT_MAX;
        /* Restore n to NULL */
        args->neighbour = NULL;

        /*if(n != NULL){
            printf("%d, %d: %d, %d, %d\n", id, b, n->x, n->y, n->gs[id]);
        }*/
        if(n != NULL && n->mark != WALL && !n->closed){
            if (n->branch[id] == -1 || n->gs[id] > cur->gs[id] + 1) {                
                n->gs[id] = cur->gs[id] + 1;
                n->fs[id] = n->gs[id] + heuristic(n, cxt->pnba->goal);
                n->parent[id] = cur;

                /*if(id == 0){
                    printf("Explore - %d, (%d, %d), fs: %d, gs: %d\n", b, n->x, n->y, n->fs[id], n->gs[id]);
                }*/

                if (n->branch[id] == -1) {
                    /* New node discovered, add into heap. */
                    n->branch[id] = b;
                    quadheap_insert(q, b, n);
                } else {
                    /* Updated old node. */
                    quadheap_update(q, n->branch[id], n);
                }

                /* Save the local minimum L. This value is candidate of
                * minimum L in once neighbourhood exploration.
                */
                if (n->gs[pid] != INT_MAX){
                    args->localL = n->gs[id] + n->gs[pid];
                    args->neighbour = n;
                }
            }
        }
        /* Inactivate exploration indicating task finished */
        args->active = false;

        /* printf("Exp barrier %d\n", id); */
        pthread_barrier_wait(&cxt->barrierExploration);
    }

    pthread_mutex_unlock(&args->mutexCond);

    return NULL;
}

void* search_thread(void *arguments)
{
    int i;
    quadheap_t *openset;
    pnba_arguments_t *pnba_args = (pnba_arguments_t*)arguments;
    /* Note that, here pid means partner id */
    int id = pnba_args->id, pid = 1 - id;

    /* Context, configurations for neighbourhood exploration */
    neighbour_context_t cxt;
    pthread_attr_t attr;
    pthread_t neighbour_threads[4];
    neighbour_arguments_t neighbour_args[4];

    openset = quadheap_init(id, node_cost_less);
    pnba_args->start->gs[id] = 0;
    pnba_args->start->fs[id] = heuristic(pnba_args->start, pnba_args->goal);
    g_cxt.F[id] = pnba_args->start->fs[id];
    quadheap_insert(openset, 0, pnba_args->start);

    /* Keep thread polling for activation */
    cxt.stop = false;
    /* Pass pnba arguments */
    cxt.pnba = pnba_args;
    /* Pass min heap */
    cxt.q = openset;
    /* Initialize barrier, synchronizing 5 threads */
    pthread_barrier_init(&cxt.barrierExploration, NULL, 5);

    /* Basically initialize neighbour arguments */
    for(i = 0; i < 4; i++){
        neighbour_args[i].active = false;   /* Inactive by default */
        neighbour_args[i].cxt = &cxt;
        neighbour_args[i].branch = i;

        /* Initialize conditional lock to awake exploration */
        pthread_cond_init(&neighbour_args[i].condAwake, NULL);
        /* Initialize the mutex for conditional waiting */
        pthread_mutex_init(&neighbour_args[i].mutexCond, NULL);
    }

    /* Create threads to perform the A* pathfinding  */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    /* Create and start threads */
    for(i = 0; i < 4; i++) {
        pthread_create(&neighbour_threads[i], &attr, explore_neighbours, (void *)&neighbour_args[i]);
    }
    pthread_attr_destroy(&attr);

    /* Loop and repeatedly extracts the node with the highest f-score to
       process on. */
    while (!g_cxt.finished) {
        if(openset->size > 0) {
            node_t *cur = quadheap_extract(openset);

            if (!cur->closed){
                int minL = cur->gs[id] + g_cxt.F[pid] - heuristic(cur, pnba_args->start);
                /* Make sure the next two comparison statements use the same L */
                int L = g_cxt.L;

                /*if(id == 0)
                    printf("** %d： %d, %d, %d\n", id, cur->x, cur->y, cur->gs[id]);*/

                if (cur->fs[id] < L && minL < L) {
                    int l;
                    node_t* n;

                    /* Check all the neighbours. Since we are using a block maze, at most
                    four neighbours on the four directions. */
                    cxt.cur = cur;

                    for(i = 0; i < 4; i++){
                        /* Activate thread initially */
                        pthread_mutex_lock(&neighbour_args[i].mutexCond);
                        neighbour_args[i].active = true;
                        /* printf("Signal: %d, %d\n", id, i); */
                        pthread_cond_signal(&neighbour_args[i].condAwake);
                        pthread_mutex_unlock(&neighbour_args[i].mutexCond);
                    }

                    /* Sychronize the exploration */
                    pthread_barrier_wait(&cxt.barrierExploration);
                    /* printf("Exp finished %d\n", id); */

                    /* Obtain the minimum L value while waiting for
                     * all neighbourhood exploration finished.
                     */
                    l = INT_MAX;
                    for(i = 0; i < 4; i++){
                        /* while(neighbour_args[i].active); */
                        if(neighbour_args[i].localL < l){
                            l = neighbour_args[i].localL;
                            n = neighbour_args[i].neighbour;
                        }
                    }

                    /* There's a design to prevent block due to issues
                     * on synchronization. Check before mutex lock can
                     * prevent a large probability on waiting for checking
                     */
                    if (l < g_cxt.L) {
                        pthread_mutex_lock(&g_cxt.mutexL);
                        if (l < g_cxt.L) {
                            g_cxt.L = l;
                            g_cxt.joint = n;
                        }
                        pthread_mutex_unlock(&g_cxt.mutexL);
                    }
                }

                cur->closed = true;
            }
        }

        if(openset->size > 0){
            g_cxt.F[id] = quadheap_peek(openset)->fs[id];
        }else{
            g_cxt.finished = true;
        }
    }

    /* Now send a signal that all neighborhood exploration should stop */
    cxt.stop = true;
    for(i = 0; i < 4; i++){
        /* Activate thread initially */
        pthread_mutex_lock(&neighbour_args[i].mutexCond);
        pthread_cond_signal(&neighbour_args[i].condAwake);
        pthread_mutex_unlock(&neighbour_args[i].mutexCond);
    }

    /* Wait for the their termination */
    for(i = 0; i < 4; i++) {
        pthread_join(neighbour_threads[i], NULL);
        /* Destroy conditional lock */
        pthread_cond_destroy(&neighbour_args[i].condAwake);
        /* Destroy associative mutex */
        pthread_mutex_destroy(&neighbour_args[i].mutexCond);
    }

    /* Destroy barrier */
    pthread_barrier_destroy(&cxt.barrierExploration);

    quadheap_destroy(openset);

    return NULL;
}

#else

void* search_thread(void *arguments)
{
    heap_t *openset;
    pnba_arguments_t *pnba_args = (pnba_arguments_t*)arguments;
    /* Note that, here pid means partner id */
    int id = pnba_args->id, pid = 1 - id;

    openset = heap_init(id, node_cost_less);
    pnba_args->start->gs[id] = 0;
    pnba_args->start->fs[id] = heuristic(pnba_args->start, pnba_args->goal);
    g_cxt.F[id] = pnba_args->start->fs[id];
    heap_insert(openset, pnba_args->start);

    pthread_barrier_wait(&g_cxt.barrierInit);

    /* Loop and repeatedly extracts the node with the highest f-score to
       process on. */
    while (!g_cxt.finished) {
        if(openset->size > 0) {
            int direction;
            node_t *cur = heap_extract(openset);

            if (!cur->closed){
                int minL = cur->gs[id] + g_cxt.F[pid] - heuristic(cur, pnba_args->start);
                /* Make sure the next two comparison statements use the same L */
                int L = g_cxt.L;
                if (cur->fs[id] < L && minL < L) {
                    /* Check all the neighbours. Since we are using a block maze, at most
                    four neighbours on the four directions. */
                    for (direction = 0; direction < 4; ++direction) {
                        node_t *n = fetch_neighbour(g_cxt.maze, cur, direction);
                        if(n == NULL || n->mark == WALL || n->closed) continue;
                        if (!n->opened[id] || n->gs[id] > cur->gs[id] + 1) {
                            n->gs[id] = cur->gs[id] + 1;
                            n->fs[id] = n->gs[id] + heuristic(n, pnba_args->goal);
                            n->parent[id] = cur;

                            if (!n->opened[id]) {
                                /* New node discovered, add into heap. */
                                n->opened[id] = true;
                                heap_insert(openset, n);
                            } else {
                                /* Updated old node. */
                                heap_update(openset, n);
                            }

                            /* There's a design to prevent block due to issues
                            * on synchronization. Check before mutex lock can
                            * prevent a large probability on waiting for checking
                            */
                            if (n->gs[pid] != INT_MAX){
                                int l = n->gs[id] + n->gs[pid];
                                if (l < g_cxt.L) {
                                    pthread_mutex_lock(&g_cxt.mutexL);
                                    if (l < g_cxt.L) {
                                        g_cxt.L = l;
                                        g_cxt.joint = n;
                                    }
                                    pthread_mutex_unlock(&g_cxt.mutexL);
                                }
                            }
                        }
                    }
                }

                cur->closed = true;
            }
        }

        if(openset->size > 0){
            g_cxt.F[id] = heap_peek(openset)->fs[id];
        }else{
            g_cxt.finished = true;
        }
    }

    heap_destroy(openset);

    return NULL;
}

#endif

/*
 * Entrance point. Time ticking will be performed on the whole procedure,
 *   including I/O. Parallelize and optimize as much as you can.
 *
 */
int main(int argc, char *argv[])
{
    int i;

    node_t *n;
    maze_t *maze;
    heap_t *pathset;

    /* Attributes of threads */
    pthread_attr_t attr;

    /* Arguments for each thread, make sure these arguments have
     * through-program life time.
     */
    pnba_arguments_t arguments[2];

    assert(argc == 2);  /* Must have given the source file name. */

    /* Stopwatch for running time calculation */
    SW_RESET(SW_0);

    /* Initializations. */
    maze = maze_init(argv[1]);

    g_cxt.finished = false;
    g_cxt.maze = maze;
    g_cxt.L = INT_MAX;
    g_cxt.joint = NULL;

    /* Fill out arguments, reverse start and goal for two threads */

    arguments[0].id = 0;
    arguments[0].start = maze->start;
    arguments[0].goal = maze->goal;
    
    arguments[1].id = 1;
    arguments[1].start = maze->goal;
    arguments[1].goal = maze->start;

    /* Initialize mutex for multithreading */
    pthread_mutex_init(&g_cxt.mutexL, NULL);
    /* Initialize barrier for multithreading */
    pthread_barrier_init(&g_cxt.barrierInit, NULL, 2);
    pthread_barrier_init(&g_cxt.barrierStep, NULL, 2);

    SW_TICK(SW_0, "Initialize maze");

    /* Create threads to perform the A* pathfinding  */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for(i = 0; i < 2; i++) {
        /* 
        Each thread works on different end, and do a bidirectional pathfinding.
        */
        pthread_create(&g_cxt.threads[i], &attr, search_thread, (void *)&arguments[i]);
    }

    pthread_attr_destroy(&attr);

    /* Wait on the other threads */
    for(i = 0; i < 2; i++) {
        pthread_join(g_cxt.threads[i], NULL);
    }
    /* Clean up creations */
    pthread_mutex_destroy(&g_cxt.mutexL);
    pthread_barrier_destroy(&g_cxt.barrierInit);
    pthread_barrier_destroy(&g_cxt.barrierStep);

    SW_TICK(SW_0, "Pathfinding");

    /* Output path information */
    if(g_cxt.joint != NULL) PRT_PATHINFO();

    /* Print the steps back. */

    pathset = heap_init(CHANNEL_SORTING, node_coord_less);

    n = g_cxt.joint;
    while (n != NULL && n->mark != START) {
        /*maze_print_step(maze, n);*/
        heap_insert(pathset, n);
        n = n->parent[CHANNEL_THREAD_ONE];
    }
    n = g_cxt.joint;
    while (n != NULL && n->mark != GOAL) {
        /*maze_print_step(maze, n);*/
        heap_insert(pathset, n);
        n = n->parent[CHANNEL_THREAD_TWO];
    }
    maze_print_steps(maze, pathset, '*');

    heap_destroy(pathset);

    SW_TICK(SW_0, "Output path");

    /* Free resources and return. */
    maze_destroy(maze);

    SW_ESCAPE(SW_0, "All time");

    return 0;
}

/*
 * Fetch the neighbour located at direction DIRECTION of node N, in the
 *   maze M. Returns pointer to the neighbour node.
 *
 */
static node_t *
fetch_neighbour (maze_t *m, node_t *n, int direction)
{
    switch (direction) {
        case 0: return maze_get_cell(m, n->x, n->y - 1);
        case 1: return maze_get_cell(m, n->x + 1, n->y);
        case 2: return maze_get_cell(m, n->x, n->y + 1);
        case 3: return maze_get_cell(m, n->x - 1, n->y);
    }
    return NULL;
}
