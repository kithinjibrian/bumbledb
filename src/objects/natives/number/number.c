#include "objects/natives/number.h"

object_o number_clone(object_o num)
{
	long a = (*(long *)num);

	return (object_o)number(a);
}

object_o number_str(object_o object)
{
	int num = (*(int *)object);
	return string_from("%ld", num);
}

bool number_equal(object_o num1, object_o num2)
{
	long a = (*(long *)num1);
	long b = (*(long *)num2);
	return a == b;
}

object_o number_is_not_equal(object_o num1, object_o num2)
{
	long a = (*(long *)num1);
	long b = (*(long *)num2);
	return (object_o)number(a != b);
}

object_o number_add(object_o num1, object_o num2)
{
	long a = (*(long *)num1);
	long b = (*(long *)num2);
	return (object_o)number(a + b);
}

object_o number_minus(object_o num1, object_o num2)
{
	long a = (*(long *)num1);
	long b = (*(long *)num2);
	return (object_o)number(a - b);
}

object_o number_mult(object_o num1, object_o num2)
{
	long a = (*(long *)num1);
	long b = (*(long *)num2);
	return (object_o)number(a * b);
}

object_o number_divide(object_o num1, object_o num2)
{
	long a = (*(long *)num1);
	long b = (*(long *)num2);
	return (object_o)number(a / b);
}

object_o number_modulo(object_o num1, object_o num2)
{
	long a = (*(long *)num1);
	long b = (*(long *)num2);
	return (object_o)number(a % b);
}

object_o number_lesser(object_o num1, object_o num2)
{
	long a = (*(long *)num1);
	long b = (*(long *)num2);
	return (object_o)number(a < b);
}

object_o number_lesser_or_equals(object_o num1, object_o num2)
{
	long a = (*(long *)num1);
	long b = (*(long *)num2);
	return (object_o)number(a <= b);
}

object_o number_greater(object_o num1, object_o num2)
{
	long a = (*(long *)num1);
	long b = (*(long *)num2);
	return (object_o)number(a > b);
}

object_o number_greater_or_equals(object_o num1, object_o num2)
{
	long a = (*(long *)num1);
	long b = (*(long *)num2);
	return (object_o)number(a >= b);
}

long *number(long num)
{
	long *n = alloc(1, sizeof(long), NT_INT_O);

	*n = num;

	static const vtable_t vt = {
		.__str__ = number_str,
		.__add__ = number_add,
		.__mult__ = number_mult,
		.__minus__ = number_minus,
		.__clone__ = number_clone,
		.__equals__ = number_equal,
		.__divide__ = number_divide,
		.__modulo__ = number_modulo,
		.__lesser__ = number_lesser,
		.__greater__ = number_greater,
		.__is_not_equals__ = number_is_not_equal,
		.__greater_or_equals__ = number_greater_or_equals,
		.__lesser_or_equals__ = number_lesser_or_equals};

	object_reg_dunders(n, &vt);

	return n;
}