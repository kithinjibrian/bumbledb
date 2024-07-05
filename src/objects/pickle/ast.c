#include "objects/pickle/pickle.h"

object_o ast_node_str(object_o object)
{
	ast_node_o *ast_node = (ast_node_o *)object;
	if (object == NULL)
		return NULL;

	switch (ast_node->node_type)
	{
	case NODE_NUMBER:
	{
		return str(ast_node->number);
	}

	case NODE_STRING:
	{
		return str(ast_node->string);
	}

	case NODE_WORD:
	{
		return str(ast_node->word);
	}

	case NODE_KEY_VALUE:
	{
		SMART object_o key = ast_node_str(ast_node->key_value.key);
		SMART object_o value = ast_node_str(ast_node->key_value.value);
		return string_format("%q: %q", key, value);
	}

	case NODE_RETURN:
	{
		SMART object_o exp = ast_node_str(ast_node->return_smt.exp);
		return string_format("return %q", exp);
	}

	case NODE_IMPORT:
	{
		SMART object_o exp = ast_node_str(ast_node->import_smt.path);
		return string_format("import %q", exp);
	}

	case NODE_BREAK:
	{
		return string_from("break;");
	}

	case NODE_CONTINUE:
	{
		return string_from("continue;");
	}

	case NODE_POSTFIX_INC:
	{
		SMART object_o iden = ast_node_str(ast_node->postfix_inc.identifier);
		return string_format("%q++", iden);
	}

	case NODE_BINARY_EXP:
	{
		SMART object_o left = ast_node_str(ast_node->binary_exp.left);
		SMART object_o right = ast_node_str(ast_node->binary_exp.right);
		SMART string_o *op = string_from("%d", ast_node->binary_exp.token_type);

		return string_format("%q <%q> %q", left, op, right);
	}

	case NODE_VARIABLE_DEC:
	{
		SMART object_o type = ast_node_str(ast_node->variable_dec.type);
		SMART object_o init = ast_node_str(ast_node->variable_dec.initializer);
		SMART object_o identifier = ast_node_str(ast_node->variable_dec.identifier);

		return string_format("let %q: %q = %q", identifier, type, init);
	}

	case NODE_PARAMETER_DEC:
	{
		SMART object_o type = ast_node_str(ast_node->parameter_dec.type);
		SMART object_o init = ast_node_str(ast_node->parameter_dec.initializer);
		SMART object_o identifier = ast_node_str(ast_node->parameter_dec.identifier);

		return string_format("%q: %q", identifier, type, init);
	}

	case NODE_ARRAY_ACCESS:
	{
		SMART object_o index_exp = ast_node_str(ast_node->array_access.index_exp);
		SMART object_o identifier = ast_node_str(ast_node->array_access.identifier);

		return string_format("%q[%q]", identifier, index_exp);
	}

	case NODE_FIELD_ACCESS:
	{
		SMART object_o object = ast_node_str(ast_node->field_access.object);
		SMART object_o identifier = ast_node_str(ast_node->field_access.identifier);

		return string_format("%q.%q", object, identifier);
	}

	case NODE_FUNCTION_DEC:
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

		return string_format("fun %q (%q): -> %q %q ", identifier, joined, return_type, body);
	}

	case NODE_FUNCTION_CALL:
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

		return string_format("%q(%q)", identifier, joined);
	}

	case NODE_IF_STATEMENT:
	{
		SMART object_o condition = ast_node_str(ast_node->if_statement.condition);
		SMART object_o if_body = ast_node_str(ast_node->if_statement.if_body);
		SMART object_o else_body = ast_node_str(ast_node->if_statement.else_body);
		return string_format("if (%q) { %q } else { %q }", condition, if_body, else_body);
	}

	case NODE_WHILE_STATEMENT:
	{
		SMART object_o body = ast_node_str(ast_node->while_statement.body);
		SMART object_o condition = ast_node_str(ast_node->while_statement.condition);
		return string_format("while(%q) { %q }", condition, body);
	}

	case NODE_FOR_STATEMENT:
	{
		SMART object_o body = ast_node_str(ast_node->for_statement.body);
		SMART object_o init = ast_node_str(ast_node->for_statement.init);
		SMART object_o update = ast_node_str(ast_node->for_statement.update);
		SMART object_o condition = ast_node_str(ast_node->for_statement.condition);
		return string_format("for(%q;%q;%q) { %q }", init, condition, update, body);
	}

	case NODE_METHOD_CALL:
	{
		SMART object_o object = ast_node_str(ast_node->method_call.object);
		SMART object_o identifier = ast_node_str(ast_node->method_call.identifier);

		ast_node_o *arguments = ast_node->method_call.arguments;
		SMART array_o *arr = new_array(0);

		while (arguments)
		{
			array_push(arr, ast_node_str(arguments));
			arguments = arguments->next;
		}

		SMART string_o *joined = array_join(arr, ", ");

		return string_format("%q.%q(%q)", object, identifier, joined);
	}

	case NODE_LIST:
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
	case NODE_ARRAY:
	case NODE_ARGUMENTS:
	case NODE_STATEMENTS:
	{
		ast_node_o *current = NULL;
		if (ast_node->node_type == NODE_STATEMENTS)
		{
			current = ast_node->statements.list;
		}
		else if (ast_node->node_type == NODE_ARGUMENTS)
		{
			current = ast_node->arguments.args;
		}
		else
		{
			current = ast_node->array.elements;
		}

		SMART array_o *arr = new_array(0);
		while (current)
		{
			array_push(arr, ast_node_str(current));
			current = current->next;
		}

		SMART string_o *joined = array_join(arr, ", ");
		return string_format("[%q]", joined);
	}
	case NODE_OBJECT:
	case NODE_COM_STATEMENTS:
	{
		ast_node_o *current = NULL;

		if (ast_node->node_type == NODE_COM_STATEMENTS)
		{
			current = ast_node->compound_statements.list;
		}
		else
		{
			current = ast_node->object.members;
		}

		SMART array_o *arr = new_array(0);
		while (current)
		{
			array_push(arr, ast_node_str(current));
			current = current->next;
		}

		SMART string_o *joined = array_join(arr, ", ");
		return string_format("{%q}", joined);
	}

	default:
		break;
	}

	return NULL;
}

