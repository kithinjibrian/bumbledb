#include "fs/vfs/vfs.h"

static icache_o *icache_g = NULL;

icache_o *icache_init(void)
{
	if (icache_g == NULL)
	{
		icache_g = new_lru_cache(20);
		pool_globals_add(icache_g);
		return icache_g;
	}
	return icache_g;
}

void icache_save(object_o key, object_o value)
{
	icache_o *icache;
	icache = icache_init();

	lru_cache_save(icache, MOVE(key), MOVE(value));
}

object_o icache_get(object_o key)
{
	lru_cache_o *icache;
	icache = icache_init();

	return lru_cache_get(icache, MOVE(key));
}

void icache_print()
{
	icache_o *icache;
	icache = icache_init();

	print(icache);
}

void icache_free()
{
	icache_o *icache;
	icache = icache_init();

	lru_cache_free(MOVE(icache));
}