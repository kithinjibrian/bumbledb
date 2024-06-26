#include "objects/state/state.h"

/*
 Check for error, most objects don't have this method defined
*/
object_o get_state(object_o object)
{
	object_header_t *object_header = get_object_header(object);

	if (object_header->vtable && object_header->vtable->__get_state__)
	{
		return object_header->vtable->__get_state__(object);
	}

	return error("fun get_state(): __get_state__ dunder method is missing!", SILENT);
}

object_o set_state(object_o object, object_o state)
{
	object_header_t *object_header = get_object_header(object);

	if (object_header->vtable && object_header->vtable->__set_state__)
	{
		return object_header->vtable->__set_state__(object, state);
	}

	return error("fun set_state(): __set_state__ dunder method is missing!", SILENT);
}