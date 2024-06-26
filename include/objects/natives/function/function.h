#pragma once
#ifndef FUNCTION_H
#define FUNCTION_H

#include <stdarg.h>

#include "objects/pool/pool.h"
#include "objects/pool/alloc.h"

typedef void (*fun_generic_t)(void);
typedef struct function_o function_o;

const char *function_name(function_o *function);
fun_generic_t function_fun(function_o *function);
function_o *function(const char *name, fun_generic_t fun_generic);

#endif