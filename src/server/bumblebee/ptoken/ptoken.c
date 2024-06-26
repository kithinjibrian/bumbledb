#include "server/bumblebee/bumble.h"

uint8_t *ptoken(htable_o *token)
{

	htable_set(token, string_from("iat"), number(time(NULL)));

	SMART object_o smt_token = token;
	SMART string_o *buffer = str(smt_token);

	uint8_t auth[crypto_sign_BYTES];

	if (crypto_sign_detached(auth, NULL, (uint8_t *)cstring(buffer), string_length(buffer), get_sign_secret_key()) != 0)
	{
		return NULL;
	}

	AuthProto auth_proto = AUTH_PROTO__INIT;
	auth_proto.authorization.data = auth;
	auth_proto.authorization.len = crypto_sign_BYTES;
	auth_proto.token = cstring(buffer);
	auth_proto.token_len = string_length(buffer);

	size_t size = auth_proto__get_packed_size(&auth_proto);
	uint8_t *bytes = new_bytes(size);
	auth_proto__pack(&auth_proto, bytes);

	return bytes;
}