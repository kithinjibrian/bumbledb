#include "objects/dunder/add.h"

object_o add(object_o a, object_o b)
{
	
	object_header_t *object_header = get_object_header(a);

	if (object_header->vtable && object_header->vtable->__add__)
	{
		return object_header->vtable->__add__(a, b);
	}

	return error("fun add(): Add dunder method not provided", PRINT | PANIC);
}