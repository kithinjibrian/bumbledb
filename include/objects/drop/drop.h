#pragma once
#ifndef DROP_H
#define DROP_H

#include "objects/pool/alloc.h"
#include "objects/visit/visit.h"
#include "objects/utils/utils.h"

/*
	A smart object will be deallocated when the owner variable
	goes out of scope
*/
#define SMART __attribute__((cleanup(drop_automatic)))
#define SMART_RAW __attribute__((cleanup(drop_automatic_raw)))
#define SMART_MUT __attribute__((cleanup(drop_automatic_force)))

#define DROP(object) drop((object_o *)&(object))

void drop_globals();
void drop(object_o *object);
void drop_automatic(void *ptr);
void drop_keep(object_o object);
void drop_force(object_o *object);
void drop_automatic_raw(void *ptr);
void drop_automatic_force(void *ptr);

#endif