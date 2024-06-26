#pragma once
#ifndef CACHE_H
#define CACHE_H

#include "objects/object.h"

typedef struct lru_cache_o lru_cache_o;

void lru_cache_free(lru_cache_o *lru_cache);
lru_cache_o *new_lru_cache(int capacity);
void lru_cache_print(object_o lru_cache, int depth);
void lru_cache_delete(lru_cache_o *lru_cache, object_o key);
object_o lru_cache_get(lru_cache_o *lru_cache, object_o key);
void lru_cache_save(lru_cache_o *lru_cache, object_o key, object_o value);

void cache_free();
void cache_print();
void cache_restore();
void cache_delete(object_o key);
object_o cache_get(object_o key);
void cache_save(object_o key, object_o value);

#endif