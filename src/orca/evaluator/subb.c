#include "orca/orca.h"

void syntax_error(string_o *err)
{
	error(cstring(err), PRINT | PANIC);
}

object_o resolve_variable(frame_o *frame, object_o variable)
{
	while (frame)
	{
		ast_node_o *node = frame_get(frame, share(variable));
		if (node)
		{
			return node;
		}
		frame = frame->parent;
	}

	return NULL;
}

void set_variable(frame_o *frame, object_o variable, object_o value)
{
	ast_node_o *var = NULL;
	while (frame)
	{
		if ((var = *(ast_node_o **)frame_get(frame, share(variable))))
		{
			if (object_typeof(var) == AST_NODE_O && var->node_type == NODE_VARIABLE)
			{
				DROP(var->variable.value);
				var->variable.value = share(value);
				return;
			}
		}

		frame = frame->parent;
	}

	syntax_error(string_format("Undefined variable: %q", variable));
}

bool decay_to_bool(object_o object)
{
	if (object == NULL)
	{
		return 0;
	}
	else if (is_error(object))
	{
		return 0;
	}
	else if (object_typeof(object) == NT_INT_O)
	{
		return (*(long *)object);
	}
	else if (object_typeof(object) == NT_NULL_O)
	{
		return 0;
	}

	return 1;
}

long *perfom_binary_exp(object_o left, object_o right, token_type_e operator)
{
	switch (operator)
	{
	case TOKEN_PLUS:
	{
		return add(left, right);
	}
	case TOKEN_MINUS:
	{
		return minus(left, right);
	}
	case TOKEN_MULT:
	{
		return mult(left, right);
	}
	case TOKEN_DIVIDE:
	{
		return divide(left, right);
	}
	case TOKEN_MODULO:
	{
		return modulo(left, right);
	}
	case TOKEN_LESSER:
	{
		return lesser(left, right);
	}
	case TOKEN_GREATER:
	{
		return greater(left, right);
	}
	case TOKEN_IS_EQUAL:
	{
		return number(equal(left, right));
	}
	case TOKEN_LESSER_OR_EQUAL:
	{
		return lesser_or_equals(left, right);
	}
	case TOKEN_GREATER_OR_EQUAL:
	{
		return greater_or_equals(left, right);
	}
	case TOKEN_IS_NOT_EQUAL:
	{
		return is_not_equal(left, right);
	}
	default:
		break;
	}

	return NULL;
}

void e_program(ast_node_o *ast_node, frame_o *frame)
{
	ast_node_o *expression = ast_node->super_statement.statement;
	while (expression)
	{
		ast_evaluate(expression, frame);
		expression = expression->next;
	}
}

void e_expression(ast_node_o *ast_node, frame_o *frame)
{
	ast_node_o *expression = ast_node->super_statement.statement;
	while (expression)
	{
		ast_evaluate(expression, frame);
		expression = expression->next;
	}
}

void e_number(ast_node_o *ast_node, frame_o *frame)
{
	array_push(frame->stack, clone(ast_node->number));
}

void e_string(ast_node_o *ast_node, frame_o *frame)
{
	array_push(frame->stack, share(ast_node->string));
}

void e_null(ast_node_o *ast_node, frame_o *frame)
{
	array_push(frame->stack, null());
}

void e_word(ast_node_o *ast_node, frame_o *frame)
{
	object_o object = resolve_variable(frame, ast_node->word);

	if (object == NULL)
	{
		syntax_error(string_format("Undefined variable: %q", ast_node->word));
	}

	if (object_typeof(object) == NT_POINTER_O)
	{
		ast_node_o *node = *(object_o *)object;

		if (node->node_type == NODE_VARIABLE)
		{
			if (object_typeof(node->variable.value) == NT_INT_O ||
				object_typeof(node->variable.value) == NT_STRING_O ||
				object_typeof(node->variable.value) == NT_POINTER_O ||
				object_typeof(node->variable.value) == NT_NULL_O)
			{
				array_push(frame->stack, share(node->variable.value));
			}
			else
			{
				array_push(frame->stack, pointer(node->variable.value));
			}
		}
	}
	else
	{
		array_push(frame->stack, share(object));
	}
}

