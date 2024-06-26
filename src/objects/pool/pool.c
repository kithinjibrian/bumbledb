#include "objects/pool/pool.h"

static object_pool_t *pool_g = NULL;

object_pool_t *pool_get()
{
	if (pool_g == NULL)
	{
		pool_g = malloc(sizeof(object_pool_t));

		if (!pool_g)
			return NULL;

		pool_g->size = 0;
		pool_g->count = 0;
		pool_g->root = NULL;
		pool_g->globals = NULL;
		pool_g->htable = pool_new_htable(500);

		pthread_mutex_init(&(pool_g->lock), NULL);

		return pool_g;
	}

	return pool_g;
}

object_htable_t *pool_new_htable(int hash_size)
{
	object_htable_t *htable = malloc(sizeof(object_htable_t));
	htable->__hash_size__ = hash_size;
	htable->__num_elements__ = 0;
	htable->__objects__ = calloc(hash_size, sizeof(object_node_t *));
	return htable;
}

object_htable_t *pool_htable_get()
{
	object_pool_t *object_pool;
	object_pool = pool_get();

	return object_pool->htable;
}

object_node_t *pool_get_globals()
{
	object_pool_t *object_pool;

	object_pool = pool_get();

	return object_pool->globals;
}

void pool_htable_free(object_htable_t *htable)
{
	for (size_t i = 0; i < htable->__hash_size__; i++)
	{
		object_node_t *node = htable->__objects__[i];
		while (node)
		{
			object_node_t *temp = node;
			node = node->next;
			free(temp);
		}
	}
	free(htable->__objects__);
	free(htable);
}

void pool_globals_free()
{
	object_node_t *current = pool_get_globals();
	while (current)
	{
		object_node_t *temp = current;
		current = current->next;
		free(temp);
	}
}

object_header_t *pool_get_root()
{
	object_pool_t *object_pool;

	object_pool = pool_get();

	return object_pool->root;
}

int pool_get_object_id()
{
	object_pool_t *pool = pool_get();
	return pool->size;
}

void pool_add_object(object_header_t **object_header)
{
	object_pool_t *pool = pool_get();

	pthread_mutex_lock(&(pool->lock));

	if (pool->root)
	{
		(*object_header)->next = pool->root;
		pool->root = *object_header;
	}
	else
	{
		pool->root = *object_header;
	}
	pool->count++;
	pool->size++;

	pthread_mutex_unlock(&(pool->lock));
}

void pool_free_object(object_header_t *object_header)
{

	object_pool_t *object_pool = pool_get();
	object_pool->count--;

	pthread_mutex_destroy(&(object_header->lock));

	if (object_header->vtable && object_header->vtable->__deinit__)
	{
		object_header->vtable->__deinit__(get_object(object_header));
	}

	pool_htable_remove(get_object(object_header));

	free(object_header->object);
	free(object_header);
}

void pool_remove_object(object_header_t *object_header)
{
	object_pool_t *pool = pool_get();
	object_header_t *temp = pool->root, *prev;

	int object_id = object_header->data_info.id;

	pthread_mutex_lock(&(pool->lock));

	if (temp != NULL && temp->data_info.id == object_id)
	{
		pool->root = temp->next;
		pool_free_object(temp);
		pthread_mutex_unlock(&(pool->lock));
		return;
	}

	while (temp != NULL && temp->data_info.id != object_id)
	{
		prev = temp;
		temp = temp->next;
	}

	if (temp == NULL)
	{
		pool_free_object(temp);
		pthread_mutex_unlock(&(pool->lock));
		return;
	}

	prev->next = temp->next;
	pool_free_object(temp);
	pthread_mutex_unlock(&(pool->lock));
}

void pool_clean(void)
{
	object_pool_t *object_pool;

	object_pool = pool_get();

	pool_globals_free();
	pool_htable_free(object_pool->htable);

	free(object_pool);
}