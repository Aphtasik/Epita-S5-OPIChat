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
    // maybe have to set some special flags (they are all set to 0)
    int ret = getaddrinfo(ip, port, &addr, &res);
    if (ret != 0)
        return 1;
    int socket = create_and_connect(res);
    freeaddrinfo(res);
    return socket;
}

static size_t my_strlen(char *buff)
{
    size_t i = 0;
    for (; buff[i] != '\n' && buff[i] != '\0' && i < DEFAULT_BUFFER_SIZE; i++)
        ;
    return i;
}

void communicate(int server_socket)
{
    if (server_socket == 1)
        errx(EXIT_FAILURE, "socket error");

    char message[DEFAULT_BUFFER_SIZE] = { 0 };
    char response[DEFAULT_BUFFER_SIZE] = { 0 };
    ssize_t bytes;
    while (1)
    {
        printf("Enter your message:\n");
        read(0, message, DEFAULT_BUFFER_SIZE);
        bytes = send(server_socket, message, my_strlen(message), 0);
        if (bytes < 0)
        {
            errx(EXIT_FAILURE, "send error");
        }
        else if (bytes == 1 || bytes == 0)
        {
            break;
        }

        if (recv(server_socket, response, DEFAULT_BUFFER_SIZE, 0) < 0)
        {
            errx(EXIT_FAILURE, "recv error");
        }

        printf("Server reply : %s\n", response);
        memset(message, 0, DEFAULT_BUFFER_SIZE);
        memset(response, 0, DEFAULT_BUFFER_SIZE);
    }
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
