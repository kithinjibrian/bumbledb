#include "cache/cache.h"
#include "quill/quill.h"

static lru_cache_o *cache_g = NULL;

lru_cache_o *__cache__(void)
{
	if (cache_g == NULL)
	{
		cache_g = new_lru_cache(20);
		pool_globals_add(cache_g);
		return cache_g;
	}
	return cache_g;
}

void cache_restore()
{
	SMART file_o *file = f_open("cache.quill", "rb");
	if (!is_error(file))
	{
		SMART array_o *array = quill_read(file);
		while (!array_at_end(array))
		{
			array_o *object = (array_o *)array_current(array);

			cache_save(share(array_at(object, 0)), share(array_at(object, 1)));

			array_next(array);
		}
	}
}

void cache_save(object_o key, object_o value)
{
	lru_cache_o *cache;
	cache = __cache__();

	lru_cache_save(cache, MOVE(key), MOVE(value));
}

object_o cache_get(object_o key)
{
	lru_cache_o *cache;
	cache = __cache__();

	return lru_cache_get(cache, MOVE(key));
}

void cache_delete(object_o key)
{
	lru_cache_o *cache;
	cache = __cache__();

	lru_cache_delete(cache, MOVE(key));
}

void cache_print()
{
	lru_cache_o *cache;
	cache = __cache__();

	print(cache);
}

void cache_free()
{
	lru_cache_o *cache;
	cache = __cache__();

	lru_cache_free(MOVE(cache));
}