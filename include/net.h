#ifndef NET_H
#define NET_H

#include "dsm.h"

/* P1 espera la conexión y P2 se conecta a host (la IP de P1, 127.0.0.1 si es la misma máquina) */
int net_init(int process_id, const char *host);

/* mandan y reciben un mensaje completo. Devuelven 0 si salió bien y -1 si hubo error */
int send_msg(const message_t *msg);
int recv_msg(message_t *msg);

/* descriptor del socket, main lo necesita para el select */
int net_peer_fd(void);
void net_close(void);

#endif
