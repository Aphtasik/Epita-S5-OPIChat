#include "parser.h"


char *get_str_status(enum status status)
{
    switch(status)
    {
    case REQUEST:
        return "0";
    case RESPONSE:
        return "1";
    case NOTIF:
        return "2";
    default: // ERROR
        return "3";
    }
}

char *get_str_cmd(enum cmd cmd)
{
    
}

void get_payload_size(struct request req, char *payload)
{
    req->payload_size = atoi(payload);
    req->msg = malloc(sizeof(char) * req->payload_size + 1);
}

void get_status(struct request req, char *status)
{
    // TODO: check if it works
    int stat = atoi(status);
    req->status = stat;
}

void get_cmd(struct request req, char *cmd)
{
    if(!strcmp("PING", cmd))
        req->cmd = PING;
    else if(!strcmp("LOGIN", cmd))
        req->cmd = LOGIN;
    else if(!strcmp("LIST_USERS", cmd))
        req->cmd = LIST_USERS;
    else if(!strcmp("SEND_DM", cmd))
        req->cmd = SEND_DM;
    else if(!strcmp("BROADCAST", cmd))
        req->cmd = BROADCAST;
}

void get_param(struct request *req, char *request)
{
    
}

void get_msg(struct request *req, char *msg)
{
    strcpy(req->msg, msg);
}

struct fun fun_tab[5] = 
{
    { .fun = (*get_payload_size) },
    { .fun = (*get_status) },
    { .fun = (*get_command) },
    { .fun = (*get_param) },
    { .fun = (*get_msg) }
};

struct request parse_request(char *request)
{
    struct request request; 
    request.param = NULL;
    char *token = NULL;
    char *saveptr;
    int fun_idx = 0;
    while((token = strtok_r(request, "\n", saveptr)))
    {
        fun_tab[fun_idx](request, token);
        fun_idx++;
    }
    return request;
}
