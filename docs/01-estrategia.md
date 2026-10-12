# Estrategia

Básicamente tenemos dos procesos, P1 y P2, corriendo en la misma máquina. Simulan que comparten memoria, pero cada uno tiene la suya y su propia tabla de páginas. Lo que uno necesita del otro se lo pide por sockets.

Al inicio P1 es dueño de todas las páginas. Si P2 quiere una página que no tiene montada, hay un fallo de página y en vez de ir a disco se la pide a P1.

Para simplificar, la página N es la fila N de `memory`. No hay traducción de dirección virtual a física porque los dos procesos están en la misma máquina.

## Orden de trabajo

1. Memoria y tabla de páginas (listo)
2. `leer` y `escribir` (listo)
3. Sockets entre P1 y P2
4. Mensajes para pedir y enviar páginas, todos impresos en pantalla
5. Demo 1: P2 lee una página de P1
6. Demo 2: P2 escribe y P1 invalida su copia
