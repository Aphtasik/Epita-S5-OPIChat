#include "basic_server.h"

#include <err.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

int create_and_bind(struct addrinfo *addrinfo)
{
    struct addrinfo *rp = addrinfo;
    int socketfd;
    for (; rp != NULL; rp = rp->ai_next)
    {
        socketfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        int optval = 1;
        setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
        if (socketfd == -1)
            continue;
        if (bind(socketfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break;
        close(socketfd);
    }
    if (!rp)
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
        errx(EXIT_FAILURE, "getaddrinfo error");
    int socket = create_and_bind(res);
    freeaddrinfo(res);
    return socket;
}

void rewrite(int fd, const void *buf, size_t count, int print)
{
    ssize_t nsend = 0;
    if (print)
    {
        write(1, "Received Body: ", sizeof("Received Body: "));
        nsend = write(fd, buf, count);
    }
    else
        nsend = send(fd, buf, count, MSG_NOSIGNAL);
    if (nsend == -1)
        errx(EXIT_FAILURE, "send failure, error code: %d", errno);
}

void echo(int fd_in, int fd_out)
{
    ssize_t nread;
    char *buffer = malloc(sizeof(char) * DEFAULT_BUFFER_SIZE);
    while ((nread = recv(fd_in, buffer, DEFAULT_BUFFER_SIZE - 1, 0)) > 0)
    {
        if (nread == -1)
            errx(EXIT_FAILURE, "receive function exited with code: %d", errno);

        if(buffer[nread -1] == '\n')
        {
            rewrite(1, buffer, nread, 1);
            rewrite(fd_out, buffer, nread, 0);
        }
    }
}

int accept_client(int socket)
{
    struct sockaddr peer_addr;
    socklen_t peer_len;
    listen(socket, 5);
    int connection = accept(socket, &peer_addr, &peer_len);
    if (connection == -1)
        errx(EXIT_FAILURE, "accept connection failed");
    write(1, "Client connected\n", sizeof("Client connected\n"));
    return connection;
}

void communicate(int client_socket)
{
    echo(client_socket, client_socket);
    close(client_socket);
    write(1, "Client disconnected\n", sizeof("Client disconnected"));
}

int main(int argc, char **argv)
{
    if (argc != 3)
        printf("Usage: <host> <port>");
    int socket = prepare_socket(argv[1], argv[2]);
    if (socket == 1)
        errx(EXIT_FAILURE, "socket creation error");
    while (1)
    {
        int server_socket = accept_client(socket);
        communicate(server_socket);
    }
    close(socket);
    return 0;
}
