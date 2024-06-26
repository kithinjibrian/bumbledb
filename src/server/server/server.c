#include "server/server/server.h"

server_o *server_start(server_o *server);

void server_deinit(object_o object)
{
	server_o *server = (server_o *)object;
	close(server->socket_fd);
}

void server_print(object_o sava, int depth)
{
	server_o *server = (server_o *)sava;
	char space[depth];
	memset(space, ' ', depth);
	space[depth] = '\0';

	printf(YELLOW "<server>");
	printf(RESET ": {\n %sport: ", space);
	printf(GREEN "%d,\n", server->port);

	printf(RESET " %sbacklog: ", space);
	printf(GREEN "%d,\n", server->backlog);

	printf(RESET " %sbuffer size: ", space);
	printf(GREEN "%d\n" RESET, server->buffer_size);

	printf("%s}", space);
}

server_o *new_server(fun_handle_t fun_handle, int port, int buffer_size, int backlog)
{
	server_o *server;

	server = (server_o *)alloc(1, sizeof(server_o), SERVER_O);

	server->port = port;
	server->buffer_size = buffer_size;
	server->backlog = backlog;
	server->fun_handle = fun_handle;

	server = server_start(server);

	if (is_error(server))
	{
		/*server is an error*/
		return (object_o)server;
	}

	static const vtable_t vt = {
		.__print__ = server_print,
		.__deinit__ = server_deinit};

	object_reg_dunders(server, &vt);

	return server;
}

server_o *server_start(server_o *server)
{
	int opt;

	if ((server->socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		return error("fun server_start(void): socket failed\n", LOG);
	}

	opt = 1;
	if (setsockopt(server->socket_fd, SOL_SOCKET, SO_REUSEADDR,
				   &opt, sizeof(opt)))
	{
		return error("fun server_start(void): setsockopt failed\n", LOG);
	}

	server->address.sin_family = AF_INET;
	server->address.sin_addr.s_addr = INADDR_ANY;
	server->address.sin_port = htons(server->port);

	if (bind(server->socket_fd, (struct sockaddr *)&(server->address),
			 sizeof(server->address)) < 0)
	{
		return error("fun server_start(void): bind failed\n", LOG);
	}

	if (listen(server->socket_fd, server->backlog) < 0)
	{
		return error("fun server_start(void): listen failed\n", LOG);
	}

	return server;
}