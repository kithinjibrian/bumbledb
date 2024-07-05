#include "quill/quill.h"

int quill_write(file_o *file, object_o object)
{
	SMART object_o smt_object = object;
	SMART string_o *sraso = str(smt_object);
	SMART object_o obj = load(share(sraso));

	if (obj == NULL)
	{
		return -1;
	}

	int x = string_length(sraso);

	uint8_t buffer[x + sizeof(int)];
	memcpy(buffer, &x, sizeof(int));
	memcpy(buffer + sizeof(int), cstring(sraso), string_length(sraso));

	return fwrite(buffer, 1, sizeof(buffer), f_get(file));
}

array_o *quill_read(file_o *file)
{
	fseek(f_get(file), 0, SEEK_END);
	long size = ftell(f_get(file));
	fseek(f_get(file), 0, SEEK_SET);

	uint8_t b[size];

	fread(b, size, sizeof(char), f_get(file));

	array_o *array = new_array(0);

	int index = 0;

	while (index < size)
	{
		int size = b[index] | (b[index + 1] << 8) | (b[index + 2] << 16) | (b[index + 3] << 24);

		SMART string_o *str = string_from("%.*s", size, b + index + sizeof(int));

		SMART object_o object = load(MOVE(str));
		array_push(array, MOVE(object));
		index += (size + sizeof(int));
	}

	return array;
}