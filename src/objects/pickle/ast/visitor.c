#include "objects/pickle/pickle.h"

void v_string(ast_node_o *node, fun_process_t fun_process)
{
	post_visit(node->string, fun_process);
}

void v_word(ast_node_o *node, fun_process_t fun_process)
{
	post_visit(node->word, fun_process);
}

void v_null(ast_node_o *node, fun_process_t fun_process)
{
	;
}

void v_number(ast_node_o *node, fun_process_t fun_process)
{
	post_visit(node->number, fun_process);
}

void v_return_smt(ast_node_o *node, fun_process_t fun_process)
{
	post_visit(node->return_smt.exp, fun_process);
}

void v_import_smt(ast_node_o *node, fun_process_t fun_process)
{
	post_visit(node->import_smt.path, fun_process);
}

void v_unary_exp(ast_node_o *node, fun_process_t fun_process)
{
	post_visit(node->unary_exp.operand, fun_process);
}

void v_key_value(ast_node_o *ast_node, fun_process_t fun_process)
{
	post_visit(ast_node->key_value.key, fun_process);
	post_visit(ast_node->key_value.value, fun_process);
}

void v_binary_exp(ast_node_o *ast_node, fun_process_t fun_process)
{
	post_visit(ast_node->binary_exp.left, fun_process);
	post_visit(ast_node->binary_exp.right, fun_process);
}

void v_reference(ast_node_o *ast_node, fun_process_t fun_process)
{
	post_visit(ast_node->word, fun_process);
}

void v_dereference(ast_node_o *ast_node, fun_process_t fun_process)
{
	post_visit(ast_node->word, fun_process);
}

void v_variable_dec(ast_node_o *ast_node, fun_process_t fun_process)
{
	post_visit(ast_node->variable.type, fun_process);
	post_visit(ast_node->variable.value, fun_process);
	post_visit(ast_node->variable.identifier, fun_process);
	post_visit(ast_node->variable.initializer, fun_process);
}

void v_parameter_dec(ast_node_o *ast_node, fun_process_t fun_process)
{
	post_visit(ast_node->parameter_dec.type, fun_process);
	post_visit(ast_node->parameter_dec.identifier, fun_process);
	post_visit(ast_node->parameter_dec.initializer, fun_process);
}

void v_function_dec(ast_node_o *ast_node, fun_process_t fun_process)
{
	post_visit(ast_node->function_dec.body, fun_process);
	post_visit(ast_node->function_dec.identifier, fun_process);
	post_visit(ast_node->function_dec.return_type, fun_process);
	ast_node_o *current = ast_node->function_dec.parameters;
	while (current)
	{
		post_visit(current, fun_process);
		current = current->next;
	}
}

void v_function_call(ast_node_o *ast_node, fun_process_t fun_process)
{
	ast_node_o *identifier = ast_node->function_call.identifier;
	ast_node_o *current = ast_node->function_call.arguments;
	ast_node_o *call_exp = ast_node->function_call.call_exp;

	while (identifier)
	{
		post_visit(identifier, fun_process);
		identifier = identifier->next;
	}

	while (current)
	{
		post_visit(current, fun_process);
		current = current->next;
	}

	while (call_exp)
	{
		post_visit(call_exp, fun_process);
		call_exp = call_exp->next;
	}
}

void v_object(ast_node_o *ast_node, fun_process_t fun_process)
{
	ast_node_o *next = NULL;
	ast_node_o *current = ast_node->object.members;
	while (current)
	{
		next = current->next;
		post_visit(current, fun_process);
		current = next;
	}
}

void v_super_elements(ast_node_o *ast_node, fun_process_t fun_process)
{
	ast_node_o *next = NULL;
	ast_node_o *current = ast_node->super_statement.statement;
	while (current)
	{
		next = current->next;
		post_visit(current, fun_process);
		current = next;
	}
}

void v_array(ast_node_o *ast_node, fun_process_t fun_process)
{
	ast_node_o *next = NULL;
	ast_node_o *current = ast_node->array.elements;
	while (current)
	{
		next = current->next;
		post_visit(current, fun_process);
		current = next;
	}
}

void v_list(ast_node_o *ast_node, fun_process_t fun_process)
{
	ast_node_o *next = NULL;
	ast_node_o *current = ast_node->list.head;
	while (current)
	{
		next = current->next;
		post_visit(current, fun_process);
		current = next;
	}
}

void v_nt_function_call(ast_node_o *ast_node, fun_process_t fun_process)
{
	post_visit(ast_node->nt_function_call.identifier, fun_process);
	ast_node_o *current = ast_node->nt_function_call.arguments;

	while (current)
	{
		post_visit(current, fun_process);
		current = current->next;
	}
}

void v_break(ast_node_o *ast_node, fun_process_t fun_process)
{
}

void v_continue(ast_node_o *ast_node, fun_process_t fun_process)
{
}

void v_if_statement(ast_node_o *ast_node, fun_process_t fun_process)
{
	post_visit(ast_node->ternary_exp.condition, fun_process);
	post_visit(ast_node->ternary_exp.if_body, fun_process);
	post_visit(ast_node->ternary_exp.else_body, fun_process);
}

