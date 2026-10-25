#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include "dsm.h"
#include "net.h"

static void print_help(void)
{
    printf("Comandos:\n");
    printf("  leer <pagina> <offset> [cantidad]   lee uno o varios bytes\n");
    printf("  escribir <pagina> <offset> <texto>  escribe el texto desde esa posicion\n");
    printf("  tabla                               muestra la tabla de paginas\n");
    printf("  ayuda                               muestra esta ayuda\n");
    printf("  salir                               termina el programa\n");
}

/* ejecuta una línea escrita por el usuario. Devuelve 1 si hay que salir */
static int run_command(char *line)
{
    char cmd[16];
    if (sscanf(line, "%15s", cmd) != 1)
        return 0; /* línea vacía */

    if (strcmp(cmd, "leer") == 0)
    {
        int page, offset, count = 1;
        if (sscanf(line, "%*s %d %d %d", &page, &offset, &count) < 2 || count < 1 || count > PAGE_SIZE)
        {
            printf("Uso: leer <pagina> <offset> [cantidad]\n");
            return 0;
        }

        char text[PAGE_SIZE + 1];
        int n = 0;
        for (; n < count; n++)
        {
            char c;
            if (leer(page, offset + n, &c) < 0)
                break;
            text[n] = c ? c : '.'; /* los bytes vacíos se muestran como punto */
        }
        text[n] = '\0';

        if (n == count && count == 1)
            printf("leer(%d, %d) = '%c' (%d)\n", page, offset, text[0] == '.' ? ' ' : text[0], text[0] == '.' ? 0 : text[0]);
        else if (n > 0)
            printf("leer(%d, %d) = \"%s\"\n", page, offset, text);
    }
    else if (strcmp(cmd, "escribir") == 0)
    {
        int page, offset, pos = 0;
        sscanf(line, "%*s %d %d %n", &page, &offset, &pos);
        if (pos == 0)
        {
            printf("Uso: escribir <pagina> <offset> <texto>\n");
            return 0;
        }

        char *text = line + pos;
        size_t len = strlen(text);
        while (len > 0 && (text[len - 1] == '\n' || text[len - 1] == '\r' || text[len - 1] == ' '))
            text[--len] = '\0';
        if (len == 0)
        {
            printf("Uso: escribir <pagina> <offset> <texto>\n");
            return 0;
        }

        size_t i = 0;
        for (; i < len; i++)
            if (escribir(page, offset + (int)i, text[i]) < 0)
                break;
        if (i > 0)
            printf("escritos %zu byte(s) en la pagina %d desde la posicion %d\n", i, page, offset);
    }
    else if (strcmp(cmd, "tabla") == 0)
        print_page_table();
    else if (strcmp(cmd, "ayuda") == 0)
        print_help();
    else if (strcmp(cmd, "salir") == 0)
        return 1;
    else
        printf("Comando desconocido: %s (escribe 'ayuda')\n", cmd);

    return 0;
}

static void prompt(void)
{
    printf("P%d> ", my_id);
    fflush(stdout);
}

/* argc es la cantidad de argumentos, argv son los argumentos como texto */
int main(int argc, char *argv[])
{
    if (argc < 2 || argc > 3)
    {
        printf("Uso: %s <1|2> [ip_de_P1]\n", argv[0]);
        return 1;
    }

    int process_id = atoi(argv[1]); /* pasa el texto a número */

    if (process_id != 1 && process_id != 2)
    {
        printf("El proceso debe ser 1 o 2\n");
        return 1;
    }

    const char *host = (argc == 3) ? argv[2] : "127.0.0.1"; /* solo lo usa P2 */

    init_page_table(process_id);

    printf("Soy P%d\n", process_id);
    if (net_init(process_id, host) < 0)
        return 1;

    print_help();
    prompt();

    /* P1 tiene que poder contestar a P2 aunque nadie esté escribiendo en su terminal,
       por eso se espera a la vez el teclado (0) y el socket con select */
    int peer = net_peer_fd();
    char input[1024];
    int quit = 0;

    while (!quit)
    {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(0, &fds);
        FD_SET(peer, &fds);

        if (select(peer + 1, &fds, NULL, NULL, NULL) < 0)
            break;

        if (FD_ISSET(peer, &fds))
        {
            message_t msg;
            if (recv_msg(&msg) < 0)
                break;
            handle_message(&msg);
            prompt();
        }

        if (FD_ISSET(0, &fds))
        {
            /* se lee con read y no con fgets, porque fgets guarda líneas en su buffer
               y select no se enteraría de ellas */
            ssize_t n = read(0, input, sizeof(input) - 1);
            if (n <= 0)
                break;
            input[n] = '\0';

            char *saveptr;
            for (char *line = strtok_r(input, "\n", &saveptr); line != NULL && !quit; line = strtok_r(NULL, "\n", &saveptr))
                quit = run_command(line);
            if (!quit)
                prompt();
        }
    }

    net_close();
    printf("\nAdios\n");
    return 0;
}
