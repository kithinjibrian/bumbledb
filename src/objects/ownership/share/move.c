#include "objects/share/share.h"

object_o move(object_o *object)
{
	if (!object || !*object)
	{
		error("fun move(): Object is null!\n", LOG | PRINT | PANIC);
	}

	void *ptr = *object;
	*object = NULL;

	return ptr;
}