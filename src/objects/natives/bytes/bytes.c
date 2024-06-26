#include "objects/print/print.h"
#include "objects/natives/bytes/bytes.h"

void bytes_print(object_o object, int depth)
{
	uint8_t *bytes = (object_o)object;
	printf(GREY "[%d]b\'", object_refcount(bytes));
	for (int a = 0; a < object_count(bytes); a++)
	{
		if (bytes[a] == 0)
		{
			printf("00");
		}
		else
		{
			if (isprint(bytes[a]))
			{
				printf(GREEN "%c" GREY, bytes[a]);
			}
			else
			{
				printf(GREEN "%02hhX" GREY, bytes[a]);
			}
		}
	}
	printf("\'" RESET);
}

uint8_t *bytes_from(void *buffer, int length)
{
	uint8_t *dest;

	dest = alloc(length, sizeof(uint8_t), NT_BYTES_O);

	memcpy(dest, buffer, length);

	static const vtable_t vt = {
		.__print__ = bytes_print};

	object_reg_dunders(dest, &vt);

	return dest;
}

uint8_t *new_bytes(int length)
{
	uint8_t *dest;

	dest = alloc(length, sizeof(uint8_t), NT_BYTES_O);

	memset(dest, 0, length);

	static const vtable_t vt = {
		.__print__ = bytes_print};

	object_reg_dunders(dest, &vt);

	return dest;
}

char *bytes_to_hex(uint8_t *bytes, int length)
{
	int new_len = (length * 2) + 1;
	char *new_bytes = alloc(new_len, sizeof(char), NT_BYTES_O);

	for (int i = 0; i < length; i++)
	{
		sprintf(&new_bytes[i * 2], "%02x", bytes[i]);
	}

	new_bytes[new_len] = '\0';

	return new_bytes;
}

char *bytes_from_hex(uint8_t *hex, int length)
{
	// unsigned char *bytes = alloc(length / 2, sizeof(char), NT_BYTES_O);

	// for (int i = 0; i < (length / 2); i++)
	// {
	// 	sscanf(hex + i * 2, "%02hhx", &bytes[i]);
	// }

	return NULL;
}