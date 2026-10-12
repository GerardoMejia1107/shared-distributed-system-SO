#include <stdio.h>
#include "dsm.h"

page_entry_t page_table[NUM_PAGES];
char memory[NUM_PAGES][PAGE_SIZE];

/* inicializa la tabla de páginas, process_id es 1 para P1 y 2 para P2 */
void init_page_table(int process_id)
{
    for (int i = 0; i < NUM_PAGES; i++)
    {
        page_table[i].owner = 1; /* al inicio P1 es dueño de todas */

        if (process_id == 1)
        {
            page_table[i].valid = 1;  /* P1 tiene todas montadas */
            page_table[i].access = 2; /* y es el único que las tiene, puede escribir */
        }
        else
        {
            page_table[i].valid = 0;  /* P2 no tiene ninguna */
            page_table[i].access = 0; /* sin permiso */
        }
    }
}

