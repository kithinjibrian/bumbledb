#include "server/bumblebee/bumble.h"

void logger(bumble_o *self, request_o *req, response_o *res)
{
	string_o *string = (string_o *)htable_get(req, string_from("action"));

	print(string);

	bumble_next(self, req, res);
}

bool bee(bumble_o *self, request_o *req, response_o *res)
{
	int *is_enc = htable_get(req, string_from("is_enc"));

	if (is_enc && (*(int *)is_enc) == 1)
	{
		uint8_t *body = (uint8_t *)htable_get(req, string_from("body"));
		QueryProto *query = query_proto__unpack(NULL, object_count(body), body);

		if (query)
		{
			htable_delete(req, string_from("body"));

			htable_o *header = new_htable(5);
			htable_set(header, string_from("host"), string_from(query->header->host));
			htable_set(header, string_from("user_agent"), string_from(query->header->user_agent));
			htable_set(header, string_from("content_type"), string_from(query->header->content_type));
			htable_set(header, string_from("authorization"), bytes_from(query->header->authorization.data, query->header->authorization.len));

			htable_set(req, string_from("header"), MOVE(header));
			htable_set(req, string_from("body"), bytes_from(query->body.data, query->body.len));

			query_proto__free_unpacked(query, NULL);
		}
	}

	return bumble_next(self, req, res);
}

bool sign_public_key(bumble_o *self, request_o *req, response_o *res)
{
	return send_unencrypted(req, res, bytes_from(get_sign_public_key(), crypto_sign_PUBLICKEYBYTES));
}

bool diffie_public_key(bumble_o *self, request_o *req, response_o *res)
{
	diffie_keys_o *diffie_keys = (diffie_keys_o *)htable_get(req, string_from("client_keys"));

	crypto_box_keypair(diffie_keys->server_public_key, diffie_keys->server_secret_key);

	diffie_keys->server_diffie_set = true;

	return send_signed(req, res, bytes_from(diffie_keys->server_public_key, crypto_box_PUBLICKEYBYTES));
}

bool accept_public_key(bumble_o *self, request_o *req, response_o *res)
{
	SMART request_o *smt_req = req;
	uint8_t *client_pk = (uint8_t *)htable_get(smt_req, string_from("body"));
	diffie_keys_o *diffie_keys = (diffie_keys_o *)htable_get(smt_req, string_from("client_keys"));

	if (diffie_keys->server_diffie_set)
	{
		if (crypto_box_beforenm(diffie_keys->client_shared_key, client_pk, diffie_keys->server_secret_key) != 0)
		{
			return send_unencrypted(share(smt_req), res, bytes_from("Error generating shared key!", 29));
		}

		diffie_keys->client_diffie_set = true;

		return send_unencrypted(share(smt_req), res, bytes_from("Public key recieved", 20));
	}

	return send_unencrypted(share(smt_req), res, bytes_from("Server error!", 14));
}

bool hello_world(bumble_o *self, request_o *req, response_o *res)
{
	return send_unencrypted(req, res, bytes_from("BumbleDB: Hello, Mama!", 23));
}

bool hello_world_enc(bumble_o *self, request_o *req, response_o *res)
{
	return bee_send(req, res, string_from("BumbleDB: Hello, Mama! I am encrypted"));
}

void bumble_routes()
{
	bumble_use((fun_generic_t)bee);
	bumble_use((fun_generic_t)logger);
	bumble_action("hello-world", 1, hello_world);
	bumble_action("hello-world-enc", 1, hello_world_enc);
	bumble_action("get-spk", 1, sign_public_key);
	bumble_action("get-dpk", 1, diffie_public_key);
	bumble_action("post-cpk", 1, accept_public_key);
}