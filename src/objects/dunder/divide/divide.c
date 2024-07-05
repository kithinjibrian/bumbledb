#include "objects/dunder/add.h"

object_o divide(object_o a, object_o b)
{

	object_header_t *object_header = get_object_header(a);

	if (object_header->vtable && object_header->vtable->__divide__)
	{
		return object_header->vtable->__divide__(a, b);
	}

	return error("fun divide(): Divide dunder method not provided", PRINT | PANIC);
}