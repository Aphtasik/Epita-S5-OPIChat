#include "basic_client.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define STDIN 1

/**
 * \brief Write count bytes of buf on the given fd
 *
 * \param fd : where we should write
 * \param buf : thing to write
 * \param count : Number of bytes to write
 */
static void rewrite(int fd, const char *buf, size_t count)
{
    size_t wrt = write(fd, buf, count);
    if (wrt == (size_t)-1)
    {
        errx(EXIT_FAILURE, "Rewrite function failed\n");
    }
    else if (wrt < count)
    {
        rewrite(fd, buf + wrt, count - wrt);
    }
}

/**
 * \brief Iterate over the struct addrinfo elements to connect to the server
 *
 * \param addrinfo: struct addrinfo elements
 *
 * \return The created socket or exit with 1 if there is an error
 *
 * Try to create and connect a socket with every addrinfo element until it
 * succeeds
 *
 */
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

/**
 * \brief Initialize the addrinfo struct and call create_and_connect()
 *
 * \param ip: IP address of the server
 * \param port: Port of the server
 *
 * \return The created socket
 */
int prepare_socket(const char *ip, const char *port) // TODO: ip not used
{
    struct addrinfo addr;
    struct addrinfo *res;
    memset(&addr, 0, sizeof(struct addrinfo));

    // maybe have to set some special flags (they are all set to 0)
    int ret = getaddrinfo(NULL, port, &addr, &res);
    if (ret != 0)
        return 1;
    int socket = create_and_connect(res);
    freeaddrinfo(res);
    return socket;
}

/**
 * \brief Handle communication with the server
 *
 * \param server_socket: server socket
 *
 * Read user message from STDIN and send it to the server
 *
 * Step 2: This function sends small messages to the server
 *
 * Step 3: In addition to step 2 behavior, this function receives the server
 * response on small messages
 *
 * Step 4: The function must now handle long messages
 */
void communicate(int server_socket)
{
    char buffer[DEFAULT_BUFFER_SIZE];
    ssize_t nread = 0;

    while (1)
    {
        memset(buffer, 0, DEFAULT_BUFFER_SIZE);
        puts("Enter your message:");
        nread = read(STDIN, buffer, DEFAULT_BUFFER_SIZE - 1);
        if (nread == -1)
            errx(EXIT_FAILURE, "read error");
        else if (nread != 0 && nread != 1)
        {
            write(1, "Server answered with: <OK> ",
                  28); // TODO: Clean this a bit
            rewrite(STDIN, buffer, DEFAULT_BUFFER_SIZE - 1);
        }
        else
        {
            break;
        }
    }
    close(server_socket);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: ./basic_client SERVER_IP SERVER_PORT\n");
    }
    else
    {
        int server_socket = prepare_socket(argv[1], argv[2]);
        communicate(server_socket);
    }

    return 0;
}
