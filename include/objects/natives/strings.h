#pragma once
#ifndef STRINGS_H
#define STRINGS_H

#include <stdarg.h>

#include "objects/pool/pool.h"
#include "objects/pool/alloc.h"
#include "objects/owner/owner.h"
#include "objects/utils/utils.h"

typedef struct string_o
{
	int index;
	int length;
	char *str;
} string_o;

/*
String objects
*/
string_o *new_string(int length);
string_o *string_from(const char *str, ...);
string_o *string_format(const char *format, ...);

int string_index(string_o *str);
int string_length(string_o *str);

char string_get_char(string_o *str);
char *cstring(string_o *str);
char *cstring_abs(string_o *str);

bool string_at_end(string_o *str);
char string_at(string_o *str, int pos);

string_o *string_push(string_o *self, string_o *src);
string_o *string_slice(string_o *self, int start, int end);

void string_rewind(string_o *str);
string_o *string_next(string_o *str);
string_o *string_prev(string_o *str);

#endif