void e_reference(ast_node_o *ast_node, frame_o *frame)
{
	object_o object = resolve_variable(frame, ast_node->word);
	if (object == NULL)
	{
		syntax_error(string_format("Undefined variable: %q", ast_node->word));
	}

	array_push(frame->stack, share(object));
}

void e_dereference(ast_node_o *ast_node, frame_o *frame)
{
	object_o object = resolve_variable(frame, ast_node->word);

	if (object == NULL)
	{
		syntax_error(string_format("Undefined variable: %q", ast_node->word));
	}

	if (object_typeof(object) == NT_POINTER_O)
	{
		ast_node_o *node = *(object_o *)object;
		ast_node_o *value = *(ast_node_o **)node->variable.value;
		array_push(frame->stack, share(value->variable.value));
	}
}

void e_unary(ast_node_o *ast_node, frame_o *frame)
{
	ast_evaluate(ast_node->unary_exp.operand, frame);

	SMART object_o object = array_pop(frame->stack);

	if (object_typeof(object) == NT_POINTER_O)
	{
		ast_node_o *node = *(object_o *)object;
		array_push(frame->stack, pointer(node));
	}
	else
	{
		syntax_error(string_from(
			"Syntax error:\n"
			"Invalid use of dereference operator (*)\n"
			"Only pointers can be dereferenced.\n"));
	}
}

void e_return(ast_node_o *ast_node, frame_o *frame)
{
	if (ast_node->return_smt.exp != NULL)
	{
		ast_evaluate(ast_node->return_smt.exp, frame);
		frame->return_value = array_pop(frame->stack);
	}
	frame->return_flag = true;
}

void e_break(ast_node_o *ast_node, frame_o *frame)
{
	frame->break_flag = true;
}

void e_continue(ast_node_o *ast_node, frame_o *frame)
{
	frame->continue_flag = true;
}

void e_left_hand(ast_node_o *ast_node, frame_o *frame)
{
	ast_node_o *left = ast_node->super_statement.statement;

	if (left->next)
	{
		ast_node_o *object = *(ast_node_o **)resolve_variable(frame, left->word);

		if (object_typeof(object) == AST_NODE_O && object->node_type == NODE_VARIABLE)
		{
			object_o value = object->variable.value;

			left = left->next;

			while (left)
			{
				SMART object_o index = NULL;

				if (left->node_type == NODE_SUPER_STATEMENT)
				{
					ast_evaluate(left, frame);
					index = array_pop(frame->stack);
				}
				else
				{
					index = share(left->word);
				}

				if (object_typeof(value) == NT_HASH_TABLE_O)
				{
					value = htable_get(value, share(index));
				}
				else if (object_typeof(value) == NT_ARRAY_O)
				{
					value = array_at(value, (*(long *)index));
					if (value == NULL)
					{
						syntax_error(string_format(
							"Runtime error: \n"
							"Accessing array out of bounds."));
					}
				}

				left = left->next;
			}
			if (object_typeof(value) == NT_INT_O ||
				object_typeof(value) == NT_STRING_O ||
				object_typeof(value) == NT_POINTER_O ||
				object_typeof(value) == NT_NULL_O)
			{
				array_push(frame->stack, share(value));
			}
			else
			{
				array_push(frame->stack, pointer(value));
			}
		}
	}
	else
	{
		ast_evaluate(left, frame);
	}
}

void e_variable(ast_node_o *ast_node, frame_o *frame)
{
	ast_node_o *var = ast_node->super_statement.statement;
	object_o result = NULL;
	while (var)
	{
		if (var->variable.initializer)
		{
			ast_evaluate(var->variable.initializer, frame);
			result = array_pop(frame->stack);
		}

		if (!result)
		{
			result = null();
		}

		var->variable.value = result;

		frame_set(frame, share(var->variable.identifier->word), pointer(var));

		var = var->next;
	}
}

