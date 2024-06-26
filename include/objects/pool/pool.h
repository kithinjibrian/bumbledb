#pragma once
#ifndef POOL_H
#define POOL_H

#include <pthread.h>
#include <stdbool.h>

#include "objects/balloc/balloc.h"

typedef struct object_node_t
{
	void *object;
	struct object_node_t *next;
} object_node_t;

typedef struct object_htable_t
{
	/*private data*/
	int __hash_size__;
	int __num_elements__;
	object_node_t **__objects__;
} object_htable_t;

typedef struct object_pool_t
{
	int size;
	int count;
	pthread_mutex_t lock;
	object_header_t *root;
	object_node_t *globals;
	object_htable_t *htable;
} object_pool_t;

object_pool_t *pool_get();
object_header_t *pool_get_root();

int pool_get_object_id();

void pool_add_object(object_header_t **object_header);

void pool_remove_object(object_header_t *object_header);

void pool_clean(void);

void pool_htable_print();
object_htable_t *pool_htable_get();
bool pool_htable_has(void *object);
void pool_htable_insert(void *object);
void pool_htable_remove(void *object);
void pool_htable_free(object_htable_t *htable);
object_htable_t *pool_new_htable(int hash_size);

void pool_globals_print();
object_node_t *pool_get_globals();
void pool_globals_add(void *object);

#endif