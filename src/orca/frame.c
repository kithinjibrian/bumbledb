#include "orca/orca.h"

object_o frame_str(object_o object)
{
	frame_o *frame = (frame_o *)object;
	SMART string_o *arr = str(frame->stack);
	SMART string_o *ht = str(frame->stable);
	SMART string_o *parent = str(frame->parent);
	SMART string_o *return_value = NULL;

	if (frame->return_flag)
	{
		if (frame->return_value == NULL)
		{
			return_value = string_format("<empty>");
		}
		else
		{
			return_value = str(frame->return_value);
		}
	}

	return string_format("{stack:%q, symtable:%q, ret_val:%q}", arr, ht, return_value, parent);
}

void frame_visitor(object_o object, fun_process_t fun_process)
{
	frame_o *frame = (frame_o *)object;
	post_visit(frame->stack, fun_process);
	post_visit(frame->stable, fun_process);
	if (frame->return_value)
	{
		post_visit(frame->return_value, fun_process);
	}
}

frame_o *new_frame(frame_o *parent)
{
	frame_o *frame = alloc(1, sizeof(frame_o), FRAME_O);
	frame->stack = new_array(0);
	frame->stable = new_htable(10);
	frame->return_flag = false;
	frame->return_value = NULL;
	frame->parent = parent;
	frame->prev = NULL;

	static const vtable_t vt = {
		.__str__ = frame_str,
		.__visitor__ = frame_visitor};

	object_reg_dunders(frame, &vt);

	return frame;
}

void frame_set(frame_o *frame, object_o key, object_o value)
{
	htable_set(frame->stable, key, value);
}

object_o frame_get(frame_o *frame, object_o key)
{
	return htable_get(frame->stable, key);
}
