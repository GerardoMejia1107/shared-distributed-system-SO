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

## Cómo se ve

El índice de la tabla es la fila de `memory`. Son dos arreglos separados, uno con las propiedades y otro con los bytes.

```
page_table                             memory
índice  valid  owner  access           página   posición 0   1   2  ...  4095
  0       1      1      2     ------>    0      [ ][ ][ ] ... [ ]
  1       1      1      2     ------>    1      [ ][ ][ ] ... [ ]
  2       1      1      2     ------>    2      [ ][ ][ ] ... [ ]
  ...
  5       1      1      2     ------>    5      [ ][ ][ ] ... [ ]
  ...
 15       1      1      2     ------>   15      [ ][ ][ ] ... [ ]
```

Este es P1.

## P1 y P2 al inicio

Cada proceso tiene su propia copia de las dos estructuras. En P2 las filas de `memory` existen pero no están montadas.

```
          P1                                  P2
 page_table      memory             page_table      memory
 valid owner acc                    valid owner acc
 [1]   [1]  [2] --> página 0        [0]   [1]  [0]  --> (vacía)
 [1]   [1]  [2] --> página 1        [0]   [1]  [0]  --> (vacía)
 ...                                ...
```

## Cómo se usa: leer(5, 10)

```
leer(5, 10)
    |
    v
page_table[5].valid == 0 ?  --si-->  fallo de página
    |no
    v
page_table[5].access < 1 ?  --si-->  sin permiso
    |no
    v
devuelve memory[5][10]
```
