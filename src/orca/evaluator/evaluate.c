#include "orca/orca.h"

void __orca_evaluate__(ast_node_o *ast_node, frame_o *frame)
{
	if (frame->return_flag)
	{
		return;
	}

	if (ast_node == NULL)
	{
		return;
	}

	ast_evaluate(ast_node, frame);
}

htable_o *orca_evaluate(ast_node_o *ast_node)
{
	SMART frame_o *global_frame = new_frame(NULL);

	frame_set(global_frame, string_from("export"), new_htable(0));

	__orca_evaluate__(ast_node, global_frame);

	return share(frame_get(global_frame, string_from("export")));
}