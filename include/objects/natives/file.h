#pragma once
#ifndef FILE_H
#define FILE_H

#include <stdio.h>

#include "objects/pool/pool.h"
#include "objects/pool/alloc.h"
#include "objects/owner/owner.h"
#include "objects/utils/utils.h"
#include "objects/natives/error.h"

typedef struct file_o
{
	int size;
	FILE *file;
} file_o;

FILE *f_get(file_o *file);
file_o *f_open(const char *filename, const char *modes);

#endif