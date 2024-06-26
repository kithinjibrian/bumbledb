#include "objects/pool/pool.h"

object_node_t *pool_global_node_create(void *object)
{
	object_node_t *node = malloc(sizeof(object_node_t));
	node->object = object;
	node->next = NULL;

	return node;
}

void pool_globals_add(void *object)
{
	object_pool_t *pool = pool_get();
	object_node_t *node = pool_global_node_create(object);

	if (!pool->globals)
	{
		pool->globals = node;
		return;
	}

	node->next = pool->globals;
	pool->globals = node;
}

void pool_globals_print()
{
	object_node_t *current = pool_get_globals();
	while (current)
	{
		printf("%p -> \n", current->object);
		current = current->next;
	}
}