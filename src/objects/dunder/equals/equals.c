#include "objects/equals/equals.h"

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