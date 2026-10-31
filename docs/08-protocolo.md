# Protocolo de mensajes

```c
typedef struct {
    int type;             /* tipo de mensaje */
    int page;             /* página de la que se habla */
    int access;           /* permiso que recibe quien la pidió (solo en ENVIAR_PAGINA) */
    char data[PAGE_SIZE]; /* contenido de la página (solo en ENVIAR_PAGINA) */
} message_t;
```

| Mensaje | Quién lo manda | Qué significa | Qué hace quien lo recibe |
|---|---|---|---|
| `PEDIR_LECTURA` | el que no tiene la página | "dame la página para leer" | si podía escribir, pasa a solo lectura y manda `ENVIAR_PAGINA` con permiso 1 |
| `PEDIR_ESCRITURA` | el que no es dueño | "dame la página y la propiedad exclusiva" | invalida su copia, cede la propiedad y manda `ENVIAR_PAGINA` con permiso 2 |
| `ENVIAR_PAGINA` | el dueño | contenido de la página y el permiso | quien la pidió la instala |
| `INVALIDAR` | el que quiere escribir y ya tiene la página | "borra tu copia" | pone `valid=0`, `access=0`, cede la propiedad a quien invalida, borra la página y manda `ACK_INVALIDAR` |
| `ACK_INVALIDAR` | el que invalidó | "ya la borré" | quien invalidó pasa a ser dueño y puede escribir |

## Reglas que se mantienen

- El dueño siempre tiene la página montada.
- Si hay dos copias, las dos son de solo lectura. Nadie escribe con otra copia viva.
- Al invalidar se borra el contenido de `memory[page]` en ese proceso, para que se vea que la copia ya no existe.

## Limitaciones

- Solo hay dos nodos, por eso `owner` es 1 o 2 y el otro es `3 - my_id`.
- Si los dos piden la misma página exactamente al mismo tiempo, el protocolo no lo resuelve. Con un demo manual no pasa.
