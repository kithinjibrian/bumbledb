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

typedef object_o (*fun_builtin_t)(array_o *);
typedef void (*fun_evaluate_t)(ast_node_o *, frame_o *);

typedef struct types_t
{
	char *name;
	data_type_e data_type;
} types_t;

typedef struct builtin_t
{
	char *name;
	fun_builtin_t fun;
} builtin_t;

htable_o *eval(string_o *prog);
htable_o *orca(string_o *filename);
ast_node_o *orca_parse(array_o *tokens);
htable_o *orca_evaluate(ast_node_o *ast_node);

array_o *parse_errors_get();
object_o builtin(char *name, array_o *args);

ast_node_o *orca_parse2(array_o *tokens);

void ast_evaluate(ast_node_o *ast_node, frame_o *frame);
void __orca_evaluate__(ast_node_o *ast_node, frame_o *frame);

void type_check(ast_node_o *ast_node, frame_o *frame);

frame_o *new_frame(frame_o *parent);
object_o frame_get(frame_o *frame, object_o key);
void frame_set(frame_o *frame, object_o key, object_o value);

void stack_print();
array_o *stack_get();
void stack_push(object_o value);

#endif