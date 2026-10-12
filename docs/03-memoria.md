# Memoria

```c
char memory[NUM_PAGES][PAGE_SIZE];
```

`memory` es una matriz. Las filas son las páginas, 16 en total, y las columnas son las posiciones dentro de cada página, 4096 en cada una (como una página de 4 KB). Es la memoria física, donde están los bytes de verdad.

Para llegar a un byte se necesitan dos datos: la página (la fila) y el offset (la posición dentro de la página). Por ejemplo, `memory[5][10]` es la posición 10 de la página 5.
