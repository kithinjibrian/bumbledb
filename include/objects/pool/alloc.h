#pragma once
#ifndef ALLOC_H
#define ALLOC_H

#include "objects/pool/pool.h"

#define RELEASE(object) release((object_o *)&(object))

void *ralloc(object_o object, int count, size_t size);
void *alloc(int count, size_t size, data_type_e data_type);

void retain(object_o object);
void release(object_o *object);
void release_keep(object_o object);

#endif