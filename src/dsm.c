#include <stdio.h>
#include <string.h>
#include "dsm.h"
#include "net.h"

page_entry_t page_table[NUM_PAGES];
char memory[NUM_PAGES][PAGE_SIZE];
int my_id;

/* inicializa la tabla de páginas, process_id es 1 para P1 y 2 para P2 */
void init_page_table(int process_id)
{
    my_id = process_id;

    for (int i = 0; i < NUM_PAGES; i++)
    {
        page_table[i].owner = 1; /* al inicio P1 es dueño de todas */

        if (process_id == 1)
        {
            page_table[i].valid = 1;             /* P1 tiene todas montadas */
            page_table[i].access = ACCESS_WRITE; /* y es el único que las tiene, puede escribir */
        }
        else
        {
            page_table[i].valid = 0;            /* P2 no tiene ninguna */
            page_table[i].access = ACCESS_NONE; /* sin permiso */
        }
    }
}

/* revisa que la página y la posición existan, si no se saldría de memory */
static int check_args(int page, int offset)
{
    if (page < 0 || page >= NUM_PAGES)
    {
        printf("la pagina %d no existe (van de 0 a %d)\n", page, NUM_PAGES - 1);
        return -1;
    }
    if (offset < 0 || offset >= PAGE_SIZE)
    {
        printf("el offset %d no existe (va de 0 a %d)\n", offset, PAGE_SIZE - 1);
        return -1;
    }
    return 0;
}

/* espera una respuesta de cierto tipo. Si mientras tanto llega otra petición del otro
   proceso, la atiende y sigue esperando */
static int wait_for(int type, int page, message_t *out)
{
    while (1)
    {
        if (recv_msg(out) < 0)
            return -1;
        if (out->type == type && out->page == page)
            return 0;
        handle_message(out);
    }
}

/* fallo de página: consigue la página (o el permiso) que falta. Al volver, el acceso ya se puede hacer.
   want_write es 1 si se quiere escribir y 0 si se quiere leer */
static int handle_page_fault(int page, int want_write)
{
    page_entry_t *e = &page_table[page];
    message_t req, reply;
    memset(&req, 0, sizeof(req));
    req.page = page;

    if (e->owner == my_id)
    {
        /* yo soy el dueño y tengo la página, pero el otro tiene una copia de solo lectura.
           Para escribir hay que invalidarla antes */
        if (!want_write || e->valid == 0)
        {
            printf("error: soy dueño de la pagina %d pero no puedo accederla\n", page);
            return -1;
        }

        printf("  la pagina %d tiene una copia en el otro proceso, hay que invalidarla antes de escribir\n", page);
        req.type = MSG_INVALIDAR;
        if (send_msg(&req) < 0)
            return -1;
        if (wait_for(MSG_ACK_INVALIDAR, page, &reply) < 0)
            return -1;

        e->access = ACCESS_WRITE;
        return 0;
    }

    /* no soy el dueño: se la pido a él */
    req.type = want_write ? MSG_PEDIR_ESCRITURA : MSG_PEDIR_LECTURA;
    if (send_msg(&req) < 0)
        return -1;
    if (wait_for(MSG_ENVIAR_PAGINA, page, &reply) < 0)
        return -1;

    /* la instalo localmente */
    memcpy(memory[page], reply.data, PAGE_SIZE);
    e->valid = 1;
    e->access = reply.access;
    if (want_write)
        e->owner = my_id; /* pedí escritura, ahora yo soy el dueño */

    printf("  pagina %d instalada (permiso %s)\n", page, e->access == ACCESS_WRITE ? "lectura y escritura" : "solo lectura");
    return 0;
}

/* lee el byte que está en la posición offset de la página page y lo deja en out */
int leer(int page, int offset, char *out)
{
    if (check_args(page, offset) < 0)
        return -1;

    if (page_table[page].valid == 0 || page_table[page].access < ACCESS_READ)
    {
        printf("fallo de pagina: la pagina %d no esta montada en P%d\n", page, my_id);
        if (handle_page_fault(page, 0) < 0)
            return -1;
    }

    *out = memory[page][offset];
    return 0;
}

/* escribe el byte value en la posición offset de la página page */
int escribir(int page, int offset, char value)
{
    if (check_args(page, offset) < 0)
        return -1;

    if (page_table[page].valid == 0 || page_table[page].access != ACCESS_WRITE)
    {
        printf("fallo de pagina: P%d no tiene permiso de escritura en la pagina %d\n", my_id, page);
        if (handle_page_fault(page, 1) < 0)
            return -1;
    }

    memory[page][offset] = value;
    return 0;
}

/* atiende lo que manda el otro proceso */
void handle_message(const message_t *msg)
{
    if (msg->page < 0 || msg->page >= NUM_PAGES)
    {
        printf("llego un mensaje con una pagina invalida (%d)\n", msg->page);
        return;
    }

    int page = msg->page;
    page_entry_t *e = &page_table[page];
    message_t reply;
    memset(&reply, 0, sizeof(reply));
    reply.page = page;

    switch (msg->type)
    {
    case MSG_PEDIR_LECTURA:
        if (e->valid == 0)
        {
            printf("error: me piden la pagina %d y no la tengo\n", page);
            return;
        }
        /* si yo podía escribir, ahora hay dos copias, las dos quedan de solo lectura */
        if (e->access == ACCESS_WRITE)
        {
            e->access = ACCESS_READ;
            printf("  mi copia de la pagina %d pasa a solo lectura\n", page);
        }
        reply.type = MSG_ENVIAR_PAGINA;
        reply.access = ACCESS_READ;
        memcpy(reply.data, memory[page], PAGE_SIZE);
        send_msg(&reply);
        break;

    case MSG_PEDIR_ESCRITURA:
        if (e->valid == 0)
        {
            printf("error: me piden la pagina %d y no la tengo\n", page);
            return;
        }
        reply.type = MSG_ENVIAR_PAGINA;
        reply.access = ACCESS_WRITE;
        memcpy(reply.data, memory[page], PAGE_SIZE);

        /* primero invalido mi copia y cedo la propiedad, después mando la página */
        e->valid = 0;
        e->access = ACCESS_NONE;
        e->owner = 3 - my_id; /* el otro proceso */
        memset(memory[page], 0, PAGE_SIZE);
        printf("  invalido mi copia de la pagina %d y cedo la propiedad a P%d\n", page, e->owner);
        send_msg(&reply);
        break;

    case MSG_INVALIDAR:
        e->valid = 0;
        e->access = ACCESS_NONE;
        e->owner = 3 - my_id; /* quien invalida pasa a ser el dueño */
        memset(memory[page], 0, PAGE_SIZE);
        printf("  mi copia de la pagina %d fue invalidada\n", page);
        reply.type = MSG_ACK_INVALIDAR;
        send_msg(&reply);
        break;

    default:
        printf("llego un mensaje inesperado (tipo %d)\n", msg->type);
        break;
    }
}

void print_page_table(void)
{
    static const char *access_name[] = {"ninguno", "lectura", "lectura/escritura"};

    printf("Tabla de paginas de P%d\n", my_id);
    printf("pagina  valid  owner  access\n");
    for (int i = 0; i < NUM_PAGES; i++)
        printf("%4d    %d      P%d     %d (%s)\n", i, page_table[i].valid, page_table[i].owner,
               page_table[i].access, access_name[page_table[i].access]);
}
