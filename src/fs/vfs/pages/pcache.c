#include "fs/vfs/vfs.h"

static pcache_o *pcache_g = NULL;

pcache_o *pcache_init(void)
{
	if (pcache_g == NULL)
	{
		pcache_g = new_lru_cache(20);
		pool_globals_add(pcache_g);
		return pcache_g;
	}
	return pcache_g;
}

void pcache_save(object_o key, uint8_t *value)
{
	pcache_o *pcache;
	pcache = pcache_init();

	lru_cache_save(pcache, MOVE(key), MOVE(value));
}

object_o pcache_get(object_o key)
{
	lru_cache_o *pcache;
	pcache = pcache_init();

	return lru_cache_get(pcache, MOVE(key));
}

void pcache_print()
{
	pcache_o *pcache;
	pcache = pcache_init();

	print(pcache);
}

void pcache_free()
{
	pcache_o *pcache;
	pcache = pcache_init();

	lru_cache_free(MOVE(pcache));
}