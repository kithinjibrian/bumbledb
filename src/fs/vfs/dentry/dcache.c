#include "fs/vfs/vfs.h"

static dcache_o *dcache_g = NULL;

dcache_o *dcache_init(void)
{
	if (dcache_g == NULL)
	{
		dcache_g = new_lru_cache(20);
		pool_globals_add(dcache_g);
		return dcache_g;
	}
	return dcache_g;
}

void dcache_save(object_o key, object_o value)
{
	dcache_o *dcache;
	dcache = dcache_init();

	lru_cache_save(dcache, MOVE(key), MOVE(value));
}

object_o dcache_get(object_o key)
{
	lru_cache_o *dcache;
	dcache = dcache_init();

	return lru_cache_get(dcache, MOVE(key));
}

void dcache_print()
{
	dcache_o *dcache;
	dcache = dcache_init();

	print(dcache);
}

void dcache_free()
{
	dcache_o *dcache;
	dcache = dcache_init();

	lru_cache_free(MOVE(dcache));
}