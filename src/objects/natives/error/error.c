#include "objects/natives/error.h"

#define ERROR_LOG_FILE "error.log"

struct error_o
{
	int type;
	const char *error;
};

void __log_error__(error_o *error)
{
	char *time_string;
	time_t current_time;

	FILE *file = fopen(ERROR_LOG_FILE, "ab+");

	if (file == NULL)
		return;

	current_time = time(NULL);
	time_string = ctime(&current_time);
	const char *sep = "\n------------------------\n";

	fwrite(time_string, sizeof(char), strlen(time_string), file);
	fwrite(error->error, sizeof(char), strlen(error->error), file);
	fwrite(sep, sizeof(char), strlen(sep), file);

	fclose(file);
}

void error_print(object_o object, int depth)
{
	error_o *error = (error_o *)object;

	char space[depth];
	memset(space, ' ', depth);
	space[depth] = '\0';

	printf(YELLOW "(error)" RESET ": {\n %smessage: ", space);
	printf(RED "\"%s\"" RESET, error->error);
	printf("\n%s}", space);
}

object_o error(const char *err, int type)
{
	error_o *error;

	error = alloc(1, sizeof(error_o), NT_ERROR_O);

	error->error = err;
	error->type = type;

	static const vtable_t vt = {
		.__print__ = error_print};

	object_reg_dunders(error, &vt);

	if (type & PRINT)
	{
		printf("ERROR! %s\n", err);
	}

	if (type & LOG)
	{
		__log_error__(error);
	}

	if (type & PANIC)
	{
		exit(1);
	}

	return (object_o)error;
}

int error_type(error_o *error)
{
	return error->type;
}

const char *error_get_error(error_o *error)
{
	return error->error;
}

bool is_error(object_o object)
{
	if (object_typeof(object) == NT_ERROR_O)
	{
		return true;
	}
	return false;
}