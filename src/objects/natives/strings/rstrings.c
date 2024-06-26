#include "objects/natives/strings/strings.h"

object_o rstring_clone(object_o str)
{
	int length = rstring_length((char *)str) + 1;
	char *new_str = alloc(length, sizeof(char), NT_RSTRING_O);

	strncpy(new_str, (char *)str, length);

	return new_str;
}

char *new_rstring(char *str)
{
	char *dest;
	size_t len;

	len = strlen(str) + 1;
	dest = alloc(len, sizeof(char), NT_RSTRING_O);

	memcpy(dest, str, len);

	dest[len] = '\0';

	static const vtable_t vt = {.__clone__ = rstring_clone};

	object_reg_dunders(dest, &vt);

	return dest;
}

char *rstring_from(const char *str)
{
	char *dest;
	size_t len;

	len = strlen(str) + 1;
	dest = alloc(len, sizeof(char), NT_RSTRING_O);

	strncpy(dest, str, len);

	dest[len] = '\0';

	static const vtable_t vt = {.__clone__ = rstring_clone};

	object_reg_dunders(dest, &vt);

	return dest;
}

char *rstring_concat(char *dest, int n, ...)
{
	va_list args;
	va_start(args, n);
	SMART char *smt_dest = dest;

	int dest_len = object_count(dest);

	while (n > 0)
	{
		SMART char *src = va_arg(args, char *);
		int src_len = object_count(src) - 1;
		int dest_len2 = dest_len;

		dest_len2 += src_len;

		smt_dest = ralloc(smt_dest, dest_len2, sizeof(char));
		strncpy(smt_dest + dest_len - 1, src, src_len);

		dest_len = dest_len2;

		n--;
	}

	va_end(args);

	RETURN(smt_dest);
}

bool rstring_compare(object_o str1, object_o str2)
{
	SMART char *smt_str1 = (char *)str1;
	SMART char *smt_str2 = (char *)str2;
	return strcmp(smt_str1, smt_str2) == 0 ? true : false;
}

char char_at(char *object, int pos)
{
	return object[pos];
}

int rstring_length(char *str)
{
	return object_count(str) - 1;
}