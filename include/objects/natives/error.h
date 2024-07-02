#pragma once
#ifndef ERROR_H
#define ERROR_H

#include <stdarg.h>

#include "objects/pool/pool.h"
#include "objects/pool/alloc.h"
#include "objects/owner/owner.h"
#include "objects/utils/utils.h"
#include "objects/natives/strings.h"

#define LOG (1 << 0)
#define PRINT (1 << 1)
#define PANIC (1 << 2)
#define SILENT (1 << 3)

typedef struct error_o error_o;

bool is_error(object_o obj);

int error_type(error_o *error);
const char *error_get_error(error_o *error);

object_o error(const char *err, int type);

#endif