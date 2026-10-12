#include <stdio.h>
#include "dsm.h"

page_entry_t page_table[NUM_PAGES];
char memory[NUM_PAGES][PAGE_SIZE];

