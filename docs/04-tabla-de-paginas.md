# Tabla de páginas

```c
page_entry_t page_table[NUM_PAGES];
```

`page_table` es un arreglo con una entrada por página. El índice es el ID de la página, así que `page_table[5]` tiene la información de la página 5, que está en `memory[5]`.

No guarda los datos, es como un intermediario: antes de tocar `memory` se revisa la tabla. Cada entrada tiene tres propiedades:

- `valid`: 1 si la página está montada en este proceso, 0 si no. Con 0 hay fallo de página.
- `owner`: el dueño de la página, 1 es P1 y 2 es P2.
- `access`: el permiso sobre la página. 0 ninguno, 1 solo lectura y 2 lectura y escritura. Lo necesitamos para distinguir leer de escribir en el demo 2.

## Valores iniciales

| | valid | owner | access |
|---|---|---|---|
| P1 | 1 | 1 | 2 |
| P2 | 0 | 1 | 0 |

`owner` es 1 en los dos porque P2 también sabe que P1 es el dueño.
