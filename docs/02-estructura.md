# Estructura del código

- `include/dsm.h`: las constantes, el struct, los tipos de mensaje y los nombres de las funciones.
- `include/net.h`: los nombres de las funciones de red.
- `src/dsm.c`: `page_table`, `memory`, `init_page_table`, `leer`, `escribir`, el manejo del fallo de página y la atención de mensajes del otro proceso.
- `src/net.c`: los sockets (`net_init`, `send_msg`, `recv_msg`) y la impresión de cada mensaje.
- `src/main.c`: lee los argumentos, conecta los procesos y corre el ciclo de comandos.
- `Makefile`: con `make` se compila.

Los `.h` dicen qué existe y los `.c` tienen el código.
