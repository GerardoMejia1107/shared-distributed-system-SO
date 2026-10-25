#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include "dsm.h"
#include "net.h"

static int peer_fd = -1; /* socket conectado con el otro proceso */

static const char *type_name(int type)
{
    switch (type)
    {
    case MSG_PEDIR_LECTURA:
        return "PEDIR_LECTURA";
    case MSG_PEDIR_ESCRITURA:
        return "PEDIR_ESCRITURA";
    case MSG_ENVIAR_PAGINA:
        return "ENVIAR_PAGINA";
    case MSG_INVALIDAR:
        return "INVALIDAR";
    case MSG_ACK_INVALIDAR:
        return "ACK_INVALIDAR";
    default:
        return "DESCONOCIDO";
    }
}

/* imprime cada mensaje que sale o llega, es lo que se ve en el demo */
static void log_msg(const char *direction, const message_t *msg)
{
    printf("  [P%d] %s %s, pagina %d", my_id, direction, type_name(msg->type), msg->page);
    if (msg->type == MSG_ENVIAR_PAGINA)
        printf(" (permiso %s)", msg->access == ACCESS_WRITE ? "lectura y escritura" : "solo lectura");
    printf("\n");
}

int net_init(int process_id, const char *host)
{
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(DSM_PORT);

    if (process_id == 1)
    {
        /* P1 es el servidor: bind, listen y accept */
        int server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd < 0)
        {
            perror("socket");
            return -1;
        }

        int yes = 1; /* para poder reiniciar el programa sin esperar a que se libere el puerto */
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            perror("bind");
            close(server_fd);
            return -1;
        }
        if (listen(server_fd, 1) < 0)
        {
            perror("listen");
            close(server_fd);
            return -1;
        }

        printf("P1 esperando a P2 en el puerto %d...\n", DSM_PORT);
        peer_fd = accept(server_fd, NULL, NULL);
        close(server_fd); /* solo hay un cliente, ya no se necesita */
        if (peer_fd < 0)
        {
            perror("accept");
            return -1;
        }
    }
    else
    {
        /* P2 es el cliente: connect, con reintentos por si P1 todavía no arranca */
        if (inet_pton(AF_INET, host, &addr.sin_addr) != 1)
        {
            printf("IP invalida: %s\n", host);
            return -1;
        }

        peer_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (peer_fd < 0)
        {
            perror("socket");
            return -1;
        }

        printf("P2 conectandose a P1 (%s:%d)...\n", host, DSM_PORT);
        int connected = 0;
        for (int i = 0; i < 30 && !connected; i++)
        {
            if (connect(peer_fd, (struct sockaddr *)&addr, sizeof(addr)) == 0)
                connected = 1;
            else
            {
                printf("P1 todavia no esta listo, reintentando...\n");
                sleep(1);
            }
        }
        if (!connected)
        {
            printf("No se pudo conectar con P1\n");
            close(peer_fd);
            return -1;
        }
    }

    /* los mensajes son pocos y pequeños, que salgan de inmediato */
    int one = 1;
    setsockopt(peer_fd, IPPROTO_TCP, TCP_NODELAY, &one, sizeof(one));

    printf("Conexion establecida\n");
    return 0;
}

/* send puede mandar menos bytes de los pedidos, por eso el ciclo */
int send_msg(const message_t *msg)
{
    const char *p = (const char *)msg;
    size_t left = sizeof(message_t);

    log_msg("envia  >>", msg);

    while (left > 0)
    {
        ssize_t n = send(peer_fd, p, left, MSG_NOSIGNAL);
        if (n < 0)
        {
            if (errno == EINTR)
                continue;
            perror("send");
            return -1;
        }
        p += n;
        left -= n;
    }
    return 0;
}

/* igual con recv: TCP es un flujo de bytes, un mensaje puede llegar en pedazos */
int recv_msg(message_t *msg)
{
    char *p = (char *)msg;
    size_t left = sizeof(message_t);

    while (left > 0)
    {
        ssize_t n = recv(peer_fd, p, left, 0);
        if (n == 0)
        {
            printf("El otro proceso cerro la conexion\n");
            return -1;
        }
        if (n < 0)
        {
            if (errno == EINTR)
                continue;
            perror("recv");
            return -1;
        }
        p += n;
        left -= n;
    }

    log_msg("recibe <<", msg);
    return 0;
}

int net_peer_fd(void)
{
    return peer_fd;
}

void net_close(void)
{
    if (peer_fd >= 0)
        close(peer_fd);
    peer_fd = -1;
}
