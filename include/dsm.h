#ifndef DSM_H
#define DSM_H

#define NUM_PAGES 16
#define PAGE_SIZE 4096 /* posiciones dentro de cada página */
#define DSM_PORT 5050  /* puerto TCP donde P1 espera a P2 */

/* valores posibles de access */
#define ACCESS_NONE 0  /* sin permiso */
#define ACCESS_READ 1  /* solo lectura */
#define ACCESS_WRITE 2 /* lectura y escritura */

/* cada entrada guarda las propiedades de una página */
typedef struct
{
    int valid;  /* 1 si la página está montada, 0 si no */
    int owner;  /* dueño de la página, 1 es P1 y 2 es P2 */
    int access; /* permiso sobre la página: 0 ninguno, 1 solo lectura, 2 lectura y escritura */
} page_entry_t;

/* tipos de mensaje que se mandan P1 y P2 por el socket */
typedef enum
{
    MSG_PEDIR_LECTURA,   /* "dame la página para leerla" */
    MSG_PEDIR_ESCRITURA, /* "dame la página y la propiedad exclusiva para escribirla" */
    MSG_ENVIAR_PAGINA,   /* respuesta con el contenido de la página */
    MSG_INVALIDAR,       /* "borra tu copia de esta página" */
    MSG_ACK_INVALIDAR    /* "ya borré mi copia" */
} msg_type_t;

/* todos los mensajes tienen el mismo tamaño, así es más fácil leerlos del socket */
typedef struct
{
    int type;             /* un msg_type_t */
    int page;             /* ID de la página de la que se habla */
    int access;           /* en ENVIAR_PAGINA: permiso que recibe quien la pidió */
    char data[PAGE_SIZE]; /* en ENVIAR_PAGINA: el contenido de la página */
} message_t;

/* se declaran aquí y se definen en dsm.c */
extern page_entry_t page_table[NUM_PAGES]; /* el índice es el ID de la página */
extern char memory[NUM_PAGES][PAGE_SIZE];  /* filas son páginas, columnas son posiciones */
extern int my_id;                          /* 1 si soy P1, 2 si soy P2 */

void init_page_table(int process_id);
int leer(int page, int offset, char *out);   /* 0 si salió bien, -1 si hubo error */
int escribir(int page, int offset, char value);
void handle_message(const message_t *msg);   /* atiende una petición del otro proceso */
void print_page_table(void);

#endif
