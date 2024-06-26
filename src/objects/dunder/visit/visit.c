#include "objects/visit/visit.h"
#include "objects/natives/natives.h"

void post_visit(object_o object, fun_process_t fun_process)
{
	if (!object)
		return;

	object_header_t *object_header = get_object_header(object);

	if (object_header->vtable && object_header->vtable->__visitor__)
	{
		object_header->vtable->__visitor__(object, fun_process);
	}

	fun_process(object_header, object);
}
