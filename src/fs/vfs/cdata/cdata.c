#include "fs/vfs/vfs.h"

void cdo_visitor(object_o object, fun_process_t fun_process)
{
	cached_data_o *cdo = (cached_data_o *)object;
	post_visit(cdo->object, fun_process);
}

void cdo_print(object_o object, int depth)
{
	cached_data_o *cdo = (cached_data_o *)object;

	char space[depth];
	memset(space, ' ', depth);
	space[depth] = '\0';

	printf(YELLOW "<cached_data>");
	printf(RESET ": {\n%sdirty:", space);
	printf(GREEN " %s" RESET ",\n %sdata: ", bool_string(cdo_get_dirty(cdo)), space);
	// print_offset(cdo->object, depth++);
	printf("%s}", space);
}

cached_data_o *create_cached_object(object_o object, bool dirty)
{
	SMART cached_data_o *cdo = alloc(1, sizeof(cached_data_o), CACHED_DATA_O);
	cdo->__is_dirty__ = dirty;
	cdo->object = object;

	static const vtable_t vt = {
		.__print__ = cdo_print,
		.__visitor__ = cdo_visitor};

	object_reg_dunders(cdo, &vt);

	RETURN(cdo);
}

bool cdo_get_dirty(cached_data_o *cdo)
{
	return cdo->__is_dirty__;
}

void cdo_set_dirty(cached_data_o *cdo)
{
	cdo->__is_dirty__ = false;
}
