# Memoria

```c
char memory[NUM_PAGES][PAGE_SIZE];
```

`memory` es una matriz. Las filas son las páginas, 16 en total, y las columnas son las posiciones dentro de cada página, 4096 en cada una (como una página de 4 KB). Es la memoria física, donde están los bytes de verdad.

Para llegar a un byte se necesitan dos datos: la página (la fila) y el offset (la posición dentro de la página). Por ejemplo, `memory[5][10]` es la posición 10 de la página 5.

## Cómo se ve

Cada fila es una página y cada columna es una posición dentro de la página.

```
                  posición (offset)
              0     1     2    ...   10    ...   4095
           +-----+-----+-----+-----+-----+-----+-----+
página 0   |     |     |     | ... |     | ... |     |
           +-----+-----+-----+-----+-----+-----+-----+
página 1   |     |     |     | ... |     | ... |     |
           +-----+-----+-----+-----+-----+-----+-----+
   ...
           +-----+-----+-----+-----+-----+-----+-----+
página 5   |     |     |     | ... | 'A' | ... |     |   <-- memory[5][10]
           +-----+-----+-----+-----+-----+-----+-----+
   ...
           +-----+-----+-----+-----+-----+-----+-----+
página 15  |     |     |     | ... |     | ... |     |
           +-----+-----+-----+-----+-----+-----+-----+
```

## Ejemplo

```c
escribir(5, 10, 'A');   /* guarda 'A' en la página 5, posición 10 */
leer(5, 10);            /* devuelve 'A' */
```

El tamaño total es 16 páginas por 4096 bytes, o sea 64 KB.
