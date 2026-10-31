# Sockets

Los procesos se hablan por una conexión TCP, en el puerto `DSM_PORT` (5050).

- **P1 es el servidor:** `socket`, `bind`, `listen` y `accept`. Se queda esperando hasta que P2 se conecte, así que se arranca primero.
- **P2 es el cliente:** `socket` y `connect`. Si P1 todavía no está listo reintenta cada segundo (hasta 30 veces).

Una vez conectados los dos son iguales: cualquiera puede mandar y recibir.

## send_msg y recv_msg

TCP es un flujo de bytes, no de mensajes. `send` puede mandar menos de lo pedido y `recv` puede devolver un mensaje a pedazos, así que las dos funciones repiten el llamado hasta completar `sizeof(message_t)`. Todos los mensajes miden lo mismo, y eso lo hace simple.

Cada mensaje que sale o llega se imprime en pantalla desde ahí (`log_msg`), por eso no hay que acordarse de imprimir en cada lado.

## Por qué select

P1 puede estar sin hacer nada y P2 le pide una página. Si P1 se quedara esperando que alguien escriba en el teclado (`fgets`), no contestaría nunca. `main` usa `select` para vigilar a la vez el teclado (descriptor 0) y el socket, y atiende lo que llegue primero.

El teclado se lee con `read` y no con `fgets`, porque `fgets` guarda líneas en su buffer y `select` no se enteraría de ellas.
