#include "objects/natives/pointer.h"

object_o pointer_str(object_o object)
{
	return string_from("%p", *(void **)object);
}

object_o pointer(object_o ptr)
{
	object_o object = alloc(1, sizeof(object_o), NT_POINTER_O);

	*(void **)object = ptr;

	static const vtable_t vt = {
		.__str__ = pointer_str};

	object_reg_dunders(object, &vt);

	return object;
}