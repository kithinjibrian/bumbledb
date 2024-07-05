#include "objects/natives/arrays.h"

int get_len_4x(int length)
{
	int remainder = length % 4;
	int new_len = length + (4 - remainder);

	return new_len;
}

void array_visitor(object_o object, fun_process_t fun_process)
{
	array_o *array = (array_o *)object;
	for (int a = 0; a < array->length; a++)
	{
		if (array->array[a])
		{
			post_visit(array->array[a], fun_process);
		}
	}
	post_visit(array->array, fun_process);
}

object_o array_str(object_o object)
{
	array_o *array = (array_o *)object;
	SMART array_o *new_arr = new_array(0);

	for (int i = 0; i < array->length; i++)
	{
		if (object_typeof(array->array[i]) == NT_STRING_O)
		{
			array_push(new_arr, string_format("\"%q\"", array->array[i]));
		}
		else
		{
			array_push(new_arr, str(array->array[i]));
		}
	}

	SMART string_o *arr_str = array_join(new_arr, ", ");

	return string_format("[%q]", arr_str);
}

object_o array_add(object_o arr1, object_o arr2)
{
	array_o *array1 = (array_o *)arr1;
	array_o *array2 = (array_o *)arr2;
	int new_len = array1->length + array2->length;

	array_o *new_arr = new_array(new_len);

	for (int a = 0; a < array1->length; a++)
	{
		new_arr->array[a] = share(array1->array[a]);
	}

	for (int b = 0; b < array2->length; b++)
	{
		new_arr->array[array1->length + b] = share(array2->array[b]);
	}

	return new_arr;
}

array_o *array_from(int length, ...)
{
	va_list args;
	va_start(args, length);

	int new_len = get_len_4x(length);

	array_o *array = alloc(1, sizeof(array_o), NT_ARRAY_O);
	array->index = 0;
	array->length = length;
	array->array = alloc(new_len, sizeof(object_o), NT_ARRAY_OBJECTS_O);

	for (int a = 0; a < length; a++)
	{
		array->array[a] = va_arg(args, object_o);
	}

	static const vtable_t vt = {
		.__str__ = array_str,
		.__add__ = array_add,
		.__visitor__ = array_visitor};

	object_reg_dunders(array, &vt);

	va_end(args);

	return array;
}

array_o *new_array(int length)
{
	int new_len = get_len_4x(length);

	array_o *array = alloc(1, sizeof(array_o), NT_ARRAY_O);
	array->index = 0;
	array->length = length;
	array->array = alloc(new_len, sizeof(object_o), NT_ARRAY_OBJECTS_O);

	for (int a = 0; a < new_len; a++)
	{
		array->array[a] = NULL;
	}

	static const vtable_t vt = {
		.__str__ = array_str,
		.__add__ = array_add,
		.__visitor__ = array_visitor};

	object_reg_dunders(array, &vt);

	return array;
}

object_o array_shift(array_o *array, object_o object)
{
	return NULL;
}

object_o array_push(array_o *array, object_o object)
{
	array->array[array->length] = object;

	array->length++;

	if (array->length >= object_count(array->array))
	{
		int new_len = get_len_4x(array->length);

		array->array = ralloc(array->array, new_len, sizeof(object_o));
	}

	return object;
}

object_o array_pop(array_o *array)
{
	if (array->length == 0)
	{
		printf("Error: Array is empty. Cannot pop.\n");
		return NULL;
	}

	array_rewind(array);

	array->length--;

	object_o popped = array->array[array->length];

	array->array[array->length] = NULL;

	RETURN(popped);
}

bool array_insert(array_o *array, int pos, object_o object)
{
	if (pos > array->length - 1)
	{
		return false;
	}

	object_o to_drop = array->array[pos];

	if (pool_htable_has(to_drop))
	{
		DROP(to_drop);
	}

	array->array[pos] = object;

	return true;
}

object_o array_at(array_o *array, int pos)
{
	if (pos > array->length - 1)
	{
		return NULL;
	}

	return array->array[pos];
}

string_o *array_join(array_o *array, const char *del)
{
	string_o *cnt = string_from("");
	for (int a = 0; a < array->length; a++)
	{
		cnt = string_push(MOVE(cnt), share(array->array[a]));
		if (a != array->length - 1)
		{
			cnt = string_push(MOVE(cnt), string_from("%s", del));
		}
	}

	return cnt;
}

int array_find_index(array_o *array, fun_array_filter_t array_filter, object_o args)
{
	SMART object_o smt_args = args;
	for (int a = 0; a < array->length; a++)
	{
		if (array_filter(array->array[a], smt_args, array))
		{
			return a;
		}
	}
	return -1;
}

object_o array_filter(array_o *array, fun_array_filter_t array_filter, object_o args)
{
	for (int a = 0; a < array->length; a++)
	{
		if (array_filter(array->array[a], args, array))
		{
			return share(array->array[a]);
		}
	}
	return error("fun array_filter(): Object not found!", SILENT);
}

bool array_delete(array_o *array, int pos)
{

	if (pos > array->length - 1)
	{
		return false;
	}

	if (array->array[pos])
	{
		drop_keep(array->array[pos]);
	}

	array->array[pos] = NULL;
	return true;
}

object_o array_current(array_o *array)
{
	return array->array[array->index];
}

bool array_at_end(array_o *array)
{
	return array->index >= array->length;
}

bool array_next(array_o *array)
{
	int pos = array->index;
	if (pos > array->length - 1)
	{
		return 0;
	}

	array->index++;

	return 1;
}

bool array_prev(array_o *array)
{
	array->index--;

	int pos = array->index;
	if (pos < 0)
	{
		array->index++;
		return 0;
	}

	return 1;
}

void array_rewind(array_o *array)
{
	array->index = 0;
}