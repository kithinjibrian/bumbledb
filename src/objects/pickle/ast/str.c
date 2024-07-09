#include "objects/pickle/pickle.h"

object_o s_number(ast_node_o *ast_node)
{
	return str(ast_node->number);
}

object_o s_string(ast_node_o *ast_node)
{
	return str(ast_node->string);
}

object_o s_word(ast_node_o *ast_node)
{
	return str(ast_node->word);
}

object_o s_null(ast_node_o *ast_node)
{
	return string_from("(null)");
}

object_o s_key_value(ast_node_o *ast_node)
{
	SMART object_o key = ast_node_str(ast_node->key_value.key);
	SMART object_o value = ast_node_str(ast_node->key_value.value);
	return string_format("%q: %q", key, value);
}

object_o s_super_elements(ast_node_o *ast_node)
{
	ast_node_o *current = ast_node->super_statement.statement;
	SMART array_o *arr = new_array(0);
	while (current)
	{
		array_push(arr, ast_node_str(current));
		current = current->next;
	}

	string_o *joined = array_join(arr, ", ");
	return joined;
}

object_o s_object(ast_node_o *ast_node)
{
	ast_node_o *current = ast_node->object.members;
	SMART array_o *arr = new_array(0);
	while (current)
	{
		array_push(arr, ast_node_str(current));
		current = current->next;
	}
	SMART string_o *joined = array_join(arr, ", ");
	return string_format("{%q}", joined);
}

object_o s_array(ast_node_o *ast_node)
{
	ast_node_o *current = ast_node->array.elements;
	SMART array_o *arr = new_array(0);
	while (current)
	{
		array_push(arr, ast_node_str(current));
		current = current->next;
	}
	SMART string_o *joined = array_join(arr, ", ");
	return string_format("[%q]", joined);
}

object_o s_list(ast_node_o *ast_node)
{
	ast_node_o *current = ast_node->list.head;
	SMART array_o *arr = new_array(0);
	while (current)
	{
		array_push(arr, ast_node_str(current));
		current = current->next;
	}
	SMART string_o *joined = array_join(arr, ", ");
	return string_format("(%q)", joined);
}

object_o s_binary_exp(ast_node_o *ast_node)
{
	SMART object_o left = ast_node_str(ast_node->binary_exp.left);
	SMART object_o right = ast_node_str(ast_node->binary_exp.right);
	SMART string_o *op = string_from("%d", ast_node->binary_exp.token_type);

	return string_format("%q <%q> %q", left, op, right);
}

object_o s_parameter_dec(ast_node_o *ast_node)
{
	SMART object_o type = ast_node_str(ast_node->parameter_dec.type);
	SMART object_o init = ast_node_str(ast_node->parameter_dec.initializer);
	SMART object_o identifier = ast_node_str(ast_node->parameter_dec.identifier);

	return string_format("%q: %q", identifier, type, init);
}

object_o s_reference(ast_node_o *ast_node)
{
	SMART object_o var = str(ast_node->word);
	return string_format("&%q", var);
}

object_o s_dereference(ast_node_o *ast_node)
{
	SMART object_o var = str(ast_node->word);
	return string_format("*%q", var);
}

object_o s_variable_dec(ast_node_o *ast_node)
{
	SMART object_o type = ast_node_str(ast_node->variable.type);
	SMART object_o init = ast_node_str(ast_node->variable.initializer);
	SMART object_o identifier = ast_node_str(ast_node->variable.identifier);
	SMART object_o value = str(ast_node->variable.value);

	return string_format("let %q = %q | %q", identifier, value, init);
}

object_o s_return_smt(ast_node_o *ast_node)
{
	SMART object_o exp = ast_node_str(ast_node->return_smt.exp);
	return string_format("return %q", exp);
}

object_o s_import_smt(ast_node_o *ast_node)
{
	SMART object_o exp = ast_node_str(ast_node->import_smt.path);
	return string_format("import %q", exp);
}

