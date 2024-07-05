#include "objects/dunder/mult.h"

object_o mult(object_o a, object_o b)
{

	object_header_t *object_header = get_object_header(a);

	if (object_header->vtable && object_header->vtable->__mult__)
	{
		return object_header->vtable->__mult__(a, b);
	}

	return error("fun mult(): Mult dunder method not provided", PRINT | PANIC);
}