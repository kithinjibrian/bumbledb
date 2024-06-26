#include "objects/print/print.h"
#include "objects/natives/number/number.h"

object_o number_clone(object_o num)
{
	long a = (*(long *)num);

	return (object_o)number(a);
}

bool number_equal(object_o num1, object_o num2)
{
	long a = (*(long *)num1);
	long b = (*(long *)num2);
	return a == b;
}

object_o number_str(object_o object)
{
	int num = (*(int *)object);
	return string_from("%ld", num);
}

long *number(long num)
{
	long *n = alloc(1, sizeof(long), NT_INT_O);

	*n = num;

	static const vtable_t vt = {
		.__str__ = number_str,
		.__clone__ = number_clone,
		.__equals__ = number_equal};

	object_reg_dunders(n, &vt);

	return n;
}