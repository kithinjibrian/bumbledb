#pragma once
#ifndef GREATER_H
#define GREATER_H

#include "objects/pool/pool.h"
#include "objects/natives/error.h"

object_o greater(object_o a, object_o b);
object_o greater_or_equals(object_o a, object_o b);

#endif