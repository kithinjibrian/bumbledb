#pragma once
#ifndef ORCA_H
#define ORCA_H

#include "objects/object.h"

typedef struct frame_o
{
	array_o *stack;
	htable_o *stable;
	bool break_flag;
	bool return_flag;
	bool continue_flag;
	object_o return_value;
	struct frame_o *prev;
	struct frame_o *parent;
} frame_o;

htable_o *orca(string_o *filename);
ast_node_o *orca_parse(array_o *tokens);
htable_o *orca_evaluate(ast_node_o *ast_node);

frame_o *new_frame(frame_o *parent);
object_o frame_get(frame_o *frame, object_o key);
void frame_set(frame_o *frame, object_o key, object_o value);

void stack_print();
array_o *stack_get();
void stack_push(object_o value);

#endif