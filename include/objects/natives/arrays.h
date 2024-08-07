#pragma once
#ifndef ARRAYS_H
#define ARRAYS_H

#include <stdarg.h>

#include "objects/pool/pool.h"
#include "objects/pool/alloc.h"
#include "objects/owner/owner.h"
#include "objects/utils/utils.h"
#include "objects/natives/strings.h"

typedef struct string_o string_o;

typedef struct array_o
{
	int index;
	int length;
	object_o *array;
} array_o;

/*
 param 1: current object
 param 2: args
 param 3: array
*/
typedef bool (*fun_array_filter_t)(object_o, object_o, array_o *);

array_o *array_from(int length, ...);

array_o *new_array(int length);

object_o array_pop(array_o *array);
object_o array_push(array_o *array, object_o object);

object_o array_at(array_o *array, int pos);
bool array_insert(array_o *array, int pos, object_o object);

string_o *array_join(array_o *array, const char *del);

void array_rewind(array_o *array);
bool array_at_end(array_o *array);
bool array_next(array_o *array);
bool array_prev(array_o *array);
object_o array_current(array_o *array);

int array_find_index(array_o *array, fun_array_filter_t array_filter, object_o args);
object_o array_filter(array_o *array, fun_array_filter_t array_filter, object_o args);

#endif