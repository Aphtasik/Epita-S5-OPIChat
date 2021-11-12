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
    if (listen(socket, 5))
        errx(EXIT_FAILURE, "listen error %d", errno);
    return socket;
}

struct connection_t *accept_client(int epoll_instance, int server_socket,
                                   struct connection_t *connection)
{
    // Accept the client and get fd
    struct sockaddr peer_addr;
    socklen_t peer_len;
    int client_fd = accept(server_socket, &peer_addr, &peer_len);
    if (client_fd == -1)
        errx(EXIT_FAILURE, "accept error %d", errno);
    write(1, "Client connected\n", sizeof("Client connected\n"));

    // Add client to the interest list, event refer to the client event
    struct epoll_event ev = { 0 };
    ev.data.fd = client_fd;
    ev.events = EPOLLIN;
    if (epoll_ctl(epoll_instance, EPOLL_CTL_ADD, client_fd, &ev) == -1)
        errx(EXIT_FAILURE, "epoll ctl failure %d", errno);

    // Add new client to connection struct
    return add_client(connection, client_fd);
}

int main(int argc, char **argv)
{
    // Args err handling
    if (argc != 3)
        printf("Usage: ./%s <host> <port>", argv[0]);

    // create server listening socket
    int server_socket = prepare_socket(argv[1], argv[2]);

    int epoll_instance = epoll_create1(0);

    // Init the event struct containing the flags and optional user data
    struct epoll_event event = { 0 };
    event.data.fd = server_socket;
    event.events = EPOLLIN;

    // add listening socket to the interest list
    if (epoll_ctl(epoll_instance, EPOLL_CTL_ADD, server_socket, &event) == -1)
        errx(EXIT_FAILURE, "epoll ctl main failure %d", errno);

    // Create connection struct
    struct connection_t *client_list = NULL;

    while (1)
    {
        // Create event list and wait for events
        struct epoll_event events[MAX_EVENTS];
        int events_count = epoll_wait(epoll_instance, events, MAX_EVENTS, -1);

        // Iterate through event list
        for (int event_idx = 0; event_idx < events_count; event_idx++)
        {
            int sock = events[event_idx].data.fd;
            if (sock == server_socket)
            {
                client_list =
                    accept_client(epoll_instance, server_socket, client_list);
            }
            else if (events[event_idx].events & EPOLLIN)
            {
                // handle client request
                struct connection_t *client = find_client(client_list, sock);
                if (!client_list)
                    errx(EXIT_FAILURE, "find error: fd not found in list");

                char buf[DEFAULT_BUFFER_SIZE] = { 0 };
                client_list->buffer = buf;
                ssize_t nread = recv(client->client_socket, client->buffer,
                                     DEFAULT_BUFFER_SIZE - 1, 0);
                client->nb_read = nread;

                char *is_newline = strstr(buf, "\n");

                if (nread <= 0)
                {
                    // Client error / want to disconnect => disconnect
                    if (epoll_ctl(epoll_instance, EPOLL_CTL_DEL, sock,
                                  &events[event_idx]))
                        errx(EXIT_FAILURE, "epoll ctl failure %d", errno);
                    client_list = remove_client(client_list, sock);
                    close(sock);
                    write(1, "Cient Disconnected\n", 19);
                }
                else
                {
                    struct connection_t *tmp = client_list;
                    while (tmp)
                    {
                        int nsend = send(tmp->client_socket, client->buffer,
                                         client->nb_read, MSG_NOSIGNAL);
                        if (nsend == -1)
                            errx(EXIT_FAILURE, "NOSIGNAL error %d", errno);
                        else if (nsend == 0)
                            break;

                        tmp = tmp->next;
                    }
                }
            }
        }
    }

    close(server_socket);
    return 0;
}
