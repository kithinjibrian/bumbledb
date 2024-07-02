#include "db/db.h"

bool db_save(bumble_o *self, request_o *req, response_o *res)
{
	uint8_t *sraso = (uint8_t *)htable_get(req, string_from("body"));

	array_o *object = (array_o *)load(string_from("%s", sraso));

	if (object == NULL)
	{
		bee_status(res, 500);
		return bee_send(req, res, string_from("Parse error"));
	}

	if (object_typeof(object) == NT_ARRAY_O && object->length == 2)
	{
		cache_save(array_at(object, 0), array_at(object, 1));

		SMART file_o *file = f_open("cache.quill", "a");
		quill_write(file, share(object));

		return bee_send(req, res, string_from("Object saved"));
	}
	return bee_send(req, res, string_from("Object should be a key-value pair"));
}

bool db_get(bumble_o *self, request_o *req, response_o *res)
{
	uint8_t *sraso = (uint8_t *)htable_get(req, string_from("body"));
	string_o *key = string_from("%s", sraso);

	object_o object = cache_get(key);
	if (object)
	{
		string_o *sraso_str = NULL;

		if (object_typeof(object) == NT_STRING_O)
		{
			sraso_str = string_format("\"%q\"", object);
		}
		else
		{
			sraso_str = str(object);
		}

		return bee_send(req, res, sraso_str);
	}

	return bee_send(req, res, string_from("Object not found"));
}

void db_routes()
{
	bumble_action("cache-get", 2, is_authn, db_get);
	bumble_action("cache-save", 2, is_authn, db_save);
}