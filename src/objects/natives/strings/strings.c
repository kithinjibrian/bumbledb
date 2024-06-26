#include "objects/print/print.h"
#include "objects/natives/strings/strings.h"

struct string_o
{
	int index;
	int length;
	char *str;
};

bool string_equals(object_o str1, object_o str2)
{
	string_o *str1_o = (string_o *)str1;
	string_o *str2_o = (string_o *)str2;
	return strcmp(str1_o->str, str2_o->str) == 0 ? true : false;
}

object_o string_str(object_o object)
{
	return clone(object);
}

object_o string_clone(object_o object)
{
	string_o *str = (string_o *)object;

	string_o *new_str = new_string(str->length);

	strncpy(new_str->str, str->str, str->length);

	return new_str;
}

string_o *string_format(const char *format, ...)
{
	va_list args, args_copy;
	va_start(args, format);
	va_start(args_copy, format);

	const char *ch;
	int length = 0;

	for (ch = format; *ch != '\0'; ch++)
	{
		if (*ch == '%')
		{
			ch++;
			if (*ch == 'q')
			{
				object_o *object = va_arg(args, object_o);
				string_o *str_rep = str(object);

				length += str_rep->length - 1;

				DROP(str_rep);
			}
		}
		else
		{
			length++;
		}
	}

	string_o *dest = new_string(length + 1);

	char *dest_ptr = dest->str;
	for (ch = format; *ch != '\0'; ch++)
	{
		if (*ch == '%')
		{
			ch++;
			if (*ch == 'q')
			{
				object_o *object = va_arg(args_copy, object_o);
				string_o *str_rep = str(object);

				strncpy(dest_ptr, str_rep->str, str_rep->length - 1);
				dest_ptr += str_rep->length - 1;

				DROP(str_rep);
			}
		}
		else
		{
			*dest_ptr++ = *ch;
		}
	}

	*dest_ptr = '\0';

	va_end(args);
	va_end(args_copy);

	return dest;
}

string_o *string_from(const char *format, ...)
{
	va_list args, args_copy;
	va_start(args, format);
	va_copy(args_copy, args);

	int length = vsnprintf(NULL, 0, format, args);

	if (length < 0)
	{
		return NULL;
	}

	string_o *dest = new_string(length + 1);

	vsprintf(dest->str, format, args_copy);

	va_end(args);
	va_end(args_copy);

	return dest;
}

string_o *new_string(int length)
{
	string_o *str = alloc(1, sizeof(string_o) + (length * sizeof(char)), NT_STRING_O);
	str->index = 0;
	str->length = length;
	str->str = (char *)(str + 1);

	static const vtable_t vt = {
		.__str__ = string_str,
		.__clone__ = string_clone,
		.__equals__ = string_equals};

	object_reg_dunders(str, &vt);

	return str;
}

int string_length(string_o *str)
{
	return str->length;
}

int string_index(string_o *str)
{
	return str->index;
}

char *cstring_abs(string_o *str)
{
	return str->str;
}

char *cstring(string_o *str)
{
	return str->str + str->index;
}

char string_get_char(string_o *str)
{
	return str->str[str->index];
}

char string_at(string_o *str, int pos)
{
	if (pos > str->length - 1)
	{
		error("Index out of bounds", PRINT | PANIC);
	}
	return str->str[pos];
}

string_o *string_slice(string_o *self, int start, int end)
{
	int len = (end - start) + 1;

	string_o *dest = new_string(len);

	strncpy(dest->str, self->str + start, len - 1);

	return dest;
}

string_o *string_push(string_o *self, string_o *src)
{
	SMART string_o *smt_src = src;
	SMART string_o *smt_self = self;
	int new_length = (smt_self->length + smt_src->length) - 1;

	string_o *new_str = new_string(new_length);

	strncpy(new_str->str, smt_self->str, smt_self->length - 1);
	strncpy(new_str->str + (smt_self->length - 1), smt_src->str, smt_src->length);

	return new_str;
}

string_o *string_next(string_o *str)
{
	int pos = str->index;
	if (pos >= str->length)
	{
		return NULL;
	}

	str->index++;

	return str;
}

string_o *string_prev(string_o *str)
{
	str->index--;

	int pos = str->index;
	if (pos < 0)
	{
		str->index++;
		return NULL;
	}

	return str;
}

bool string_at_end(string_o *str)
{
	return str->index >= str->length;
}

void string_rewind(string_o *str)
{
	str->index = 0;
}