object_o s_function_call(ast_node_o *ast_node)
{
	ast_node_o *arguments = ast_node->function_call.arguments;
	ast_node_o *identifier = ast_node->function_call.identifier;
	ast_node_o *call_exp = ast_node->function_call.call_exp;

	SMART array_o *arr = new_array(0);
	SMART array_o *arr2 = new_array(0);
	SMART array_o *arr3 = new_array(0);

	while (identifier)
	{
		array_push(arr, ast_node_str(identifier));
		identifier = identifier->next;
	}

	while (arguments)
	{
		array_push(arr2, ast_node_str(arguments));
		arguments = arguments->next;
	}

	while (call_exp)
	{
		array_push(arr3, ast_node_str(call_exp));
		call_exp = call_exp->next;
	}

	SMART string_o *joined = array_join(arr2, ", ");
	SMART string_o *j_identifier = array_join(arr, ".");
	SMART string_o *j_call_exp = array_join(arr3, ",");

	return string_format("%q(%q)%q", j_identifier, joined, j_call_exp);
}

object_o s_function_dec(ast_node_o *ast_node)
{

	SMART object_o body = ast_node_str(ast_node->function_dec.body);
	SMART object_o identifier = ast_node_str(ast_node->function_dec.identifier);
	SMART object_o return_type = ast_node_str(ast_node->function_dec.return_type);
	SMART array_o *array = new_array(0);

	ast_node_o *parameters = ast_node->function_dec.parameters;

	while (parameters)
	{
		array_push(array, ast_node_str(parameters));
		parameters = parameters->next;
	}

	SMART string_o *joined = array_join(array, ", ");

	return string_format("fun %q (%q) %q ", identifier, joined, body);
}

object_o s_nt_function_call(ast_node_o *ast_node)
{
	SMART object_o identifier = ast_node_str(ast_node->function_call.identifier);
	ast_node_o *arguments = ast_node->function_call.arguments;
	SMART array_o *arr = new_array(0);

	while (arguments)
	{
		array_push(arr, ast_node_str(arguments));
		arguments = arguments->next;
	}

	SMART string_o *joined = array_join(arr, ", ");

	return string_format("%q!(%q)", identifier, joined);
}

object_o s_unary_exp(ast_node_o *ast_node)
{
	SMART object_o iden = ast_node_str(ast_node->unary_exp.operand);
	return string_format("<unary>%q", iden);
}

object_o s_break(ast_node_o *ast_node)
{
	return string_from("break;");
}

object_o s_continue(ast_node_o *ast_node)
{
	return string_from("continue;");
}

object_o s_if_statement(ast_node_o *ast_node)
{
	SMART object_o condition = ast_node_str(ast_node->ternary_exp.condition);
	SMART object_o if_body = ast_node_str(ast_node->ternary_exp.if_body);
	SMART object_o else_body = ast_node_str(ast_node->ternary_exp.else_body);
	return string_format("if (%q) { %q } else { %q }", condition, if_body, else_body);
}

object_o s_while_statement(ast_node_o *ast_node)
{
	SMART object_o body = ast_node_str(ast_node->while_statement.body);
	SMART object_o condition = ast_node_str(ast_node->while_statement.condition);
	return string_format("while(%q) { %q }", condition, body);
}

object_o s_for_statement(ast_node_o *ast_node)
{
	SMART object_o body = ast_node_str(ast_node->for_statement.body);
	SMART object_o init = ast_node_str(ast_node->for_statement.init);
	SMART object_o update = ast_node_str(ast_node->for_statement.update);
	SMART object_o condition = ast_node_str(ast_node->for_statement.condition);
	return string_format("for(%q;%q;%q) { %q }", init, condition, update, body);
}

object_o s_struct(ast_node_o *ast_node)
{
	SMART object_o identifier = ast_node_str(ast_node->struct_statement.identifier);
	ast_node_o *body = ast_node->struct_statement.body;

	SMART array_o *array = new_array(0);
	while (body)
	{
		array_push(array, ast_node_str(body));
		body = body->next;
	}

	SMART string_o *joined = array_join(array, "\n");

	return string_format("struct %q { %q }", identifier, joined);
}

