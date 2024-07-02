#pragma once
#ifndef HTABLE_H
#define HTABLE_H

#include "objects/pool/pool.h"
#include "objects/pool/alloc.h"
#include "objects/owner/owner.h"
#include "objects/utils/utils.h"
#include "objects/natives/list.h"
#include "objects/natives/strings.h"
#include "objects/natives/keyvalue.h"

#define LOAD_FACTOR_THRESHOLD 0.7

typedef struct list_node_o list_node_o;

typedef struct htable_o
{
	int size;
	int elements;
	list_node_o **table;
} htable_o;

htable_o *new_htable(int size);

bool htable_has(htable_o *htable, object_o key);
bool htable_delete(htable_o *htable, object_o key);
object_o htable_get(htable_o *htable, object_o key);
void htable_set(htable_o *htable, object_o key, object_o value);

#endif