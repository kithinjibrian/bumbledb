#include "server/bumblebee/bumble.h"

static uint8_t *__pack_response__(bool is_enc, uint8_t *nonce, uint8_t *body, int body_len, uint8_t *signature)
{
	ResponseProto res_proto = RESPONSE_PROTO__INIT;
	res_proto.is_enc = is_enc;
	res_proto.body.data = body;
	res_proto.body.len = body_len;
	res_proto.nonce.data = nonce;
	res_proto.nonce.len = nonce != NULL ? crypto_box_NONCEBYTES : 0;
	res_proto.signature.data = signature;
	res_proto.signature.len = signature != NULL ? crypto_sign_BYTES : 0;

	size_t size = response_proto__get_packed_size(&res_proto);

	uint8_t *buffer = new_bytes(size);
	response_proto__pack(&res_proto, buffer);

	return buffer;
}

bool send_unencrypted(request_o *req, response_o *res, uint8_t *bytes)
{

	SMART request_o *smt_req = req;
	SMART request_o *smt_res = res;
	SMART uint8_t *smt_bytes = bytes;

	client_o *client = (client_o *)htable_get(smt_req, string_from("client"));

	SMART uint8_t *buffer = __pack_response__(0, NULL, smt_bytes, object_count(smt_bytes), NULL);

	send(client->socket_fd, buffer, object_count(buffer), 0);

	return 1;
}

bool send_signed(request_o *req, response_o *res, uint8_t *bytes)
{
	SMART request_o *smt_req = req;
	SMART request_o *smt_res = res;
	SMART uint8_t *smt_bytes = bytes;

	client_o *client = (client_o *)htable_get(smt_req, string_from("client"));

	SMART uint8_t *signature = new_bytes(crypto_sign_BYTES);

	if (crypto_sign_detached(signature, NULL, smt_bytes, object_count(smt_bytes), get_sign_secret_key()) != 0)
	{
		return false;
	}

	SMART uint8_t *buffer = __pack_response__(0, NULL, smt_bytes, object_count(smt_bytes), signature);

	send(client->socket_fd, buffer, object_count(buffer), 0);

	return true;
}

bool send_encrypted(request_o *req, response_o *res, uint8_t *bytes)
{
	SMART request_o *smt_req = req;
	SMART response_o *smt_res = res;
	SMART uint8_t *smt_bytes = bytes;

	client_o *client = (client_o *)htable_get(smt_req, string_from("client"));
	client_crypto_keys_o *cck = (client_crypto_keys_o *)htable_get(smt_req, string_from("client_keys"));

	int length = object_count(bytes) + crypto_box_MACBYTES;

	uint8_t cipher[length];
	memset(cipher, 0, length);

	uint8_t nonce[crypto_box_NONCEBYTES];
	randombytes_buf(nonce, crypto_box_NONCEBYTES);

	if (crypto_box_easy_afternm(
			cipher,
			smt_bytes,
			object_count(smt_bytes),
			nonce,
			cck->shared_key) != 0)
	{
		return false;
	}

	SMART uint8_t *buffer = __pack_response__(1, nonce, cipher, length, NULL);

	send(client->socket_fd, buffer, object_count(buffer), 0);

	return true;
}