object_o s_struct_instance(ast_node_o *ast_node)
{
	ast_node_o *current = ast_node->struct_instance.fields;
	SMART array_o *arr = new_array(0);
	while (current)
	{
		array_push(arr, ast_node_str(current));
		current = current->next;
	}
	SMART string_o *joined = array_join(arr, "; ");
	return string_format("%q {%q}", ast_node->struct_instance.identifier, joined);
}

static htable_o *s_table_g = NULL;

object_o ast_str(ast_node_o *ast_node)
{

	if (s_table_g == NULL)
	{
		s_table_g = new_htable(20);
		pool_globals_add(s_table_g);
		htable_set(s_table_g, number(NODE_LIST), function("list", (fun_generic_t)s_list));
		htable_set(s_table_g, number(NODE_WORD), function("word", (fun_generic_t)s_word));
		htable_set(s_table_g, number(NODE_NULL), function("null", (fun_generic_t)s_null));
		htable_set(s_table_g, number(NODE_BREAK), function("break", (fun_generic_t)s_break));
		htable_set(s_table_g, number(NODE_ARRAY), function("array", (fun_generic_t)s_array));
		htable_set(s_table_g, number(NODE_NUMBER), function("number", (fun_generic_t)s_number));
		htable_set(s_table_g, number(NODE_STRUCT), function("struct", (fun_generic_t)s_struct));
		htable_set(s_table_g, number(NODE_STRING), function("string", (fun_generic_t)s_string));
		htable_set(s_table_g, number(NODE_OBJECT), function("object", (fun_generic_t)s_object));
		htable_set(s_table_g, number(NODE_KEY_VALUE), function("number", (fun_generic_t)s_key_value));
		htable_set(s_table_g, number(NODE_CONTINUE), function("continue", (fun_generic_t)s_continue));
		htable_set(s_table_g, number(NODE_IMPORT), function("import_smt", (fun_generic_t)s_import_smt));
		htable_set(s_table_g, number(NODE_REFERENCE), function("reference", (fun_generic_t)s_reference));
		htable_set(s_table_g, number(NODE_RETURN), function("return_smt", (fun_generic_t)s_return_smt));
		htable_set(s_table_g, number(NODE_UNARY_EXP), function("unary_exp", (fun_generic_t)s_unary_exp));
		htable_set(s_table_g, number(NODE_BINARY_EXP), function("binary_exp", (fun_generic_t)s_binary_exp));
		htable_set(s_table_g, number(NODE_VARIABLE), function("variable_dec", (fun_generic_t)s_variable_dec));
		htable_set(s_table_g, number(NODE_DEREFERENCE), function("dereference", (fun_generic_t)s_dereference));
		htable_set(s_table_g, number(NODE_TERNARY_EXP), function("if_statement", (fun_generic_t)s_if_statement));
		htable_set(s_table_g, number(NODE_FUNCTION_DEC), function("function_dec", (fun_generic_t)s_function_dec));
		htable_set(s_table_g, number(NODE_FUNCTION_CALL), function("function_call", (fun_generic_t)s_function_call));
		htable_set(s_table_g, number(NODE_FOR_STATEMENT), function("for_statement", (fun_generic_t)s_for_statement));
		htable_set(s_table_g, number(NODE_PARAMETER_DEC), function("parameter_dec", (fun_generic_t)s_parameter_dec));
		htable_set(s_table_g, number(NODE_SUPER_STATEMENT), function("source_elements", (fun_generic_t)s_super_elements));
		htable_set(s_table_g, number(NODE_WHILE_STATEMENT), function("while_statement", (fun_generic_t)s_while_statement));
		htable_set(s_table_g, number(NODE_NT_FUNCTION), function("nt_function_call", (fun_generic_t)s_nt_function_call));
		htable_set(s_table_g, number(NODE_STRUCT_INSTANCE), function("struct_instance", (fun_generic_t)s_struct_instance));
	}

	if (ast_node == NULL)
		return string_from("NULL");

	function_o *function = (function_o *)htable_get(s_table_g, number(ast_node->node_type));

	if (function == NULL)
	{
		error(cstring(string_format("Str unknown node type: %d", ast_node->node_type)), PRINT | PANIC);
	}

	fun_ast_str fun = (fun_ast_str)function->function;

	return fun(ast_node);
}