/*
 * File: maze.c
 *
 *   Implementation of library functions manipulating a minecraft-style block
 *     maze. Feel free to add, remove, or modify these library functions to
 *     serve your algorithm.
 *
 * Jose @ ShanghaiTech University
 *
 */

#include <stdlib.h>     /* abs, malloc, free */
#include <assert.h>     /* assert */
#include "maze.h"
#include "node.h"
#include "heap.h"

/* Local helpers prototype. */
static mark_t char_to_mark (char c);

/*
 * Initialize a maze from a formatted source file named FILENAME. Returns the
 *   pointer to the new maze.
 *
 */
maze_t *
maze_init (char *filename)
{
    int rows, cols, i, j;
    maze_t *m = malloc(sizeof(maze_t));
    /* Buffer for incoming line */
    int bufSize;
    char* rowBuf;

    /* Open the source file and read in number of rows & cols. */
    m->file = fopen(filename, "r+");    /* Open with "r+" since might modify */
                                        /* at maze_print_step.               */
    fscanf(m->file, "%d %d\n", &rows, &cols);
    m->rows = rows;
    m->cols = cols;
    /* Record the position of the beginning of the maze grids section */
    m->maze_start = ftell(m->file);

    /* Two more byte for EOL and \0 respectively */
    bufSize = m->cols + 2;

    /* Allocate space for all nodes (cells) inside the maze, then read them
       in from the source file. The maze records all pointers to its nodes
       in an array NODES. */
    m->nodes = malloc(rows * cols * sizeof(node_t *));

    rowBuf = malloc(bufSize * sizeof(char));
    for (i = 0; i < rows; ++i) {
        /* Read a whole line at once */
        fgets(rowBuf, bufSize, m->file);
        for (j = 0; j < cols; ++j) {
            /* mark_t mark = char_to_mark(fgetc(m->file)); */
            mark_t mark = char_to_mark(rowBuf[j]);
            maze_set_cell(m, i, j, mark);
            if (mark == START)
                m->start = maze_get_cell(m, i, j);
            else if (mark == GOAL)
                m->goal  = maze_get_cell(m, i, j);
        }
        /* while ((c = fgetc(m->file)) != '\n' && c != EOF); */
    }

    free(rowBuf);

    return m;
}

/*
 * Delete the memory occupied by the maze M.
 *
 */
void
maze_destroy (maze_t *m)
{
    int i, j;
    for (i = 0; i < m->rows; ++i)
        for (j = 0; j < m->cols; ++j)
            node_destroy(m->nodes[i * m->cols + j]);
    free(m->nodes);
    fclose(m->file);
    free(m);
}

/*
 * Initialize a cell in maze M at position (X, Y) to be a MARK-type cell.
 *
 */
void
maze_set_cell (maze_t *m, int x, int y, mark_t mark)
{
    assert(x >= 0 && x < m->rows &&
           y >= 0 && y < m->cols);
    m->nodes[x * m->cols + y] = node_init(x, y, mark);
}

/*
 * Get the cell in maze M at position (X, Y). Returns the pointer to the
 *   specified cell.
 *
 */
node_t *
maze_get_cell (maze_t *m, int x, int y)
{
    if (x < 0 || x >= m->rows ||    /* Returns NULL if exceeds boundary. */
        y < 0 || y >= m->cols)
        return NULL;

    return m->nodes[x * m->cols + y];   /* Notice the row-major order. */
}

/*
 * Sets the position of node N in maze M in the source file to be "*",
 *   indicating it is an intermediate step along the shortest path.
 *
 */
void
maze_print_step (maze_t *m, node_t *n)
{
    int i;
    fseek(m->file, 0, SEEK_SET);
    while (fgetc(m->file) != '\n');
    for (i = 0; i < n->x; ++i)
        while (fgetc(m->file) != '\n');
    for (i = 0; i < n->y; ++i)
        fgetc(m->file);
    fputc('*', m->file);
}

void
maze_print_steps (maze_t *m, heap_t *h, char ch)
{
    /* One character more for EOL */
    const int charsOfLine = m->cols + 1;
    /* Buffer for a line */
    /* One characters more for \0 */
    char* rowArr = malloc(charsOfLine + 1);
    int currentRow;

    /* Seek to the beginning of maze grids */
    fseek(m->file, m->maze_start, SEEK_SET);

    /* Initializae for the first read */
    currentRow = 0;
    /* Initializae for the first read, retrieve the first whole line */
    fgets(rowArr, charsOfLine + 1, m->file);
    fseek(m->file, -charsOfLine, SEEK_CUR);

    /* Go through map row by row */
    while(h->size > 0){
        node_t* n = heap_extract(h);

        if(currentRow < n->x){
            int byteOffset;

            /* Put the previous line */
            fputs(rowArr, m->file);

            /* Make a shortcut jump */
            byteOffset = charsOfLine * (n->x - currentRow - 1);
            fseek(m->file, byteOffset, SEEK_CUR);
            currentRow = n->x;

            /* Once the current row changed, read the whole line */
            fgets(rowArr, charsOfLine + 1, m->file);

            /* Seek backward to the beginning of line */
            fseek(m->file, -charsOfLine, SEEK_CUR);
        }
        
        rowArr[n->y] = ch;
    }

    /* Put the last line of the maze */
    fputs(rowArr, m->file);

    free(rowArr);
}

/*
 * Maps a character C to its corresponding mark type. Returns the mark.
 *
 */
static mark_t
char_to_mark (char c)
{
    switch (c) {
        case '#': return WALL;
        case '@': return START;
        case '%': return GOAL;
        default:  return NONE;
    }
}
