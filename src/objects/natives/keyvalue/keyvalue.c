#include "objects/object.h"

void keyvalue_visitor(object_o object, fun_process_t fun_process)
{
	keyvalue_o *keyvalue = (keyvalue_o *)object;
	post_visit(keyvalue->key, fun_process);
	post_visit(keyvalue->value, fun_process);
}

object_o keyvalue_str(object_o object)
{
	keyvalue_o *keyvalue = (keyvalue_o *)object;
	SMART string_o *key = NULL, *value = NULL;

	if (object_typeof(keyvalue->key) == NT_STRING_O)
	{
		key = string_format("\"%q\"", keyvalue->key);
	}
	else
	{
		key = str(keyvalue->key);
	}

	if (object_typeof(keyvalue->value) == NT_STRING_O)
	{
		value = string_format("\"%q\"", keyvalue->value);
	}
	else
	{
		value = str(keyvalue->value);
	}

	return string_format("%q: %q", key, value);
}

bool keyvalue_equal(object_o a, object_o b)
{
	keyvalue_o *keyvalue1 = (keyvalue_o *)a;
	keyvalue_o *keyvalue2 = (keyvalue_o *)b;

	return equal(keyvalue1->key, keyvalue2->key);
}

object_o keyvalue_clone(object_o object)
{
	keyvalue_o *src = (keyvalue_o *)object;

	keyvalue_o *key_value = new_keyvalue(clone(src->key), clone(src->value));

	return (object_o)key_value;
}

keyvalue_o *new_keyvalue(object_o key, object_o value)
{
	keyvalue_o *key_val;

	data_type_e key_type = object_typeof(key);

	if (key_type != NT_INT_O &&
		key_type != NT_STRING_O &&
		key_type != NT_RSTRING_O)
	{
		return NULL;
	}

	key_val = alloc(1, sizeof(keyvalue_o), NT_KEYVALUE_O);
	key_val->key = key;
	key_val->value = value;

	static const vtable_t vt = {
		.__str__ = keyvalue_str,
		.__clone__ = keyvalue_clone,
		.__equals__ = keyvalue_equal,
		.__visitor__ = keyvalue_visitor};

	object_reg_dunders(key_val, &vt);

	return key_val;
}

void keyvalue_set_key(keyvalue_o *keyvalue, object_o key)
{
	object_o to_drop = keyvalue->key;

	DROP(to_drop);

	keyvalue->key = key;
}

void keyvalue_set_value(keyvalue_o *keyvalue, object_o value)
{
	object_o to_drop = keyvalue->value;

	DROP(to_drop);

	keyvalue->value = value;
}