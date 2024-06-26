#include "fs/vfs/vfs.h"

array_o *path_split(const char *path)
{
	char *token;
	array_o *path_arr = NULL;
	SMART string_o *path_dup = NULL;

	path_arr = new_array(0);
	path_dup = string_from(path);

	if (path[0] == '/')
	{
		array_push(path_arr, string_from("/"));
	}

	token = strtok(cstring(path_dup), "/");
	while (token != NULL)
	{
		array_push(path_arr, string_from(token));
		token = strtok(NULL, "/");
	}

	return path_arr;
}