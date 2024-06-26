#pragma once
#ifndef HTABLE_H
#define HTABLE_H

#include "objects/pool/pool.h"
#include "objects/pool/alloc.h"
#include "objects/share/share.h"
#include "objects/print/print.h"
#include "objects/natives/list/list.h"
#include "objects/natives/keyvalue/keyvalue.h"

#define LOAD_FACTOR_THRESHOLD 0.7

typedef struct htable_o htable_o;

htable_o *new_htable(int size);
int htable_get_size(htable_o *htable);
int htable_get_elements(htable_o *htable);
array_o *htable_get_table(htable_o *htable);

bool htable_has(htable_o *htable, object_o key);
bool htable_delete(htable_o *htable, object_o key);
object_o htable_get(htable_o *htable, object_o key);
list_node_o *htable_get_list(htable_o *htable, object_o key);
void htable_set(htable_o *htable, object_o key, object_o value);

#endif