#include "objects/dunder/modulo.h"

object_o modulo(object_o a, object_o b)
{

	object_header_t *object_header = get_object_header(a);

	if (object_header->vtable && object_header->vtable->__modulo__)
	{
		return object_header->vtable->__modulo__(a, b);
	}

	return error("fun modulo(): Modulo dunder method not provided", PRINT | PANIC);
}