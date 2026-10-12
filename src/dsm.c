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

/* lee el byte que está en la posición offset de la página page */
char leer(int page, int offset)
{
    if (page_table[page].valid == 0)
    {
        printf("fallo de pagina: la pagina %d no esta montada\n", page);
        return 0;
    }

    if (page_table[page].access < 1)
    {
        printf("sin permiso de lectura en la pagina %d\n", page);
        return 0;
    }

    return memory[page][offset];
}

