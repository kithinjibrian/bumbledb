#include "server/bumblebee/bumble.h"

static uint8_t *__pack_reply__(long *status, uint8_t *body, int len)
{
	ReplyProto reply_proto = REPLY_PROTO__INIT;
	reply_proto.status = status != NULL ? (*(long *)status) : 200;
	reply_proto.status_text = "OK";
	reply_proto.body.data = body;
	reply_proto.body.len = len;

	ReplyHeader header = REPLY_HEADER__INIT;
	header.server = "bumbleDB";
	header.content_type = "text/plain";

	reply_proto.header = &header;

	size_t size = reply_proto__get_packed_size(&reply_proto);
	uint8_t *buffer = new_bytes(size);
	reply_proto__pack(&reply_proto, buffer);
	return buffer;
}

bool bee_bytes(request_o *req, response_o *res, uint8_t *bytes)
{
	SMART uint8_t *smt_bytes = bytes;

	long *status = htable_get(res, string_from("status"));

	uint8_t *buffer = __pack_reply__(status, smt_bytes, object_count(smt_bytes));
	return send_encrypted(req, res, buffer);
}

bool bee_send(request_o *req, response_o *res, string_o *message)
{
	SMART string_o *smt_message = message;

	long *status = htable_get(res, string_from("status"));

	uint8_t *buffer = __pack_reply__(status, (uint8_t *)cstring(smt_message), string_length(smt_message));

	return send_encrypted(req, res, buffer);
}

bool bee_status(response_o *res, int status)
{
	htable_set(res, string_from("status"), number(status));
	return 1;
}