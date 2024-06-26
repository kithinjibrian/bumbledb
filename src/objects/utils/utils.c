#include "objects/utils/utils.h"

static const char *type_string_g[] = {
	TYPES(GENERATE_STRING_DT)};

void object_reg_dunders(object_o object, const vtable_t *vtable)
{
	object_header_t *object_header;
	object_header = get_object_header(object);

	object_header->vtable = vtable;
}

int object_refcount(object_o object)
{
	object_header_t *object_header;

	object_header = get_object_header(object);

	return object_header->data_info.ref;
}

int object_typeof(object_o object)
{
	object_header_t *object_header;

	object_header = get_object_header(object);

	return object_header->data_info.data_type;
}

const char *object_typeof_str(object_o object)
{
	return type_string_g[object_typeof(object)];
}

const char *object_typeof_str2(data_type_e data_type)
{
	return type_string_g[data_type];
}

int object_count(object_o object)
{
	object_header_t *object_header;

	object_header = get_object_header(object);

	return object_header->data_info.count;
}

size_t object_size(object_o object)
{
	object_header_t *object_header;

	object_header = get_object_header(object);

	return object_header->data_info.size;
}

void object_info(object_o object)
{
	object_header_t *object_header;
	object_header = get_object_header(object);

	printf("\n-----------------------------------------\n");
	printf("\n		Object info\n");
	printf("\n-----------------------------------------\n");
	printf("ID: %d\n", object_header->data_info.id);
	printf("Data Type: %s\n", object_typeof_str2(object_header->data_info.data_type));
	printf("Ref Count: %d\n", object_header->data_info.ref);
	printf("Data Size: %ld\n", object_header->data_info.size);
	printf("Data Count: %d\n", object_header->data_info.count);
	printf("Data Address: %p\n", (void *)object_header->object);
	printf("Object Address: %p\n\n", (void *)object_header);
}

void object_info_all()
{
	object_header_t *current = pool_get_root();

	printf("\n-----------------------------------------\n");
	printf("\n		Object info\n");

	while (current != NULL)
	{
		printf("\n-----------------------------------------\n");
		printf("ID: %d\n", current->data_info.id);
		printf("Data Type: %s\n", object_typeof_str2(current->data_info.data_type));
		printf("Ref Count: %d\n", current->data_info.ref);
		printf("Data Size: %ld\n", current->data_info.size);
		printf("Data Count: %d\n", current->data_info.count);
		printf("Data Address: %p\n", (void *)current->object);
		printf("Object Address: %p\n\n", (void *)current);
		current = current->next;
	}
}