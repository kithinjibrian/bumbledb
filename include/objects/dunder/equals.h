#pragma once
#ifndef EQUALS_H
#define EQUALS_H

#include "objects/pool/pool.h"
#include "objects/natives/error.h"

bool equal(object_o a, object_o b);
object_o is_not_equal(object_o a, object_o b);

#endif