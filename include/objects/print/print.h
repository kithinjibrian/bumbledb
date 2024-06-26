#pragma once
#ifndef PRINT_H
#define PRINT_H

#include <ctype.h>

#include "objects/str/str.h"
#include "objects/pool/alloc.h"
#include "objects/utils/utils.h"
#include "objects/natives/natives.h"

#define RED "\x1b[31m"
#define RESET "\x1b[0m"
#define GREY "\x1b[90m"
#define BLUE "\x1b[34m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define MAGENTA "\x1b[35m"

void print(object_o object);
void print(object_o object);
void print_offset(object_o object, int depth);
void print_hex(unsigned char *data, size_t length);

#endif