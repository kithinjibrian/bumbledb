#pragma once
#ifndef FUNCTION_H
#define FUNCTION_H

#include <stdarg.h>

#include "objects/pool/pool.h"
#include "objects/pool/alloc.h"
#include "objects/utils/utils.h"
#include "objects/natives/strings.h"

typedef void (*fun_generic_t)(void);

typedef struct function_o
{
	const char *name;
	fun_generic_t function;
} function_o;

function_o *function(const char *name, fun_generic_t fun_generic);

#endif