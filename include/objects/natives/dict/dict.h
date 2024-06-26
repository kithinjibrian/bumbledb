#pragma once
#ifndef DICT_H
#define DICT_H

#include "objects/pool/pool.h"
#include "objects/pool/alloc.h"
#include "objects/print/print.h"
#include "objects/share/share.h"
#include "objects/natives/arrays/arrays.h"
#include "objects/natives/keyvalue/keyvalue.h"

typedef array_o dict_o;

dict_o *new_dict(int size);
object_o dict_get(dict_o *self, object_o key);
void dict_set(dict_o *self, object_o key, object_o value);

#endif