#include "objects/dunder/greater.h"

object_o greater(object_o a, object_o b)
{

	object_header_t *object_header = get_object_header(a);

	if (object_header->vtable && object_header->vtable->__greater__)
	{
		return object_header->vtable->__greater__(a, b);
	}

	return error("fun greater(): Greater dunder method not provided", PRINT | PANIC);
}

object_o greater_or_equals(object_o a, object_o b)
{

	object_header_t *object_header = get_object_header(a);

	if (object_header->vtable && object_header->vtable->__greater_or_equals__)
	{
		return object_header->vtable->__greater_or_equals__(a, b);
	}

	return error("fun greater_or_equals(): GreaterOrEquals dunder method not provided", PRINT | PANIC);
}