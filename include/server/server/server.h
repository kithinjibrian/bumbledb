#pragma once
#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "yarns/yarns.h"
#include "objects/object.h"

typedef struct client_o client_o;

typedef void (*fun_handle_t)(client_o *);

typedef struct server_o
{
	int port;
	int backlog;
	int socket_fd;
	int buffer_size;
	fun_handle_t fun_handle;
	struct sockaddr_in address;
} server_o;

struct client_o
{
	int socket_fd;
	struct sockaddr_in address;
};

void server_close(server_o *server);
void accept_clients(server_o *server);
server_o *new_server(fun_handle_t fun_handle, int port, int buffer_size, int backlog);

#endif