void e_assign(ast_node_o *left, object_o right, frame_o *frame)
{
	if (!left)
		return;

	if (left && left->next)
	{
		ast_node_o *object = *(ast_node_o **)resolve_variable(frame, left->word);

		if (object_typeof(object) == AST_NODE_O && object->node_type == NODE_VARIABLE)
		{
			object_o value = object->variable.value;

			left = left->next;

			while (left->next)
			{
				SMART object_o index = NULL;
				if (left->node_type == NODE_SUPER_STATEMENT)
				{
					ast_evaluate(left, frame);
					index = array_pop(frame->stack);
				}
				else
				{
					index = share(left->word);
				}

				if (object_typeof(value) == NT_HASH_TABLE_O)
				{
					value = htable_get(value, share(index));
				}
				else if (object_typeof(value) == NT_ARRAY_O)
				{
					value = array_at(value, (*(long *)index));
					if (value == NULL)
					{
						syntax_error(string_format(
							"Runtime error: \n"
							"Accessing array out of bounds."));
					}
				}

				left = left->next;
			}

			SMART object_o index = NULL;
			if (left->node_type == NODE_SUPER_STATEMENT)
			{
				ast_evaluate(left, frame);
				index = array_pop(frame->stack);
			}
			else
			{
				index = share(left->word);
			}

			if (object_typeof(value) == NT_HASH_TABLE_O)
			{
				htable_set(value, share(index), share(right));
			}
			else if (object_typeof(value) == NT_ARRAY_O)
			{
				if (!array_insert(value, (*(long *)index), share(right)))
				{
					syntax_error(string_format(
						"Runtime error: \n"
						"Index out of bounds."));
				}
			}
		}
	}
	else
	{
		set_variable(frame, left->word, right);
	}
}

void e_binary(ast_node_o *ast_node, frame_o *frame)
{
	ast_evaluate(ast_node->binary_exp.right, frame);

	SMART object_o right = array_pop(frame->stack);

	if (ast_node->binary_exp.token_type == TOKEN_EQUALS)
	{
		if (ast_node->binary_exp.left->node_type == NODE_UNARY_EXP)
		{
			ast_evaluate(ast_node->binary_exp.left, frame);
			SMART object_o left = array_pop(frame->stack);
			if (object_typeof(left) == NT_POINTER_O)
			{
				ast_node_o *var = *(ast_node_o **)left;
				DROP(var->variable.value);
				var->variable.value = share(right);
			}
		}
		else
		{
			set_variable(frame, ast_node->binary_exp.left->word, right);
		}
	}
	else
	{
		ast_evaluate(ast_node->binary_exp.left, frame);
		SMART object_o left = array_pop(frame->stack);
		object_o result = perfom_binary_exp(left, right, ast_node->binary_exp.token_type);
		array_push(frame->stack, result);
	}
}

void e_function_expression(ast_node_o *ast_node, frame_o *frame)
{
	if (ast_node->function_dec.identifier)
	{
		frame_set(frame, share(ast_node->function_dec.identifier->word), pointer(ast_node));
	}
	else
	{
		array_push(frame->stack, pointer(ast_node));
	}
}

