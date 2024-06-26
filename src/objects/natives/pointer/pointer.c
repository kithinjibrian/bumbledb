#include "objects/natives/pointer/pointer.h"

void deinit_pointer(object_o object)
{
	free(*((object_o *)object));
}

object_o pointer(void *ptr)
{
	object_o *object;

	object = alloc(1, sizeof(void *), NT_POINTER_O);

	static const vtable_t vt = {.__deinit__ = deinit_pointer};

	object_reg_dunders(object, &vt);

	*object = ptr;

	return object;
}