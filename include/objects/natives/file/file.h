#pragma once
#ifndef FILE_H
#define FILE_H

#include "stdlib.h"

#include "objects/drop/drop.h"
#include "objects/pool/pool.h"
#include "objects/pool/alloc.h"
#include "objects/share/share.h"
#include "objects/natives/error/error.h"

typedef struct file_o file_o;

FILE *f_get(file_o *file);
file_o *f_open(const char *filename, const char *modes);

#endif