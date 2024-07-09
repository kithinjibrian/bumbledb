#include "orca/orca.h"

void type_for(ast_node_o *ast_node, frame_o *frame) {}
void type_word(ast_node_o *ast_node, frame_o *frame) {}
void type_list(ast_node_o *ast_node, frame_o *frame) {}
void type_break(ast_node_o *ast_node, frame_o *frame) {}
void type_array(ast_node_o *ast_node, frame_o *frame) {}
void type_while(ast_node_o *ast_node, frame_o *frame) {}
void type_block(ast_node_o *ast_node, frame_o *frame) {}
void type_htable(ast_node_o *ast_node, frame_o *frame) {}
void type_import(ast_node_o *ast_node, frame_o *frame) {}
void type_struct(ast_node_o *ast_node, frame_o *frame) {}

void type_number(ast_node_o *ast_node, frame_o *frame)
{
	array_push(frame, str(ast_node->number));
}

void type_string(ast_node_o *ast_node, frame_o *frame)
{
	array_push(frame, string_format("string_from(\"%q\")", ast_node->string));
}

void type_binary(ast_node_o *ast_node, frame_o *frame)
{
	type_check(ast_node->binary_exp.left, frame);

	if (ast_node->binary_exp.token_type == TOKEN_PLUS)
	{
		array_push(frame, string_from(" + "));
	}
	else if (ast_node->binary_exp.token_type == TOKEN_MINUS)
	{
		array_push(frame, string_from(" - "));
	}
	else if (ast_node->binary_exp.token_type == TOKEN_MULT)
	{
		array_push(frame, string_from(" * "));
	}
	else if (ast_node->binary_exp.token_type == TOKEN_DIVIDE)
	{
		array_push(frame, string_from(" / "));
	}

	type_check(ast_node->binary_exp.right, frame);
}

void type_return(ast_node_o *ast_node, frame_o *frame) {}
void type_native(ast_node_o *ast_node, frame_o *frame) {}
void type_ternary(ast_node_o *ast_node, frame_o *frame) {}

void type_program(ast_node_o *ast_node, frame_o *frame)
{
	ast_node_o *expression = ast_node->super_statement.statement;
	while (expression)
	{
		type_check(expression, frame);
		expression = expression->next;
	}
}

void type_variable(ast_node_o *ast_node, frame_o *frame)
{
	ast_node_o *var = ast_node->super_statement.statement;
	if (equal(var->variable.type->word, string_from("charp")))
	{
		array_push(frame, string_from("char *"));
	}
	else
	{
		array_push(frame, share(var->variable.type->word));
		array_push(frame, string_from(" "));
	}
	array_push(frame, share(var->variable.identifier->word));

	while (var)
	{
		if (var->variable.initializer)
		{
			array_push(frame, string_from(" = "));
			type_check(var->variable.initializer, frame);
		}

		var = var->next;
	}

	array_push(frame, string_from(";\n"));
}

void type_continue(ast_node_o *ast_node, frame_o *frame) {}

void type_expression(ast_node_o *ast_node, frame_o *frame)
{
	ast_node_o *expression = ast_node->super_statement.statement;
	while (expression)
	{
		type_check(expression, frame);
		expression = expression->next;
	}
}

void type_left_hand(ast_node_o *ast_node, frame_o *frame)
{
	ast_node_o *left = ast_node->super_statement.statement;

	if (left->next)
	{
		print(left);
	}
	else
	{
		type_check(left, frame);
	}
}

void type_function_call(ast_node_o *ast_node, frame_o *frame) {}
void type_struct_instance(ast_node_o *ast_node, frame_o *frame) {}
void type_function_expression(ast_node_o *ast_node, frame_o *frame) {}

static htable_o *type_table_g = NULL;

void type_check(ast_node_o *ast_node, frame_o *frame)
{
	if (type_table_g == NULL)
	{
		type_table_g = new_htable(30);
		pool_globals_add(type_table_g);
		htable_set(type_table_g, number(NODE_WORD), function("word", (fun_generic_t)type_word));
		htable_set(type_table_g, number(NODE_LIST), function("list", (fun_generic_t)type_list));
		htable_set(type_table_g, number(NODE_BREAK), function("break", (fun_generic_t)type_break));
		htable_set(type_table_g, number(NODE_ARRAY), function("array", (fun_generic_t)type_array));
		htable_set(type_table_g, number(NODE_OBJECT), function("htable", (fun_generic_t)type_htable));
		htable_set(type_table_g, number(NODE_IMPORT), function("import", (fun_generic_t)type_import));
		htable_set(type_table_g, number(NODE_STRUCT), function("struct", (fun_generic_t)type_struct));
		htable_set(type_table_g, number(NODE_NUMBER), function("number", (fun_generic_t)type_number));
		htable_set(type_table_g, number(NODE_STRING), function("string", (fun_generic_t)type_string));
		htable_set(type_table_g, number(NODE_FOR_STATEMENT), function("for", (fun_generic_t)type_for));
		htable_set(type_table_g, number(STATEMENT_BLOCK), function("block", (fun_generic_t)type_block));
		htable_set(type_table_g, number(NODE_BINARY_EXP), function("binary", (fun_generic_t)type_binary));
		htable_set(type_table_g, number(NODE_RETURN), function("return_smt", (fun_generic_t)type_return));
		htable_set(type_table_g, number(NODE_CONTINUE), function("continue", (fun_generic_t)type_continue));
		htable_set(type_table_g, number(NODE_WHILE_STATEMENT), function("while", (fun_generic_t)type_while));
		htable_set(type_table_g, number(NODE_TERNARY_EXP), function("ternary", (fun_generic_t)type_ternary));
		htable_set(type_table_g, number(STATEMENT_PROGRAM), function("program", (fun_generic_t)type_program));
		htable_set(type_table_g, number(NODE_NT_FUNCTION), function("string", (fun_generic_t)type_native));
		htable_set(type_table_g, number(STATEMENT_VARIABLE), function("variable", (fun_generic_t)type_variable));
		htable_set(type_table_g, number(STATEMENT_EXPRESSION), function("exepression", (fun_generic_t)type_expression));
		htable_set(type_table_g, number(NODE_FUNCTION_DEC), function("function", (fun_generic_t)type_function_expression));
		htable_set(type_table_g, number(NODE_FUNCTION_CALL), function("function call", (fun_generic_t)type_function_call));
		htable_set(type_table_g, number(NODE_STRUCT_INSTANCE), function("struct instance", (fun_generic_t)type_struct_instance));
		htable_set(type_table_g, number(LIST_LEFT_HAND), function("left hand", (fun_generic_t)type_left_hand));
	}

	if (ast_node == NULL)
		return;

	function_o *function = NULL;

	if (ast_node->node_type == NODE_SUPER_STATEMENT)
	{
		printf("SUPER: %d\n", ast_node->super_statement.type);
		function = (function_o *)htable_get(type_table_g, number(ast_node->super_statement.type));
	}
	else
	{
		printf("NON SUPER: %d\n", ast_node->node_type);
		function = (function_o *)htable_get(type_table_g, number(ast_node->node_type));
	}

	print(function);

	if (function == NULL)
	{
		error("type unknown node type", PRINT | PANIC);
	}

	fun_evaluate_t fun = (fun_evaluate_t)function->function;

	fun(ast_node, frame);
}