void e_function_call(ast_node_o *ast_node, frame_o *frame)
{
	ast_node_o *identifier = ast_node->function_call.identifier;

	ast_node_o *function = *(ast_node_o **)resolve_variable(frame, identifier->word);

	if (function == NULL)
	{
		syntax_error(string_format("Undefined function: %q", identifier->word));
	}

	if (object_typeof(function) != AST_NODE_O)
	{
		syntax_error(string_format("Function call on a non function type: %q", identifier->word));
	}

	ast_node_o *args = ast_node->function_call.arguments;

	SMART array_o *evaluated_args = new_array(0);
	while (args)
	{
		ast_evaluate(args, frame);
		object_o object = array_pop(frame->stack);
		array_push(evaluated_args, object);
		args = args->next;
	}

	if (function->node_type == NODE_NT_FUNCTION)
	{
		char *name = cstring(function->nt_function_call.identifier->word);
		object_o built_ret = builtin(name, evaluated_args);
		if (built_ret)
		{
			array_push(frame->stack, built_ret);
		}
		return;
	}

	SMART frame_o *new_stack_frame = new_frame(frame);
	ast_node_o *parameters = function->function_dec.parameters;

	int a = 0;
	while (parameters)
	{
		if (array_at(evaluated_args, a))
		{
			parameters->variable.value = share(array_at(evaluated_args, a));

			frame_set(new_stack_frame,
					  share(parameters->variable.identifier->word),
					  pointer(parameters));
		}
		else
		{
			if (parameters->variable.initializer)
			{
				ast_evaluate(parameters->variable.initializer, frame);
				frame_set(new_stack_frame,
						  share(parameters->variable.identifier->word),
						  array_pop(frame->stack));
			}
			else
			{
				syntax_error(string_format("Missing parameter: %q", parameters->variable.identifier->word));
			}
		}
		a++;
		parameters = parameters->next;
	}

	if (a < evaluated_args->length)
	{
		syntax_error(string_format("Too many arguments: %q", function->function_dec.identifier->word));
	}

	ast_evaluate(function->function_dec.body, new_stack_frame);

	if (new_stack_frame->return_value)
	{
		array_push(frame->stack, share(new_stack_frame->return_value));
	}
}

void e_block(ast_node_o *ast_node, frame_o *frame)
{
	SMART frame_o *new_stack_frame = new_frame(frame);
	ast_node_o *statement = ast_node->super_statement.statement;
	while (statement)
	{
		ast_evaluate(statement, new_stack_frame);

		if (new_stack_frame->break_flag || new_stack_frame->continue_flag)
		{
			break;
		}

		statement = statement->next;
	}

	frame->break_flag = new_stack_frame->break_flag;
	frame->continue_flag = new_stack_frame->continue_flag;

	if (new_stack_frame->return_flag)
	{
		frame->return_flag = new_stack_frame->return_flag;
		if (new_stack_frame->return_value != NULL)
		{
			frame->return_value = share(new_stack_frame->return_value);
		}
	}
}

void e_native(ast_node_o *ast_node, frame_o *frame)
{
	if (ast_node->nt_function_call.arguments)
	{
		ast_node_o *args = ast_node->nt_function_call.arguments;

		SMART array_o *evaluated_args = new_array(0);
		while (args)
		{
			ast_evaluate(args, frame);
			array_push(evaluated_args, array_pop(frame->stack));
			args = args->next;
		}

		char *method = cstring(ast_node->nt_function_call.identifier->word);

		object_o result = builtin(method, evaluated_args);
		if (result)
		{
			array_push(frame->stack, result);
		}
	}
	else
	{
		array_push(frame->stack, share(ast_node));
	}
}

void e_array(ast_node_o *ast_node, frame_o *frame)
{
	array_o *array = new_array(0);
	ast_node_o *elems = ast_node->array.elements;

	while (elems)
	{
		ast_evaluate(elems, frame);
		array_push(array, array_pop(frame->stack));
		elems = elems->next;
	}

	array_push(frame->stack, array);
}

void e_list(ast_node_o *ast_node, frame_o *frame)
{
	list_node_o *node = NULL;
	ast_node_o *elems = ast_node->list.head;

	while (elems)
	{
		ast_evaluate(elems, frame);
		list_unshift(&node, array_pop(frame->stack));
		elems = elems->next;
	}

	array_push(frame->stack, node);
}

void e_htable(ast_node_o *ast_node, frame_o *frame)
{
	int length = 0;
	ast_node_o *key_values = ast_node->object.members;

	while (key_values)
	{
		length++;
		key_values = key_values->next;
	}

	htable_o *result = new_htable(length);

	key_values = ast_node->object.members;

	while (key_values)
	{
		ast_evaluate(key_values->key_value.key, frame);
		ast_evaluate(key_values->key_value.value, frame);
		htable_set(result, array_pop(frame->stack), array_pop(frame->stack));
		key_values = key_values->next;
	}

	array_push(frame->stack, result);
}

