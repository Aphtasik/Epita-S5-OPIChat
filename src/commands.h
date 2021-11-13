#ifndef COMMANDS_H
#define COMMANDS_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "connection.h"
#include "parser.h"

struct exec_fun
{
    // left parameter is client right parameter is client list
    char *(*fun)(struct connection_t *, struct connection_t *);
};

char *exec_cmd(struct connection_t *client, struct connection_t *client_list);
#endif
