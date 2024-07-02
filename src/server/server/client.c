#include "server/server/server.h"

void accept_clients(server_o *server)
{
	int addrlen;
	SMART client_o *client;
	struct sockaddr *address;

	client = alloc(1, sizeof(client_o), CLIENT_O);

	addrlen = sizeof(client->address);
	address = (struct sockaddr *)&client->address;

	while (1)
	{
		if ((client->socket_fd = accept(
				 server->socket_fd,
				 address,
				 (socklen_t *)&addrlen)) < 0)
		{
			perror("accept");
			break;
		}

		server->fun_handle(client);

		// break;
	}
}