void e_struct_instance(ast_node_o *ast_node, frame_o *frame)
{
	int length = 0;
	ast_node_o *key_values = ast_node->struct_instance.fields;
	ast_node_o *object = (ast_node_o *)resolve_variable(frame, ast_node->struct_instance.identifier->word);

	if (object == NULL)
	{
		syntax_error(string_format("Struct declaration not found: %q", ast_node->struct_instance.identifier->word));
	}

	ast_node_o *f = object->struct_statement.body;

	while (key_values)
	{
		length++;
		key_values = key_values->next;
	}

	htable_o *result = new_htable(length);
	key_values = ast_node->struct_instance.fields;

	while (key_values)
	{
		ast_evaluate(key_values->key_value.value, frame);

		htable_set(result, share(key_values->key_value.key->word), array_pop(frame->stack));

		key_values = key_values->next;
	}

	while (f)
	{
		if (f->node_type == NODE_FUNCTION_DEC)
		{
			htable_set(result, share(f->function_dec.identifier->word), share(f));
		}
		f = f->next;
	}

	array_push(frame->stack, result);
}

void e_import(ast_node_o *ast_node, frame_o *frame)
{
	htable_o *b = orca(share(ast_node->import_smt.path->string));
	string_o *name = htable_get(b, string_from("package"));

	if (name)
	{
		frame_set(frame, clone(name), b);
	}
	else
	{
		syntax_error(string_format(
			"Undefined package: %q\n"
			"Tip! Ensure the export has a 'package' property.\n"
			"\nExample:\n"
			"export = {\n 'package': 'my_package'\n}\n",
			ast_node->import_smt.path->string));
	}
}

void e_ternary(ast_node_o *ast_node, frame_o *frame)
{
	ast_evaluate(ast_node->ternary_exp.condition, frame);

	SMART object_o condition = array_pop(frame->stack);

	if (decay_to_bool(condition))
	{
		ast_evaluate(ast_node->ternary_exp.if_body, frame);
	}
	else
	{
		if (ast_node->ternary_exp.else_body != NULL)
		{
			ast_evaluate(ast_node->ternary_exp.else_body, frame);
		}
	}
}

void e_while(ast_node_o *ast_node, frame_o *frame)
{
	ast_evaluate(ast_node->while_statement.condition, frame);

	SMART object_o condition = array_pop(frame->stack);
	int condition_result = decay_to_bool(condition);

	while (condition_result)
	{
		ast_evaluate(ast_node->while_statement.body, frame);
		ast_evaluate(ast_node->while_statement.condition, frame);

		if (frame->break_flag)
		{
			frame->break_flag = false;
			break;
		}

		if (frame->continue_flag)
		{
			frame->continue_flag = false;
			continue;
		}

		if (frame->return_flag)
		{
			break;
		}

		SMART object_o condition2 = array_pop(frame->stack);

		condition_result = decay_to_bool(condition2);
	}
}

void e_for(ast_node_o *ast_node, frame_o *frame)
{
	int condition_result;

	if (ast_node->for_statement.init)
	{
		ast_evaluate(ast_node->for_statement.init, frame);
	}

	if (ast_node->for_statement.condition == NULL)
	{
		condition_result = true;
	}
	else
	{
		ast_evaluate(ast_node->for_statement.condition, frame);
		SMART object_o condition = array_pop(frame->stack);
		condition_result = decay_to_bool(condition);
	}

	while (condition_result)
	{
		ast_evaluate(ast_node->for_statement.body, frame);
		ast_evaluate(ast_node->for_statement.update, frame);

		if (frame->break_flag)
		{
			frame->break_flag = false;
			break;
		}

		if (frame->continue_flag)
		{
			frame->continue_flag = false;
			continue;
		}

		if (frame->return_flag)
		{
			break;
		}

		if (ast_node->for_statement.condition != NULL)
		{
			ast_evaluate(ast_node->for_statement.condition, frame);

			SMART object_o condition2 = array_pop(frame->stack);
			condition_result = decay_to_bool(condition2);
		}
	}
}

void e_struct(ast_node_o *ast_node, frame_o *frame)
{
	frame_set(frame, share(ast_node->struct_statement.identifier->word), share(ast_node));
}

static htable_o *e_table_g = NULL;

