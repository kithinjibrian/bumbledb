#include "objects/dunder/equals.h"

bool equal(object_o a, object_o b)
{
	if (object_typeof(a) != object_typeof(b))
	{
		return false;
	}
	else
	{
		object_header_t *object_header = get_object_header(a);

		if (object_header->vtable && object_header->vtable->__equals__)
		{
			return object_header->vtable->__equals__(a, b);
		}
	}

	return false;
}

object_o is_not_equal(object_o a, object_o b)
{
	if (object_typeof(a) != object_typeof(b))
	{
		return number(1);
	}
	else
	{
		object_header_t *object_header = get_object_header(a);

		if (object_header->vtable && object_header->vtable->__is_not_equals__)
		{
			return object_header->vtable->__is_not_equals__(a, b);
		}
	}

	return number(1);
}