#include "objects/dunder/clone.h"

object_o clone(object_o object)
{
	object_header_t *object_header = get_object_header(object);

	if (object_header->vtable && object_header->vtable->__clone__)
	{
		return object_header->vtable->__clone__(object);
	}

	return error("fun clone(): Clone dunder method not provided", PRINT | PANIC);
}