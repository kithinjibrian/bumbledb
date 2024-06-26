#include "objects/balloc/balloc.h"

object_header_t *get_object_header(object_o object)
{
	char *ptr = (char *)object;
	ptr -= sizeof(object_header_t *);

	return *((object_header_t **)ptr);
}

object_o get_object(object_header_t *object_header)
{
	return (object_o)((char *)object_header->object + sizeof(object_header_t *));
}

void *b_alloc(int count, size_t size, data_type_e data_type)
{
	object_o object;
	object_header_t *object_header;

	object_header = malloc(sizeof(object_header_t));
	if (!object_header)
	{
		return NULL;
	}

	object_header->next = NULL;
	object_header->vtable = NULL;
	object_header->magic_number = 0xABCD1234;

	pthread_mutex_init(&(object_header->lock), NULL);

	object_header->data_info.id = 0;
	object_header->data_info.ref = 1;
	object_header->data_info.size = size;
	object_header->data_info.count = count;
	object_header->data_info.data_type = data_type;

	object = calloc(count, size + sizeof(object_header_t *));
	if (!object)
	{
		free(object_header);
		return NULL;
	}

	*((object_header_t **)object) = object_header;

	object_header->object = (char *)object;

	return (char *)object + sizeof(object_header_t *);
}

void *b_ralloc(object_o object, int count, size_t size)
{
	char *obj;
	object_header_t *object_header;

	object_header = get_object_header(object);

	obj = realloc(object_header->object, (count * size) + sizeof(object_header_t *));
	if (!obj)
		return NULL;

	object_header->object = obj;

	object_header->data_info.size = size;
	object_header->data_info.count = count;

	return (char *)obj + sizeof(object_header_t *);
}

void b_retain(object_o object)
{
	object_header_t *object_header;

	object_header = get_object_header(object);

	pthread_mutex_lock(&(object_header->lock));

	object_header->data_info.ref++;

	pthread_mutex_unlock(&(object_header->lock));
}

void b_release(object_o object, fun_free_t free)
{

	object_header_t *object_header;

	object_header = get_object_header(object);

	pthread_mutex_lock(&(object_header->lock));

	object_header->data_info.ref--;

	pthread_mutex_unlock(&(object_header->lock));

	if (object_header->data_info.ref == 0)
	{
		free(object_header);
	}
}