void ast_node_visitor(object_o object, fun_process_t fun_process)
{
	ast_node_o *ast_node = (ast_node_o *)object;
	if (object == NULL)
		return;

	switch (ast_node->node_type)
	{
	case NODE_STRING:
		post_visit(ast_node->string, fun_process);
		break;

	case NODE_WORD:
		post_visit(ast_node->word, fun_process);
		break;

	case NODE_NUMBER:
		post_visit(ast_node->number, fun_process);
		break;

	case NODE_KEY_VALUE:
		post_visit(ast_node->key_value.key, fun_process);
		post_visit(ast_node->key_value.value, fun_process);
		break;

	case NODE_ARGUMENTS:
		post_visit(ast_node->arguments.args, fun_process);
		break;

	case NODE_RETURN:
		post_visit(ast_node->return_smt.exp, fun_process);
		break;

	case NODE_IMPORT:
		post_visit(ast_node->import_smt.path, fun_process);
		break;

	case NODE_POSTFIX_INC:
		post_visit(ast_node->postfix_inc.identifier, fun_process);
		break;

	case NODE_BINARY_EXP:
		post_visit(ast_node->binary_exp.left, fun_process);
		post_visit(ast_node->binary_exp.right, fun_process);
		break;

	case NODE_VARIABLE_DEC:
		post_visit(ast_node->variable_dec.type, fun_process);
		post_visit(ast_node->variable_dec.identifier, fun_process);
		post_visit(ast_node->variable_dec.initializer, fun_process);
		break;

	case NODE_PARAMETER_DEC:
		post_visit(ast_node->parameter_dec.type, fun_process);
		post_visit(ast_node->parameter_dec.identifier, fun_process);
		post_visit(ast_node->parameter_dec.initializer, fun_process);
		break;

	case NODE_FUNCTION_DEC:
		post_visit(ast_node->function_dec.body, fun_process);
		post_visit(ast_node->function_dec.identifier, fun_process);
		post_visit(ast_node->function_dec.parameters, fun_process);
		post_visit(ast_node->function_dec.return_type, fun_process);
		break;

	case NODE_FUNCTION_CALL:
		post_visit(ast_node->function_call.arguments, fun_process);
		post_visit(ast_node->function_call.identifier, fun_process);
		break;

	case NODE_METHOD_CALL:
		post_visit(ast_node->method_call.object, fun_process);
		post_visit(ast_node->method_call.arguments, fun_process);
		post_visit(ast_node->method_call.identifier, fun_process);
		break;

	case NODE_ARRAY_ACCESS:
		post_visit(ast_node->array_access.index_exp, fun_process);
		post_visit(ast_node->array_access.identifier, fun_process);
		break;

	case NODE_FIELD_ACCESS:
		post_visit(ast_node->field_access.object, fun_process);
		post_visit(ast_node->field_access.identifier, fun_process);
		break;

	case NODE_IF_STATEMENT:
		post_visit(ast_node->if_statement.condition, fun_process);
		post_visit(ast_node->if_statement.if_body, fun_process);
		post_visit(ast_node->if_statement.else_body, fun_process);
		break;

	case NODE_WHILE_STATEMENT:
		post_visit(ast_node->while_statement.body, fun_process);
		post_visit(ast_node->while_statement.condition, fun_process);
		break;

	case NODE_FOR_STATEMENT:
		post_visit(ast_node->for_statement.body, fun_process);
		post_visit(ast_node->for_statement.init, fun_process);
		post_visit(ast_node->for_statement.condition, fun_process);
		post_visit(ast_node->for_statement.update, fun_process);
		break;

	case NODE_LIST:
	case NODE_ARRAY:
	case NODE_OBJECT:
	case NODE_STATEMENTS:
	case NODE_COM_STATEMENTS:
	{
		ast_node_o *next = NULL;
		ast_node_o *current = NULL;
		if (ast_node->node_type == NODE_ARRAY)
		{
			current = ast_node->array.elements;
		}
		else if (ast_node->node_type == NODE_LIST)
		{
			current = ast_node->list.head;
		}
		else if (ast_node->node_type == NODE_STATEMENTS)
		{
			current = ast_node->statements.list;
		}
		else if (ast_node->node_type == NODE_COM_STATEMENTS)
		{
			current = ast_node->compound_statements.list;
		}
		else
		{
			current = ast_node->object.members;
		}
		while (current)
		{
			next = current->next;
			post_visit(current, fun_process);
			current = next;
		}
	}
	break;

	default:
		break;
	}
}

