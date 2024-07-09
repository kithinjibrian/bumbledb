#include "objects/natives/file.h"

void deinit_file(object_o object)
{
	file_o *file = (file_o *)object;
	if (file->file)
	{
		fclose(file->file);
	}
}

file_o *f_open(const char *filename, const char *modes)
{
	SMART file_o *file_obj = alloc(1, sizeof(file_o), NT_FILE_O);
	file_obj->file = NULL;

	static const vtable_t vt = {.__deinit__ = deinit_file};

	object_reg_dunders(file_obj, &vt);

	if ((file_obj->file = fopen(filename, modes)))
	{
		fseek(file_obj->file, 0, SEEK_END);
		file_obj->size = ftell(file_obj->file);
		fseek(file_obj->file, 0, SEEK_SET);

		RETURN(file_obj);
	}

	return error("fun f_open(): error opening file", LOG);
}

FILE *f_get(file_o *file)
{
	return file->file;
}