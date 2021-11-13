#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>
#include <string.h>

enum cmd
{
    PING = 0,
    LOGIN,
    LIST_USERS,
    SEND_DM,
    BROADCAST
};

enum status
{
    REQUEST = 0,
    RESPONSE = 1,
    NOTIF = 2,
    ERROR = 3
};

struct request
{
    enum cmd cmd;
    enum status status;
    size_t nb_param;
    char **param;
    int payload_size;
    char *msg;
};

struct fun
{
    void (*fun)(struct request *, char *);
};

struct request parse_request(char *str_request);
void free_request(struct request *req);

#endif