ast_node_o *new_ast_node(node_type_e node_type)
{
	ast_node_o *ast_node = alloc(1, sizeof(ast_node_o), AST_NODE_O);
	ast_node->node_type = node_type;
	ast_node->next = NULL;

	switch (node_type)
	{
	case NODE_NUMBER:
		ast_node->number = NULL;
		break;

	case NODE_STRING:
		ast_node->string = NULL;
		break;

	case NODE_WORD:
		ast_node->word = NULL;
		break;

	case NODE_OBJECT:
		ast_node->object.members = NULL;
		break;

	case NODE_ARRAY:
		ast_node->array.elements = NULL;
		break;

	case NODE_LIST:
		ast_node->list.head = NULL;
		break;

	case NODE_KEY_VALUE:
		ast_node->key_value.key = NULL;
		ast_node->key_value.value = NULL;
		break;

	case NODE_POSTFIX_INC:
		ast_node->postfix_inc.identifier = NULL;
		break;

	case NODE_STATEMENTS:
		ast_node->statements.list = NULL;
		break;

	case NODE_COM_STATEMENTS:
		ast_node->compound_statements.list = NULL;
		break;

	case NODE_RETURN:
		ast_node->return_smt.exp = NULL;
		break;

	case NODE_IMPORT:
		ast_node->import_smt.path = NULL;
		break;

	case NODE_ARGUMENTS:
		ast_node->arguments.args = NULL;
		break;

	case NODE_BINARY_EXP:
		ast_node->binary_exp.left = NULL;
		ast_node->binary_exp.right = NULL;
		break;

	case NODE_VARIABLE_DEC:
		ast_node->variable_dec.type = NULL;
		ast_node->variable_dec.identifier = NULL;
		ast_node->variable_dec.initializer = NULL;
		break;

	case NODE_PARAMETER_DEC:
		ast_node->parameter_dec.type = NULL;
		ast_node->parameter_dec.identifier = NULL;
		ast_node->parameter_dec.initializer = NULL;
		break;

	case NODE_FUNCTION_DEC:
		ast_node->function_dec.body = NULL;
		ast_node->function_dec.identifier = NULL;
		ast_node->function_dec.parameters = NULL;
		ast_node->function_dec.return_type = NULL;
		break;

	case NODE_FUNCTION_CALL:
		ast_node->function_call.identifier = NULL;
		ast_node->function_call.arguments = NULL;
		break;

	case NODE_METHOD_CALL:
		ast_node->method_call.object = NULL;
		ast_node->method_call.identifier = NULL;
		ast_node->method_call.arguments = NULL;
		break;

	case NODE_ARRAY_ACCESS:
		ast_node->array_access.identifier = NULL;
		ast_node->array_access.index_exp = NULL;
		break;

	case NODE_FIELD_ACCESS:
		ast_node->field_access.object = NULL;
		ast_node->field_access.identifier = NULL;
		break;

	case NODE_IF_STATEMENT:
		ast_node->if_statement.condition = NULL;
		ast_node->if_statement.if_body = NULL;
		ast_node->if_statement.else_body = NULL;
		break;

	case NODE_WHILE_STATEMENT:
		ast_node->while_statement.body = NULL;
		ast_node->while_statement.condition = NULL;
		break;

	case NODE_FOR_STATEMENT:
		ast_node->for_statement.body = NULL;
		ast_node->for_statement.init = NULL;
		ast_node->for_statement.update = NULL;
		ast_node->for_statement.condition = NULL;
		break;

	default:
		break;
	}

	static const vtable_t vt = {
		.__str__ = ast_node_str,
		.__visitor__ = ast_node_visitor};

	object_reg_dunders(ast_node, &vt);

	return ast_node;
}

void ast_node_push(ast_node_o **root_node, ast_node_o *node)
{
	if (*root_node == NULL)
	{
		*root_node = node;
	}
	else
	{
		ast_node_o *temp = *root_node;
		while (temp->next != NULL)
		{
			temp = temp->next;
		}
		temp->next = node;
	}
}