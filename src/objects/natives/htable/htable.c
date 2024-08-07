#include "objects/natives/htable.h"

size_t __hash_calc__(char *key);

int get_len_4x2(int length)
{
	int remainder = length % 4;
	int new_len = length + (4 - remainder);

	return new_len;
}

size_t __hash_calc__(char *key)
{
	size_t hash = 2166136261u;
	while (*key)
	{
		hash ^= (unsigned char)(*key++);
		hash *= 16777619u;
	}
	return hash;
}

int hash(object_o object, htable_o *htable)
{
	int index = 0;
	if (object_typeof(object) == NT_INT_O)
	{
		return (*(int *)object) % htable->size;
	}
	else if (object_typeof(object) == NT_STRING_O)
	{
		return __hash_calc__(cstring(object)) % htable->size;
	}

	return index;
}

void htable_visitor(object_o object, fun_process_t fun_process)
{
	htable_o *htable = (htable_o *)object;
	for (int a = 0; a < htable->size; a++)
	{
		if (htable->table[a])
		{
			post_visit(htable->table[a], fun_process);
		}
	}
	post_visit(htable->table, fun_process);
}

object_o htable_str(object_o object)
{
	htable_o *htable = (htable_o *)object;
	SMART array_o *arr = new_array(0);
	for (int a = 0; a < htable->size; a++)
	{
		if (htable->table[a])
		{
			list_node_o *node = htable->table[a];
			while (node)
			{
				keyvalue_o *keyvalue = node->object;

				string_o *key = str(keyvalue);

				array_push(arr, key);

				node = node->next;
			}
		}
	}

	SMART string_o *joined = array_join(arr, ", ");

	return string_format("{%q}", joined);
}

htable_o *new_htable(int size)
{
	if (size == 0)
	{
		size = 1;
	}

	htable_o *htable;

	htable = alloc(1, sizeof(htable_o), NT_HASH_TABLE_O);

	htable->size = get_len_4x2(size);
	htable->elements = 0;
	htable->table = alloc(htable->size, sizeof(list_node_o *), NT_ARRAY_OBJECTS_O);

	static const vtable_t vt = {
		.__str__ = htable_str,
		.__visitor__ = htable_visitor};

	object_reg_dunders(htable, &vt);

	return htable;
}

void htable_resize(htable_o *htable)
{
	keyvalue_o *key_value;
	list_node_o *next;
	list_node_o *current;
	list_node_o **new_table;

	htable->size *= 2;
	new_table = alloc(htable->size, sizeof(list_node_o *), NT_ARRAY_OBJECTS_O);

	for (int a = 0; a < htable->size; a++)
	{
		current = htable->table[a];
		while (current)
		{
			next = current->next;
			key_value = current->object;
			int index = hash(key_value->key, htable);
			current->next = new_table[index];
			new_table[index] = current;
			current = next;
		}
	}

	DROP(htable->table);

	htable->table = new_table;
}

void htable_set(htable_o *htable, object_o key, object_o value)
{
	int index;
	list_node_o *node;
	keyvalue_o *key_value;
	SMART object_o *smt_key = key;
	SMART object_o *smt_value = value;

	if ((htable->elements / htable->size) > LOAD_FACTOR_THRESHOLD)
	{
		htable_resize(htable);
	}

	key_value = new_keyvalue(share(smt_key), share(smt_value));

	node = new_list_node(key_value);

	index = hash(smt_key, htable);

	if (htable->table[index] == NULL)
	{
		htable->table[index] = node;
	}
	else
	{
		if (htable_has(htable, share(smt_key)))
		{
			list_delete(&(htable->table[index]), equal, new_keyvalue(MOVE(smt_key), NULL));
			htable->elements--;
		}
		list_node_push(&(htable->table[index]), node);
	}

	htable->elements++;
}

object_o htable_get(htable_o *htable, object_o key)
{
	keyvalue_o *key_value;
	SMART object_o smt_key = key;

	int index = hash(smt_key, htable);

	key_value = (keyvalue_o *)list_find(htable->table[index], equal, new_keyvalue(MOVE(smt_key), NULL));

	if (key_value)
	{
		return key_value->value;
	}

	return NULL;
}

bool htable_delete(htable_o *htable, object_o key)
{
	SMART object_o smt_key = key;

	int index = hash(smt_key, htable);

	list_node_o *node = htable->table[index];

	list_delete(&node, equal, new_keyvalue(MOVE(smt_key), NULL));

	htable->elements--;

	return false;
}

bool htable_has(htable_o *htable, object_o key)
{
	if (htable_get(htable, key))
	{
		return true;
	}
	return false;
}