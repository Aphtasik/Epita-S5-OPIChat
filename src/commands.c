#include "command.h"

// send struct response in client_socket, return -1 on error, 0 otherwise
int send_response(struct request response, int client_socket)
{
}

char *exec_ping(struct connection_t *client, struct connection_t client_list)
{
    
}

char *exec_login(struct connection_t *client, struct connection_t client_list)
{
    
}

char *exec_list_users(struct connection_t *client, struct connection_t client_list)
{
    
}

char *exec_send_dm(struct connection_t *client, struct connection_t client_list)
{
    
}

char *exec_broadcast(struct connection_t *client, struct connection_t client_list)
{
    
}

struct fun fun_tab[5] = 
{
    { .fun = (*exec_ping) },
    { .fun = (*exec_login) },
    { .fun = (*exec_list_users) },
    { .fun = (*exec_send_dm) },
    { .fun = (*exec_broadcast) }
};

char *exec_cmd(struct connection_t *client, struct connection_t *client_list)
{
    struct request req = client->request;
    fun_tab[req->cmd].fun(client, client_list);
}
