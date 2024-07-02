#pragma once
#ifndef KEYVALUE_H
#define KEYVALUE_H

#include <stdbool.h>

#include "objects/pool/pool.h"
#include "objects/pool/alloc.h"
#include "objects/utils/utils.h"
#include "objects/natives/strings.h"

typedef struct keyvalue_o
{
	object_o key;
	object_o value;
} keyvalue_o;

typedef void (*fun_keyvalue_set_t)(keyvalue_o *, object_o);

keyvalue_o *new_keyvalue(object_o key, object_o value);

void keyvalue_set_key(keyvalue_o *keyvalue, object_o key);
void keyvalue_set_value(keyvalue_o *keyvalue, object_o value);

#endif