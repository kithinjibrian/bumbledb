#pragma once
#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

#include "objects/pool/pool.h"

#define MUTABLE(object)                                                             \
	do                                                                              \
	{                                                                               \
		if (!object_ismutate(object))                                               \
		{                                                                           \
			error("macro MUTABLE(): Mutating an immutable object!", PRINT | PANIC); \
		}                                                                           \
	} while (0)

void object_info_all();
void object_info_locals();
int object_count(object_o object);
void object_info(object_o object);
int object_typeof(object_o object);
size_t object_size(object_o object);
int object_refcount(object_o object);
void object_is_global(object_o object);
void object_info_child(object_o object);
const char *object_typeof_str(object_o object);
void object_name(char name[], object_o object);
const char *object_typeof_str2(data_type_e data_type);
void object_reg_dunders(object_o object, const vtable_t *vtable);

#endif