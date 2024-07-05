#include "objects/dunder/lesser.h"

object_o lesser(object_o a, object_o b)
{

	object_header_t *object_header = get_object_header(a);

	if (object_header->vtable && object_header->vtable->__lesser__)
	{
		return object_header->vtable->__lesser__(a, b);
	}

	return error("fun lesser(): Lesser dunder method not provided", PRINT | PANIC);
}