#pragma once
#ifndef KEYVALUE_H
#define KEYVALUE_H

#include <stdbool.h>

#include "objects/drop/drop.h"
#include "objects/pool/pool.h"
#include "objects/pool/alloc.h"
#include "objects/utils/utils.h"
#include "objects/share/share.h"
#include "objects/clone/clone.h"

typedef struct keyvalue_o keyvalue_o;
typedef keyvalue_o *keyvalue_o_ptr;

typedef void (*fun_keyvalue_set_t)(keyvalue_o *, object_o);

keyvalue_o *new_keyvalue(object_o key, object_o value);

object_o keyvalue_get_key(keyvalue_o *keyvalue);
object_o keyvalue_get_value(keyvalue_o *keyvalue);
data_type_e keyvalue_get_key_type(keyvalue_o *keyvalue);

void keyvalue_set_key(keyvalue_o *keyvalue, object_o key);
void keyvalue_set_key_type(keyvalue_o *keyvalue, int key);
void keyvalue_set_value(keyvalue_o *keyvalue, object_o value);

keyvalue_o *keyvalue_int(object_o key, object_o value);
keyvalue_o *keyvalue_str(object_o key, object_o value);

bool key_compare(object_o key_val, object_o key);
bool keyvalue_compare(keyvalue_o *keyvalue1, keyvalue_o *keyvalue2);

#endif