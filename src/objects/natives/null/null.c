#include "objects/natives/null.h"

static object_o object = NULL;

object_o null_str(object_o object)
{
	(void)object;
	return string_from("(null)");
}

object_o null()
{
	if (object == NULL)
	{
		object = alloc(1, 0, NT_NULL_O);

		static const vtable_t vt = {
			.__str__ = null_str,
		};

		object_reg_dunders(object, &vt);

		return object;
	}

	return share(object);
}