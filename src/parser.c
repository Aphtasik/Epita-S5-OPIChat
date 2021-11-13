#include "parser.h"

static void get_payload_size(struct request *req, char *payload)
{
    req->payload_size = atoi(payload);
    req->msg = malloc(sizeof(char) * req->payload_size + 1);
}
static void get_status(struct request *req, char *status)
{
    int stat = atoi(status);
    req->status = stat;
}

static void get_cmd(struct request *req, char *cmd)
{
    if (!strcmp("PING", cmd))
    {
        req->cmd = PING;
        req->nb_param = 0;
    }
    else if (!strcmp("LOGIN", cmd))
    {
        req->cmd = LOGIN;
        req->nb_param = 0;
    }
    else if (!strcmp("LIST-USERS", cmd))
    {
        req->cmd = LIST_USERS;
        req->nb_param = 0;
    }
    else if (!strcmp("SEND-DM", cmd))
    {
        req->cmd = SEND_DM;
        req->nb_param = 2;
    }
    else if (!strcmp("BROADCAST", cmd))
    {
        req->cmd = BROADCAST;
        req->nb_param = 1;
    }
}

static void get_msg(struct request *req, char *msg)
{
    strcpy(req->msg, msg);
}

static void get_param(struct request *req, char *param)
{
    if (req->nb_param == 0)
    {
        get_msg(req, param);
        return;
    }

    if (!req->param)
    {
        req->param = malloc(sizeof(char *) * req->nb_param);
        for (size_t i = 0; i < req->nb_param; i++)
            req->param[i] = NULL;
    }

    size_t param_len = strlen(param);
    char *arg = malloc(sizeof(char) * param_len + 1);

    int j = 0;
    while (req->param[j])
        j++;
    strcpy(arg, param);
    req->param[j] = arg;
}

void free_request(struct request *req)
{
    free(req->msg);
    for (size_t i = 0; i < req->nb_param; i++)
        free(req->param[i]);
    free(req->param);
}

struct fun fun_tab[5] = { { .fun = (*get_payload_size) },
                          { .fun = (*get_status) },
                          { .fun = (*get_cmd) },
                          { .fun = (*get_param) },
                          { .fun = (*get_msg) } };

struct request parse_request(char *str_request)
{
    struct request request;
    request.param = NULL;
    int fun_idx = 0;
    char *token = strtok(str_request, "\\n");
    while (token)
    {
        fun_tab[fun_idx].fun(&request, token);
        fun_idx++;
        token = strtok(NULL, "\\n");
    }
    return request;
}