void ast_evaluate(ast_node_o *ast_node, frame_o *frame)
{
	if (e_table_g == NULL)
	{
		e_table_g = new_htable(30);
		pool_globals_add(e_table_g);
		htable_set(e_table_g, number(NODE_WORD), function("word", (fun_generic_t)e_word));
		htable_set(e_table_g, number(NODE_NULL), function("null", (fun_generic_t)e_null));
		htable_set(e_table_g, number(NODE_LIST), function("list", (fun_generic_t)e_list));
		htable_set(e_table_g, number(NODE_BREAK), function("break", (fun_generic_t)e_break));
		htable_set(e_table_g, number(NODE_ARRAY), function("array", (fun_generic_t)e_array));
		htable_set(e_table_g, number(NODE_OBJECT), function("htable", (fun_generic_t)e_htable));
		htable_set(e_table_g, number(NODE_IMPORT), function("import", (fun_generic_t)e_import));
		htable_set(e_table_g, number(NODE_STRUCT), function("struct", (fun_generic_t)e_struct));
		htable_set(e_table_g, number(NODE_NUMBER), function("number", (fun_generic_t)e_number));
		htable_set(e_table_g, number(NODE_STRING), function("string", (fun_generic_t)e_string));
		htable_set(e_table_g, number(NODE_UNARY_EXP), function("unary", (fun_generic_t)e_unary));
		htable_set(e_table_g, number(NODE_FOR_STATEMENT), function("for", (fun_generic_t)e_for));
		htable_set(e_table_g, number(STATEMENT_BLOCK), function("block", (fun_generic_t)e_block));
		htable_set(e_table_g, number(NODE_BINARY_EXP), function("binary", (fun_generic_t)e_binary));
		htable_set(e_table_g, number(NODE_RETURN), function("return_smt", (fun_generic_t)e_return));
		htable_set(e_table_g, number(NODE_CONTINUE), function("continue", (fun_generic_t)e_continue));
		htable_set(e_table_g, number(NODE_WHILE_STATEMENT), function("while", (fun_generic_t)e_while));
		htable_set(e_table_g, number(NODE_TERNARY_EXP), function("ternary", (fun_generic_t)e_ternary));
		htable_set(e_table_g, number(STATEMENT_PROGRAM), function("program", (fun_generic_t)e_program));
		htable_set(e_table_g, number(NODE_REFERENCE), function("reference", (fun_generic_t)e_reference));
		htable_set(e_table_g, number(LIST_LEFT_HAND), function("left hand", (fun_generic_t)e_left_hand));
		htable_set(e_table_g, number(STATEMENT_VARIABLE), function("variable", (fun_generic_t)e_variable));
		htable_set(e_table_g, number(NODE_NT_FUNCTION), function("native string", (fun_generic_t)e_native));
		htable_set(e_table_g, number(NODE_DEREFERENCE), function("dereference", (fun_generic_t)e_dereference));
		htable_set(e_table_g, number(STATEMENT_EXPRESSION), function("exepression", (fun_generic_t)e_expression));
		htable_set(e_table_g, number(NODE_FUNCTION_DEC), function("function", (fun_generic_t)e_function_expression));
		htable_set(e_table_g, number(NODE_FUNCTION_CALL), function("function call", (fun_generic_t)e_function_call));
		htable_set(e_table_g, number(NODE_STRUCT_INSTANCE), function("struct instance", (fun_generic_t)e_struct_instance));
	}

	if (ast_node == NULL)
		return;

	if (frame->return_flag)
		return;

	function_o *function = NULL;

	if (ast_node->node_type == NODE_SUPER_STATEMENT)
	{
		printf("SUPER: %d\n", ast_node->super_statement.type);
		function = (function_o *)htable_get(e_table_g, number(ast_node->super_statement.type));
	}
	else
	{
		printf("NON SUPER: %d\n", ast_node->node_type);
		function = (function_o *)htable_get(e_table_g, number(ast_node->node_type));
	}

	print(function);

	if (function == NULL)
	{
		error("Eve unknown node type", PRINT | PANIC);
	}

	fun_evaluate_t fun = (fun_evaluate_t)function->function;

	fun(ast_node, frame);
}