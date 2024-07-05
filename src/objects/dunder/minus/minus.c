#include "objects/dunder/minus.h"

object_o minus(object_o a, object_o b)
{

	object_header_t *object_header = get_object_header(a);

	if (object_header->vtable && object_header->vtable->__minus__)
	{
		return object_header->vtable->__minus__(a, b);
	}

	return error("fun minus(): Minus dunder method not provided", PRINT | PANIC);
}