#ifndef COMMANDS_H
#define COMMAND_H

#include "parser.h"
#include "connection.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>

struct fun
{
    // left parameter is client right parameter is client list
    char *(*fun)(struct connection_t*, struct connection_t*);
}

#endif
