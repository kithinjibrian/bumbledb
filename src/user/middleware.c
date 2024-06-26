#include "user/auth.h"

array_o *user_find(string_o *username)
{
	SMART string_o *smt_username = username;

	SMART file_o *file = f_open("users.quill", "rb");
	if (is_error(file))
	{
		RETURN(file);
	}

	SMART array_o *users_many = quill_read(file);

	while (!array_at_end(users_many))
	{
		htable_o *user = (htable_o *)array_current(users_many);

		object_o value = htable_get(user, string_from("username"));

		if (equal(value, smt_username))
		{
			return share(user);
		}

		array_next(users_many);
	}

	return error("User not found!", SILENT);
}

bool user_reg_check(bumble_o *self, request_o *req, response_o *res)
{
	uint8_t *body = (uint8_t *)htable_get(req, string_from("body"));

	if (body)
	{
		UserTransitProto *user_proto = user_transit_proto__unpack(NULL, object_count(body), body);
		SMART string_o *username = string_from(user_proto->username);
		SMART string_o *password = string_from(user_proto->password);
		user_transit_proto__free_unpacked(user_proto, NULL);
		user_proto = NULL;

		SMART object_o saved_user = user_find(share(username));
		if (is_error(saved_user))
		{
			htable_o *user = new_htable(2);
			htable_set(user, string_from("username"), share(username));
			htable_set(user, string_from("password"), share(password));

			htable_set(req, string_from("user"), user);

			return bumble_next(self, req, res);
		}

		return bee_send(MOVE(req), MOVE(res), string_from("User already exists!"));
	}

	return bee_send(req, res, string_from("Body not provided!"));
}

bool user_log_check(bumble_o *self, request_o *req, response_o *res)
{
	uint8_t *body = (uint8_t *)htable_get(req, string_from("body"));

	if (body)
	{
		UserTransitProto *user_proto = user_transit_proto__unpack(NULL, object_count(body), body);
		SMART string_o *username = string_from(user_proto->username);
		SMART string_o *password = string_from(user_proto->password);
		user_transit_proto__free_unpacked(user_proto, NULL);
		user_proto = NULL;

		SMART object_o saved_user = user_find(share(username));
		if (is_error(saved_user))
		{
			return bee_send(MOVE(req), MOVE(res), string_from("Login failed!"));
		}

		htable_set(req, string_from("saved-user"), MOVE(saved_user));

		htable_o *user = new_htable(2);
		htable_set(user, string_from("username"), share(username));
		htable_set(user, string_from("password"), share(password));

		htable_set(req, string_from("user"), user);

		return bumble_next(self, req, res);
	}

	return bee_send(req, res, string_from("Body not provided!"));
}

bool is_authn(bumble_o *self, request_o *req, response_o *res)
{
	htable_o *header = (htable_o *)htable_get(req, string_from("header"));
	uint8_t *authorization = (uint8_t *)htable_get(header, string_from("authorization"));

	AuthProto *auth = auth_proto__unpack(NULL, object_count(authorization), authorization);

	if (auth)
	{
		if (crypto_sign_verify_detached(auth->authorization.data, (uint8_t *)auth->token, auth->token_len, get_sign_public_key()) != 0)
		{
			auth_proto__free_unpacked(auth, NULL);
			return bee_send(req, res, string_from("Authorization failed!"));
		}

		object_o object = load(string_from("%.*s", auth->token_len, auth->token));
		auth_proto__free_unpacked(auth, NULL);

		long *iat = htable_get(object, string_from("iat"));
		long *exp = htable_get(object, string_from("expires-in"));
		long now = time(NULL);

		if (now > (*(long *)iat) + (*(long *)exp))
		{
			bee_status(res, 401);
			return bee_send(req, res, string_from("Authorization failed! Token expired!"));
		}

		htable_set(req, string_from("token"), object);

		return bumble_next(self, req, res);
	}
	return bee_send(req, res, string_from("Authorization failed! Token not provided!"));
}