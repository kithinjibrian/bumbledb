#pragma once
#ifndef STATE_H
#define STATE_H

#include "objects/pool/pool.h"
#include "objects/share/share.h"
#include "objects/natives/error/error.h"

object_o get_state(object_o object);
object_o set_state(object_o object, object_o state);

#endif