void v_while_statement(ast_node_o *ast_node, fun_process_t fun_process)
{
	post_visit(ast_node->while_statement.body, fun_process);
	post_visit(ast_node->while_statement.condition, fun_process);
}

void v_for_statement(ast_node_o *ast_node, fun_process_t fun_process)
{
	post_visit(ast_node->for_statement.body, fun_process);
	post_visit(ast_node->for_statement.init, fun_process);
	post_visit(ast_node->for_statement.condition, fun_process);
	post_visit(ast_node->for_statement.update, fun_process);
}

void v_struct(ast_node_o *ast_node, fun_process_t fun_process)
{
	ast_node_o *body = ast_node->struct_statement.body;
	while (body)
	{
		post_visit(body, fun_process);
		body = body->next;
	}
	post_visit(ast_node->struct_statement.identifier, fun_process);
}

void v_struct_instance(ast_node_o *ast_node, fun_process_t fun_process)
{
	ast_node_o *next = NULL;
	ast_node_o *current = ast_node->struct_instance.fields;
	while (current)
	{
		next = current->next;
		post_visit(current, fun_process);
		current = next;
	}
	post_visit(ast_node->struct_instance.identifier, fun_process);
}

static htable_o *v_table_g = NULL;

void ast_visit(ast_node_o *ast_node, fun_process_t fun_process)
{

	if (v_table_g == NULL)
	{
		v_table_g = new_htable(20);
		htable_set(v_table_g, number(NODE_LIST), function("list", (fun_generic_t)v_list));
		htable_set(v_table_g, number(NODE_NULL), function("null", (fun_generic_t)v_null));
		htable_set(v_table_g, number(NODE_WORD), function("word", (fun_generic_t)v_word));
		htable_set(v_table_g, number(NODE_ARRAY), function("array", (fun_generic_t)v_array));
		htable_set(v_table_g, number(NODE_BREAK), function("break", (fun_generic_t)v_break));
		htable_set(v_table_g, number(NODE_NUMBER), function("number", (fun_generic_t)v_number));
		htable_set(v_table_g, number(NODE_STRUCT), function("struct", (fun_generic_t)v_struct));
		htable_set(v_table_g, number(NODE_STRING), function("string", (fun_generic_t)v_string));
		htable_set(v_table_g, number(NODE_OBJECT), function("object", (fun_generic_t)v_object));
		htable_set(v_table_g, number(NODE_KEY_VALUE), function("number", (fun_generic_t)v_key_value));
		htable_set(v_table_g, number(NODE_CONTINUE), function("continue", (fun_generic_t)v_continue));
		htable_set(v_table_g, number(NODE_IMPORT), function("import_smt", (fun_generic_t)v_import_smt));
		htable_set(v_table_g, number(NODE_RETURN), function("return_smt", (fun_generic_t)v_return_smt));
		htable_set(v_table_g, number(NODE_UNARY_EXP), function("unary_exp", (fun_generic_t)v_unary_exp));
		htable_set(v_table_g, number(NODE_BINARY_EXP), function("binary_exp", (fun_generic_t)v_binary_exp));
		htable_set(v_table_g, number(NODE_VARIABLE), function("variable_dec", (fun_generic_t)v_variable_dec));
		htable_set(v_table_g, number(NODE_TERNARY_EXP), function("if_statement", (fun_generic_t)v_if_statement));
		htable_set(v_table_g, number(NODE_FUNCTION_DEC), function("function_dec", (fun_generic_t)v_function_dec));
		htable_set(v_table_g, number(NODE_REFERENCE), function("reference", (fun_generic_t)v_reference));
		htable_set(v_table_g, number(NODE_DEREFERENCE), function("reference", (fun_generic_t)v_dereference));
		htable_set(v_table_g, number(NODE_PARAMETER_DEC), function("parameter_dec", (fun_generic_t)v_parameter_dec));
		htable_set(v_table_g, number(NODE_FUNCTION_CALL), function("function_call", (fun_generic_t)v_function_call));
		htable_set(v_table_g, number(NODE_FOR_STATEMENT), function("for_statement", (fun_generic_t)v_for_statement));
		htable_set(v_table_g, number(NODE_WHILE_STATEMENT), function("while_statement", (fun_generic_t)v_while_statement));
		htable_set(v_table_g, number(NODE_SUPER_STATEMENT), function("super_statement", (fun_generic_t)v_super_elements));
		htable_set(v_table_g, number(NODE_NT_FUNCTION), function("nt_function_call", (fun_generic_t)v_nt_function_call));
		htable_set(v_table_g, number(NODE_STRUCT_INSTANCE), function("struct_instance", (fun_generic_t)v_struct_instance));
	}

	if (ast_node == NULL)
		return;

	function_o *function = (function_o *)htable_get(v_table_g, number(ast_node->node_type));

	if (function == NULL)
	{
		error(cstring(string_format("Vis unknown node type: %d", ast_node->node_type)), PRINT | PANIC);
	}

	fun_ast_visit fun = (fun_ast_visit)function->function;

	fun(ast_node, fun_process);
}

void drop_ast_table()
{
	DROP(v_table_g);
}