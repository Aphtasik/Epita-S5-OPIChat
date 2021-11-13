#ifndef PARSER_H
#define PARSER_H

enum cmd
{
    PING,
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

struct param
{
    char *var;
    char *content;
    struct param *next;
};

struct request
{
    enum cmd cmd;
    enum status status;
    struct param param;
    char *msg;
};

#endif
