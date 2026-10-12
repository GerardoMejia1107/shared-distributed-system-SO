#include <stdio.h>
#include "dsm.h"

int main(void)
{
    init_page_table();

    printf("Soy P1\n");
    for (int i = 0; i < NUM_PAGES; i++)
        printf("pagina %d: valid=%d owner=%d\n", i, page_table[i].valid, page_table[i].owner);

    return 0;
}
