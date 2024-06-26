#include "utils/bool/bool.h"

const char *bool_string(bool bl)
{
	const char *bool_str = bl ? "true" : "false";

	return bool_str;
}