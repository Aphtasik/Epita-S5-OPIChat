#include "commands.h"

char *enum_to_string(enum cmd cmd)
{
    switch (cmd)
    {
    case PING:
        return "PING";
    case LOGIN:
        return "LOGIN";
    case LIST_USERS:
        return "LIST_USERS";
    case SEND_DM:
        return "SEND_DM";
    case BROADCAST:
        return "BROADCAST";
    default:
        return NULL;
    }
}

size_t total_size(struct request req, char *cmd_str)
{
    size_t total = 7;
    for (size_t i = 0; i < req.nb_param; i++)
    {
        total += strlen(req.param[i]);
    }
    total += strlen(req.msg);
    total += strlen(cmd_str);

    return total;
}

// send struct response in client_socket, return -1 on error, 0 otherwise
char *format_response(struct request response)
{
    char *cmd_str = enum_to_string(response.cmd);
    if (!cmd_str)
        return NULL;

    size_t nb_bytes = total_size(response, cmd_str) + 16;
    char *buf = calloc(nb_bytes, sizeof(char));

    if (response.nb_param == 0)
        sprintf(buf, "%d\n%d\n%s\n\n%s", response.payload_size, response.status,
                cmd_str, response.msg);
    if (response.nb_param == 1)
        sprintf(buf, "%d\n%d\n%s\n%s\n%s", response.payload_size,
                response.status, cmd_str, response.param[0], response.msg);
    else
        sprintf(buf, "%d\n%d\n%s\n%s\n%s\n%s", response.payload_size,
                response.status, cmd_str, response.param[0], response.param[1],
                response.msg);

    return buf;
}

char *exec_ping(struct connection_t *client, struct connection_t *client_list)
{
    struct request response = 
    {
        .cmd = PING,
        .nb_param = 0,
        .param = NULL,
        .status = 1,
        .payload_size = 5,
        .msg = "PONG"
    };

    char* formated_str = format_response(response);
    return formated_str;
}

char *exec_login(struct connection_t *client, struct connection_t *client_list)
{
    struct request response;
    if (client->name)
    {
        response.cmd = LOGIN;
        response.nb_param = 0;
        response.param = NULL;
        response.status = 3;
        response.payload_size = 19;
        response.msg = "Duplicate username\n";
    }
    else
    {
        client->name = malloc(sizeof(char) * client->request.payload_size + 1);
        strcpy(client->name, client->request.msg);

        response.cmd = LOGIN;
        response.nb_param = 0;
        response.param = NULL;
        response.status = 1;
        response.payload_size = 10;
        response.msg = "Logged in\n";
    }

    char* formated_str = format_response(response);
    if (!formated_str)
        return NULL; //TODO: handle errors

    return formated_str;
}

char *exec_list_users(struct connection_t *client,
                      struct connection_t *client_list)
{
    assert(0 && "Not implemented");
}

char *exec_send_dm(struct connection_t *client,
                   struct connection_t *client_list)
{
    assert(0 && "Not implemented");
}


char *exec_broadcast(struct connection_t *client,
                     struct connection_t *client_list)
{
    assert(0 && "Not implemented");
}

struct exec_fun fun_tab[5] = { { .fun = (*exec_ping) },
                               { .fun = (*exec_login) },
                               { .fun = (*exec_list_users) },
                               { .fun = (*exec_send_dm) },
                               { .fun = (*exec_broadcast) } };

char *exec_cmd(struct connection_t *client, struct connection_t *client_list)
{
    struct request req = client->request;
    return fun_tab[req.cmd].fun(client, client_list);
}
