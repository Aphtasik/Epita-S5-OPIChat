#include "basic_client.h"

#include <err.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int create_and_connect(struct addrinfo *addrinfo)
{
    struct addrinfo *rp = addrinfo;
    int socketfd;
    for (; rp != NULL; rp = rp->ai_next)
    {
        socketfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (socketfd == -1)
        {
            close(socketfd);
            continue;
        }
        if (connect(socketfd, rp->ai_addr, rp->ai_addrlen) != -1)
            break;
    }
    if (rp == NULL)
        return 1;
    return socketfd;
}

int prepare_socket(const char *ip, const char *port)
{
    struct addrinfo addr;
    struct addrinfo *res;
    memset(&addr, 0, sizeof(struct addrinfo));

    int ret = getaddrinfo(ip, port, &addr, &res);
    if (ret != 0)
        return 1;

    int socket = create_and_connect(res);
    freeaddrinfo(res);
    return socket;
}

void communicate(int server_socket)
{
    if (server_socket == 1)
        errx(EXIT_FAILURE, "socket error");

    char *line = NULL;
    size_t len = 0;
    ssize_t bytes;

    puts("Enter your message: ");
    while ((bytes = getline(&line, &len, stdin)) != -1)
    {
        bytes = send(server_socket, line, bytes, 0);
        if (bytes < 0)
            errx(EXIT_FAILURE, "send error");

        else if (bytes == 1 || bytes == 0)
            break;

        if ((bytes = recv(server_socket, line, bytes + 5, 0)) < 0)
            errx(EXIT_FAILURE, "recv error");

        write(1, "Server anwsered: ", 17);
        write(1, line, bytes);
        write(1, "Enter your message:\n", 20);
    }
    free(line);
    close(server_socket);
}

int main(int argc, char **argv)
{
    if (argc != 3)
        printf("Usage: ./basic_client SERVER_IP SERVER_PORT\n");
    else
    {
        int server_socket = prepare_socket(argv[1], argv[2]);
        communicate(server_socket);
    }
    return 0;
}
