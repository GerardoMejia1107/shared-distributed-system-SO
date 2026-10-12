#include <stdio.h>
#include "dsm.h"

page_entry_t page_table[NUM_PAGES];
char memory[NUM_PAGES][PAGE_SIZE];

/* inicializa la tabla de páginas de P1, tiene todas las páginas */
void init_page_table(void)
{
    for (int i = 0; i < NUM_PAGES; i++)
    {
        page_table[i].valid = 1;
        page_table[i].owner = 1;
    }
}

