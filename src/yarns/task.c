#include "yarns/yarns.h"

void task_visitor(object_o object, fun_process_t fun_process)
{
	task_o *task = (task_o *)object;
	post_visit(task->args, fun_process);
	post_visit(task->function, fun_process);
}

task_o *create_task(function_o *function, object_o args)
{
	task_o *task;

	task = alloc(1, sizeof(task_o), TASK_O);

	if (task == NULL)
		return NULL;

	task->args = args;
	task->function = function;

	static const vtable_t vt = {
		.__visitor__ = task_visitor};

	object_reg_dunders(task, &vt);

	return task;
}