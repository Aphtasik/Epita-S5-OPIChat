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

void communicate(int server_socket)
{
    if (server_socket == 1)
        errx(EXIT_FAILURE, "socket error");
    char buffer[DEFAULT_BUFFER_SIZE] = { 0 };
    ssize_t nread = 0;
    while (1)
    {
        write(1, "Enter your message:\n", sizeof("Enter your message:\n"));
        nread = read(0, buffer, DEFAULT_BUFFER_SIZE);
        if (nread == -1)
            errx(EXIT_FAILURE, "read error");
        if (nread == 1)
            break;
        if (send(server_socket, buffer, nread, 0) != nread)
            errx(EXIT_FAILURE, "send error");
        ssize_t nrecieved;
        if ((nrecieved = recv(server_socket, buffer, DEFAULT_BUFFER_SIZE, 0))
            != 0)
        {
            if (nrecieved == -1)
                errx(EXIT_FAILURE, "reception error");
            write(1, "Server answered with: ", 23);
            if (write(1, buffer, nrecieved) == -1)
                errx(EXIT_FAILURE, "write error");
        }
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
