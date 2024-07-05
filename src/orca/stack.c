#include "orca/orca.h"

static array_o *stack_g = NULL;

array_o *stack_get()
{
	if (stack_g == NULL)
	{
		stack_g = new_array(0);
		pool_globals_add(stack_g);
	}

	return stack_g;
}

void stack_push(object_o value)
{
	array_o *stack = stack_get();
	array_push(stack, value);
}

void stack_print()
{
	print(stack_get());
}