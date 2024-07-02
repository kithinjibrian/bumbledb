#include "objects/owner/drop.h"

void __drop__(object_header_t *object_header, object_o object)
{
	if (object != NULL)
	{
		b_release(object, pool_remove_object);
	}
}

void drop(object_o *object)
{
	if (object != NULL && *object != NULL)
	{
		if (!pool_htable_has(*object))
		{
			printf("Double free for pointer: %p!!!\n", *object);
			exit(1);
		}

		post_visit(*object, __drop__);
		*object = NULL;
	}
}

void drop_keep(object_o object)
{
	if (object != NULL)
	{
		post_visit(object, __drop__);
	}
}

void drop_automatic(void *ptr)
{
	void *resource = (*(void **)ptr);

	if (resource)
	{
		DROP(resource);
	}
}

void drop_automatic_raw(void *ptr)
{
	void *resource = (*(void **)ptr);
	free(resource);
}

void drop_globals()
{
	object_node_t *globals = pool_get_globals();

	while (globals)
	{
		DROP(globals->object);
		globals = globals->next;
	}
}