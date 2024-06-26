#pragma once
#ifndef NTREE_H
#define NTREE_H

#include "objects/object.h"

typedef struct ntree_o ntree_o;

ntree_o *new_ntree(object_o object);
void ntree_add(ntree_o *root, ntree_o *child);

#endif