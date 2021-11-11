#include "epoll-server.h"

#include <assert.h>
#include <err.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

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
        errx(EXIT_FAILURE, "create and bind error %d", errno);
    return socketfd;
}

int prepare_socket(const char *ip, const char *port)
{
    // Setup the addrinfo struct
    struct addrinfo addr;
    struct addrinfo *res;
    memset(&addr, 0, sizeof(struct addrinfo));

    // Fill the addrinfo struct
    int ret = getaddrinfo(ip, port, &addr, &res);
    if (ret != 0)
        errx(EXIT_FAILURE, "getaddrinfo error %d", errno);

    // Get a socket and listen for client connections
    int socket = create_and_bind(res);
    freeaddrinfo(res);
    listen(socket, 5);
    return socket;
}

struct connection_t *accept_client(int epoll_instance, int server_socket,
                                   struct connection_t *connection)
{
    // Init the event struct containing the flags and optional user data
    struct epoll_event event = { 0 };
    event.data.fd = server_socket;
    event.events = EPOLLIN;

    struct epoll_event events[MAX_EVENTS];
    int events_count = epoll_wait(epoll_instance, events, MAX_EVENTS, -1);

    // Iterate through event list
    for (int event_idx = 0; event_idx < events_count; event_idx++)
    {
        int sock = events[event_idx].data.fd;

        if (sock == server_socket)
        {
            // Accept new client
            struct sockaddr peer_addr;
            socklen_t peer_len;
            int client_fd = accept(sock, &peer_addr, &peer_len);
            if (client_fd == -1)
                errx(EXIT_FAILURE, "accept error %d", errno);
            write(1, "Client connected\n", sizeof("Client connected\n"));

            // Add client to the interest list
            if (epoll_ctl(epoll_instance, EPOLL_CTL_ADD, client_fd, &event)
                == -1)
                errx(EXIT_FAILURE, "epoll ctl failure %d", errno);

            // Add new client to connection struct
            connection = add_client(connection, sock);
        }
    }
    return connection;
}

int main(int argc, char **argv)
{
    // Args err handling
    if (argc != 3)
        printf("Usage: <host> <port>");

    // create server listening socket
    int server_socket = prepare_socket(argv[1], argv[2]);
    if (server_socket == 1)
        errx(EXIT_FAILURE, "socket creation error");

    int epoll_instance = epoll_create(0);

    // Init the event struct containing the flags and optional user data
    struct epoll_event event = { 0 };
    event.data.fd = server_socket;
    event.events = EPOLLIN;

    // add listening socket to the interest list
    if (epoll_ctl(epoll_instance, EPOLL_CTL_ADD, server_socket, &event) == -1)
        errx(EXIT_FAILURE, "epoll ctl failure %d", errno);

    // Create connection struct
    struct connection_t *connections = NULL;

    // Accept clients
    connections = accept_client(epoll_instance, server_socket, connections);

    return 0;
}
