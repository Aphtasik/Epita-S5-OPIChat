#ifndef COMMANDS_H
#define COMMAND_H

#include "parser.h"
#include "connection.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <assert.h>

struct exec_fun
{
    // left parameter is client right parameter is client list
    char *(*fun)(struct connection_t*, struct connection_t*);
};

char *exec_cmd(struct connection_t *client, struct connection_t *client_list);
#endif
