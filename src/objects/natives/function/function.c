#include "objects/print/print.h"
#include "objects/natives/function/function.h"

struct function_o
{
	const char *name;
	fun_generic_t function;
};

object_o function_str(object_o object)
{
	function_o *function = (function_o *)object;
	return string_from("\"(function: %s)\"", function->name);
}

/*
 Accepts a function void pointer
*/
function_o *function(const char *name, fun_generic_t fun_generic)
{
	function_o *function;
	function = alloc(1, sizeof(function_o), NT_FUNCTION_O);

	function->name = name;
	function->function = fun_generic;

	static const vtable_t vt = {
		.__str__ = function_str};

	object_reg_dunders(function, &vt);

	return function;
}

const char *function_name(function_o *function)
{
	return function->name;
}

fun_generic_t function_fun(function_o *function)
{
	return function->function;
}