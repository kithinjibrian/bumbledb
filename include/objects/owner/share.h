#pragma once
#ifndef SHARE_H
#define SHARE_H

#include "objects/pool/alloc.h"
#include "objects/utils/utils.h"
#include "objects/dunder/dunder.h"
#include "objects/natives/error.h"

/*
	Transfer owner
*/
#define MOVE(object) move((object_o *)&object)

/*
	Transfer owner to caller
*/
#define RETURN(object) return move((object_o *)&object)

object_o share(object_o object);
object_o move(object_o *object);

#endif