#include "objects/str/str.h"
#include "objects/natives/strings/strings.h"

object_o str(object_o object)
{
	if (!object)
	{
		return string_from("<nil>");
	}

	object_header_t *object_header = get_object_header(object);

	if (object_header->vtable && object_header->vtable->__str__)
	{
		return object_header->vtable->__str__(object);
	}

	return string_from("<object (%s)>", object_typeof_str(object));
}