#include "user/auth.h"

bool user_register(bumble_o *self, request_o *req, response_o *res)
{
	htable_o *user = htable_get(req, string_from("user"));

	SMART file_o *file = f_open("users.quill", "a");

	char hash[crypto_pwhash_STRBYTES];
	SMART string_o *key = string_from("password");
	string_o *password = htable_get(user, share(key));

	if (crypto_pwhash_str(
			hash,
			cstring(password),
			string_length(password),
			crypto_pwhash_OPSLIMIT_INTERACTIVE,
			crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0)
	{
		return bee_send(MOVE(req), res, string_from("User creation failed!"));
	}

	htable_set(user, share(key), string_from("%s", hash));

	int bul = quill_write(file, share(user));

	if (bul == -1)
	{
		return bee_send(MOVE(req), res, string_from("Data is wrongly formatted!"));
	}

	return bee_send(MOVE(req), res, string_from("User created!"));
}

bool user_login(bumble_o *self, request_o *req, response_o *res)
{
	htable_o *user = htable_get(req, string_from("user"));
	htable_o *saved_user = htable_get(req, string_from("saved-user"));

	SMART string_o *key = string_from("password");
	string_o *password = htable_get(user, share(key));
	string_o *hash_password = htable_get(saved_user, share(key));

	if (crypto_pwhash_str_verify(
			cstring(hash_password),
			cstring(password),
			string_length(password)) != 0)
	{
		return bee_send(req, res, string_from("Login failed!"));
	}

	htable_o *tkn = new_htable(2);
	htable_set(tkn, string_from("expires-in"), number(4));
	htable_set(tkn, string_from("username"), share(htable_get(user, string_from("username"))));

	SMART uint8_t *token = ptoken(tkn);

	if (token)
	{
		return bee_bytes(req, res, MOVE(token));
	}

	return bee_send(req, res, string_from("Login failed!"));
}

bool protected(bumble_o *self, request_o *req, response_o *res)
{
	return bee_send(req, res, string_from("BumbleDB: Hello, Mama! I am protected!"));
}

void user_routes(void)
{
	bumble_action("user-register", 2, user_reg_check, user_register);
	bumble_action("user-login", 2, user_log_check, user_login);
	bumble_action("hello-world-protected", 2, is_authn, protected);
}