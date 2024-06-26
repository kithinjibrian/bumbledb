#include "cache/cache.h"

struct lru_cache_o
{
	int size;
	int capacity;
	htable_o *htable;
	list_node_o *queue;
};

void lru_cache_visitor(object_o object, fun_process_t fun_process)
{
	lru_cache_o *lru_cache = (lru_cache_o *)object;
	post_visit(lru_cache->htable, fun_process);
	if (lru_cache->queue)
	{
		post_visit(lru_cache->queue, fun_process);
	}
}

object_o lru_cache_str(object_o object)
{
	lru_cache_o *lru_cache = (lru_cache_o *)object;

	return string_format("{ htable: %q, queue: %q }", str(lru_cache->htable), str(lru_cache->queue));
}

lru_cache_o *new_lru_cache(int capacity)
{
	lru_cache_o *lru_cache;

	lru_cache = alloc(1, sizeof(lru_cache_o), LRU_CACHE_O);

	lru_cache->capacity = capacity;
	lru_cache->htable = new_htable(capacity);
	lru_cache->size = 0;
	lru_cache->queue = NULL;

	static const vtable_t vt = {
		.__str__ = lru_cache_str,
		.__visitor__ = lru_cache_visitor};

	object_reg_dunders(lru_cache, &vt);

	return lru_cache;
}

void lru_cache_save(lru_cache_o *lru_cache, object_o key, object_o value)
{
	SMART object_o smt_key = key;
	SMART object_o smt_value = value;

	if (htable_has(lru_cache->htable, share(smt_key)))
	{
		list_delete(&(lru_cache->queue), equal, share(smt_key));
		list_unshift(&(lru_cache->queue), share(smt_key));

		htable_set(lru_cache->htable, share(smt_key), MOVE(smt_value));
	}
	else
	{
		if (lru_cache->size == lru_cache->capacity)
		{
			char *key = (char *)list_pop(&(lru_cache->queue));
			htable_delete(lru_cache->htable, share(key));
			lru_cache->size--;
		}

		htable_set(lru_cache->htable, share(smt_key), MOVE(smt_value));

		list_unshift(&(lru_cache->queue), share(smt_key));

		lru_cache->size++;
	}
}

object_o lru_cache_get(lru_cache_o *lru_cache, object_o key)
{
	SMART object_o smt_key = key;
	object_o object = NULL;

	if ((object = htable_get(lru_cache->htable, share(smt_key))))
	{
		list_delete(&(lru_cache->queue), equal, share(smt_key));
		list_unshift(&(lru_cache->queue), share(smt_key));

		return object;
	}
	return NULL;
}

void lru_cache_delete(lru_cache_o *lru_cache, object_o key)
{
	SMART object_o smt_key = key;
	list_delete(&(lru_cache->queue), equal, share(smt_key));
	htable_delete(lru_cache->htable, share(smt_key));
}

void lru_cache_free(lru_cache_o *lru_cache)
{
	DROP(lru_cache->htable);
	DROP(lru_cache->queue);
	DROP(lru_cache);
}