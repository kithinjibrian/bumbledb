#include "server/bumblebee/bumble.h"

static bumble_o *bumble_g = NULL;

bumble_o *bumble_get()
{
	if (bumble_g == NULL)
	{
		bumble_g = new_array(0);
		pool_globals_add(bumble_g);
	}
	return bumble_g;
}

void bumble_print()
{
	bumble_o *bumble = bumble_get();
	print(bumble);
}

void bumble_use(fun_generic_t middleware)
{
	bumble_o *bumble = bumble_get();
	array_push(bumble, function("global-mw", middleware));
}

void bumble_action(const char *action, int n, ...)
{
	va_list args;
	va_start(args, n);

	bumble_o *bumble = bumble_get();

	for (int a = 0; a < n; a++)
	{
		fun_generic_t fun = va_arg(args, fun_generic_t);
		array_push(bumble, function(action, fun));
	}
	va_end(args);
}

bool bumble_next(bumble_o *self, request_o *request, response_o *response)
{
	if (!array_at_end(self))
	{
		string_o *string = (string_o *)htable_get(request, string_from("action"));

		function_o *function = array_next(self);
		fun_middleware_t mw = (fun_middleware_t)function_fun(function);

		if (strcmp(function_name(function), "global-mw") == 0)
		{
			mw(self, request, response);
		}
		else if (strcmp(function_name(function), cstring(string)) == 0)
		{
			mw(self, request, response);
		}
		else
		{
			bumble_next(self, request, response);
		}
	}

	return true;
}

void bumble_run(request_o *request, response_o *response)
{
	string_o *action = (string_o *)htable_get(request, string_from("action"));
	if (strcmp("global-mw", cstring(action)) != 0)
	{
		bumble_o *bumble = bumble_get();
		bumble_next(bumble, request, response);
	}
}

void bumble(client_o *client)
{
	RequestProto *req_proto;
	SMART client_crypto_keys_o *client_keys = alloc(1, sizeof(client_crypto_keys_o), CLIENT_CRYPTO_KEYS_O);
	client_keys->set = false;

	// TODO: change this to be dynamic
	uint8_t buffer[4096];
	memset(buffer, 0, 4096);

	while (1)
	{
		ssize_t bytes_received = recv(client->socket_fd, buffer, sizeof(buffer), 0);
		if (bytes_received <= 0)
		{
			break;
		}

		req_proto = request_proto__unpack(NULL, bytes_received, buffer);

		if (!req_proto)
		{
			break;
		}

		response_o *response = new_htable(10);
		request_o *request = new_htable(10);
		htable_set(request, string_from("client"), share(client));
		htable_set(request, string_from("client_keys"), share(client_keys));
		htable_set(request, string_from("action"), string_from(req_proto->action));

		if (req_proto->is_enc)
		{
			if (client_keys->set)
			{

				int len = req_proto->body.len - crypto_box_MACBYTES;
				uint8_t *decrypted = new_bytes(len);

				if (crypto_box_open_easy_afternm(decrypted, req_proto->body.data, req_proto->body.len, req_proto->nonce.data, client_keys->shared_key) != 0)
				{
					break;
				}

				htable_set(request, string_from("body"), decrypted);
				htable_set(request, string_from("is_enc"), number(1));
			}
			else
			{
				break;
			}
		}
		else
		{
			htable_set(request, string_from("body"), bytes_from(req_proto->body.data, req_proto->body.len));
		}

		request_proto__free_unpacked(req_proto, NULL);

		bumble_run(request, response);

		array_rewind(bumble_get());

		memset(buffer, 0, 1024);
	}

	close(client->socket_fd);
}
