#include "utils/bitmap/bitmap.h"

void bit_set(uint8_t bitmap[], int index)
{
	bitmap[index / 8] |= (1 << (index % 8));
}

int bit_get(uint8_t bitmap[], int index)
{
	return (bitmap[index / 8] >> (index % 8)) & 1;
}

void bit_clear(uint8_t bitmap[], int index)
{
	bitmap[index / 8] &= ~(1 << (index % 8));
}

int bit_search(uint8_t *bitmap, int size, int value)
{
	for (int i = 0; i < size * 8; ++i)
	{
		if (((bitmap[i / 8] >> (i % 8)) & 1) == value)
		{
			return i;
		}
	}
	return -1;
}

void bit_print(uint8_t bitmap[], int size)
{
	for (int i = 0; i < size; ++i)
	{
		printf("(row: %d) = ", i);
		for (int j = 7; j >= 0; --j)
		{
			printf("%d", (bitmap[i] >> j) & 1);
		}
		printf("\n");
	}
	printf("\n");
}