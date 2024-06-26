#include "objects/equals/equals.h"
#include "objects/natives/keyvalue/keyvalue.h"

struct keyvalue_o
{
	object_o key;
	object_o value;
};

void keyvalue_visitor(object_o object, fun_process_t fun_process)
{
	keyvalue_o *keyvalue = (keyvalue_o *)object;
	post_visit(keyvalue->key, fun_process);
	post_visit(keyvalue->value, fun_process);
}

object_o keyvalue_d_str(object_o object)
{
	keyvalue_o *keyvalue = (keyvalue_o *)object;
	SMART string_o *key = NULL, *value = NULL;

	if (object_typeof(keyvalue_get_key(keyvalue)) == NT_STRING_O)
	{
		key = string_format("\"%q\"", keyvalue_get_key(keyvalue));
	}
	else
	{
		key = str(keyvalue_get_key(keyvalue));
	}

	if (object_typeof(keyvalue_get_value(keyvalue)) == NT_STRING_O)
	{
		value = string_format("\"%q\"", keyvalue_get_value(keyvalue));
	}
	else
	{
		value = str(keyvalue_get_value(keyvalue));
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

object_o keyvalue_get_state(object_o object)
{
	keyvalue_o *key_value = (keyvalue_o *)object;

	array_o *state = new_array(0);
	array_push(state, share(key_value->key));
	array_push(state, share(key_value->value));

	return state;
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
		.__str__ = keyvalue_d_str,
		.__clone__ = keyvalue_clone,
		.__equals__ = keyvalue_equal,
		.__visitor__ = keyvalue_visitor,
		.__get_state__ = keyvalue_get_state};

	object_reg_dunders(key_val, &vt);

	return key_val;
}

object_o keyvalue_get_key(keyvalue_o *keyvalue)
{
	return keyvalue->key;
}

object_o keyvalue_get_value(keyvalue_o *keyvalue)
{
	return keyvalue->value;
}

data_type_e keyvalue_get_key_type(keyvalue_o *keyvalue)
{
	return object_typeof(keyvalue->key);
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