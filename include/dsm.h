#ifndef DSM_H
#define DSM_H

#define NUM_PAGES 16
#define PAGE_SIZE 4096 /* posiciones dentro de cada página */

/* cada entrada guarda las propiedades de una página */
typedef struct
{
    int valid; /* 1 si la página está montada, 0 si no */
    int owner; /* dueño de la página, 1 es P1 y 2 es P2 */
} page_entry_t;

/* se declaran aquí y se definen en dsm.c */
extern page_entry_t page_table[NUM_PAGES]; /* el índice es el ID de la página */
extern char memory[NUM_PAGES][PAGE_SIZE];  /* filas son páginas, columnas son posiciones */

#endif
