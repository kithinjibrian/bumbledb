#include "objects/natives/dict/dict.h"

static bool __find_key__(object_o object, object_o args, array_o *array);

static bool __find_key__(object_o object, object_o args, array_o *array)
{
	(void)array;
	return rstring_compare(share(keyvalue_get_key(object)), share(args));
}

dict_o *new_dict(int size)
{
	dict_o *dict = new_array(size);

	return dict;
}

void dict_set(dict_o *self, object_o key, object_o value)
{
	keyvalue_o *key_value = new_keyvalue(key, value);
	array_push(self, key_value);
}

object_o dict_get(dict_o *self, object_o key)
{
	SMART object_o smt_key = key;
	return array_filter(self, __find_key__, smt_key);
}