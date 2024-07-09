#include "orca/orca.h"

object_o __builtin_print__(array_o *args)
{
	print(array_at(args, 0));
	return NULL;
}

object_o __builtin_open__(array_o *args)
{
	file_o *file = f_open(cstring(array_at(args, 0)), cstring(array_at(args, 1)));
	return file;
}

object_o __builtin_read__(array_o *args)
{
	file_o *file = (file_o *)array_at(args, 0);
	uint8_t *buffer = new_bytes(file->size);
	fread(buffer, file->size, 1, f_get(file));
	return buffer;
}

object_o __builtin_write__(array_o *args)
{
	file_o *file = (file_o *)array_at(args, 0);

	string_o *buffer = (string_o *)array_at(args, 1);

	fwrite(buffer->str, buffer->length - 1, 1, f_get(file));

	return NULL;
}

object_o __builtin_seek__(array_o *args)
{
	file_o *file = (file_o *)array_at(args, 0);
	long position = *(long *)array_at(args, 1);
	long whence = *(long *)array_at(args, 2);

	fseek(f_get(file), position, whence);

	return NULL;
}

object_o __builtin_str__(array_o *args)
{
	object_o object = array_at(args, 0);
	return str(object);
}

object_o __builtin_load__(array_o *args)
{
	object_o object = array_at(args, 0);
	return load(share(object));
}

object_o __builtin_orca__(array_o *args)
{
	object_o object = array_at(args, 0);
	return eval(share(object));
}

object_o __builtin_retain__(array_o *args)
{
	object_o object = array_at(args, 0);
	share(object);
	return NULL;
}

object_o __builtin_release__(array_o *args)
{
	object_o object = array_at(args, 0);

	ast_node_o *ast_node = *(object_o *)object;

	if (object_typeof(ast_node) == AST_NODE_O)
	{
		if (ast_node->node_type == NODE_VARIABLE)
		{
			DROP(ast_node->variable.value);
			ast_node->variable.value = null();
		}
	}

	return NULL;
}

object_o __builtin_info__(array_o *args)
{
	object_o object = array_at(args, 0);
	object_info(object);
	return NULL;
}

object_o builtin(char *name, array_o *args)
{
	static builtin_t builtin[] = {
		{"print", __builtin_print__},
		{"open", __builtin_open__},
		{"read", __builtin_read__},
		{"write", __builtin_write__},
		{"str", __builtin_str__},
		{"seek", __builtin_seek__},
		{"load", __builtin_load__},
		{"orca", __builtin_orca__},
		{"retain", __builtin_retain__},
		{"release", __builtin_release__},
		{"info", __builtin_info__},
		{NULL, NULL}};

	int i = 0;
	while (builtin[i].name != NULL)
	{
		if (strcmp(builtin[i].name, name) == 0)
		{
			return builtin[i].fun(args);
		}
		i++;
	}

	return NULL;
}