#include "objects/pool/pool.h"

void pool_htable_insert(void *object)
{
	object_htable_t *htable = pool_htable_get();
	size_t index = ((size_t)object) % htable->__hash_size__;

	object_node_t *node = malloc(sizeof(object_node_t));
	node->object = object;
	node->next = htable->__objects__[index];
	htable->__objects__[index] = node;
}

bool pool_htable_has(void *object)
{
	object_htable_t *htable = pool_htable_get();
	size_t index = ((size_t)object) % htable->__hash_size__;

	object_node_t *node = htable->__objects__[index];

	while (node)
	{
		if (node->object == object)
		{
			return 1;
		}
		node = node->next;
	}
	return 0;
}

void pool_htable_remove(void *object)
{
	object_htable_t *htable = pool_htable_get();
	size_t index = ((size_t)object) % htable->__hash_size__;

	object_node_t *node = htable->__objects__[index];
	object_node_t *prev = NULL;

	while (node)
	{
		if (node->object == object)
		{
			if (prev)
			{
				prev->next = node->next;
			}
			else
			{
				htable->__objects__[index] = node->next;
			}
			free(node);
			return;
		}
		prev = node;
		node = node->next;
	}
}

void pool_htable_print()
{
	object_htable_t *htable = pool_htable_get();
	for (size_t i = 0; i < htable->__hash_size__; i++)
	{
		object_node_t *node = htable->__objects__[i];
		while (node)
		{
			printf("%p -> ", node->object);
			node = node->next;
		}
	}
	printf("\n");
}