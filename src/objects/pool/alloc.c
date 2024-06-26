#include "objects/pool/alloc.h"

void *alloc(int count, size_t size, data_type_e data_type)
{
	object_o object;
	object_header_t *object_header;

	object = b_alloc(count, size, data_type);

	object_header = get_object_header(object);

	if (!object_header)
		return NULL;

	object_header->data_info.id = pool_get_object_id();

	pool_add_object(&object_header);

	pool_htable_insert(object);

	return object;
}

void *ralloc(object_o object, int count, size_t size)
{
	object_o old_ptr = object;
	object_o new_ptr = b_ralloc(object, count, size);
	if (old_ptr != new_ptr)
	{
		pool_htable_remove(old_ptr);
		pool_htable_insert(new_ptr);
	}
	return new_ptr;
}

void retain(object_o object)
{
	b_retain(object);
}

void release(object_o *object)
{
	if (object != NULL && *object != NULL)
	{
		b_release(*object, pool_remove_object);
		*object = NULL;
	}
}

void release_keep(object_o object)
{
	b_release(object, pool_remove_object);
}