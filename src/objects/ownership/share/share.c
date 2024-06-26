#include "objects/share/share.h"

void __share__(object_header_t *object_header, object_o object)
{
	retain(object);
}

/*
	Share ownership
*/
object_o share(object_o object)
{
	if (object == NULL)
	{
		printf("fun share(): Object is null\n");
		exit(1);
	}

	post_visit(object, __share__);
	return object;
}