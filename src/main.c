#include <stdio.h>
#include <stdlib.h>
#include "dsm.h"

/* argc es la cantidad de argumentos, argv son los argumentos como texto */
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Uso: %s <1|2>\n", argv[0]);
        return 1;
    }

    int process_id = atoi(argv[1]); /* pasa el texto a número */

    if (process_id != 1 && process_id != 2)
    {
        printf("El proceso debe ser 1 o 2\n");
        return 1;
    }

    init_page_table(process_id);

    printf("Soy P%d\n", process_id);
    for (int i = 0; i < NUM_PAGES; i++)
        printf("pagina %d: valid=%d owner=%d\n", i, page_table[i].valid, page_table[i].owner);

    return 0;
}
