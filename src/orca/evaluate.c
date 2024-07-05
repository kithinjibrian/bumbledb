#include "orca/orca.h"

bool decay_true(object_o object);
void check_return(frame_o *frame, ast_node_o *ast_node);
object_o resolve_variable(frame_o *frame, object_o variable);
void set_variable(frame_o *frame, object_o variable, object_o value);
long *perfom_binary_exp(object_o left, object_o right, token_type_e operator);

bool decay_true(object_o object)
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

	return 1;
}

void check_return(frame_o *frame, ast_node_o *ast_node)
{
	if (ast_node->node_type == NODE_FUNCTION_CALL)
	{
		string_o *identifier = ast_node->function_call.identifier->word;
		ast_node_o *func = resolve_variable(frame, identifier);

		if (func != NULL)
		{
			if (func->function_dec.return_type)
			{
				char *return_type = cstring(func->function_dec.return_type->word);

				if (strcmp(return_type, "void") == 0)
				{
					SMART string_o *err = string_format("Function \"%q()\" does not return a value", identifier);
					error(cstring(err), PRINT | PANIC);
				}
			}
		}
		else
		{
			char *name = cstring(identifier);
			if (strcmp(name, "print") == 0)
			{
				SMART string_o *err = string_format("Function \"%q()\" does not return a value", identifier);
				error(cstring(err), PRINT | PANIC);
			}
		}
	}
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
	default:
		break;
	}

	return number(0);
}

object_o resolve_variable(frame_o *frame, object_o variable)
{
	while (frame)
	{
		object_o object = frame_get(frame, share(variable));
		if (object)
		{
			return object;
		}
		frame = frame->parent;
	}

	return NULL;
}

void set_variable(frame_o *frame, object_o variable, object_o value)
{
	while (frame)
	{
		if (frame_get(frame, share(variable)))
		{
			frame_set(frame, share(variable), share(value));
			return;
		}

		frame = frame->parent;
	}
}

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

	switch (ast_node->node_type)
	{
	case NODE_NUMBER:
		array_push(frame->stack, share(ast_node->number));
		break;
	case NODE_STRING:
		array_push(frame->stack, share(ast_node->string));
		break;
	case NODE_WORD:
	{
		object_o object = resolve_variable(frame, ast_node->word);
		if (object == NULL)
		{
			string_o *err = string_format("Undefined variable: %q", ast_node->word);
			error(cstring(err), PRINT | PANIC);
		}
		array_push(frame->stack, share(object));
		break;
	}
	case NODE_ARRAY:
	{
		array_o *array_object = new_array(0);
		ast_node_o *current = ast_node->array.elements;

		while (current)
		{
			__orca_evaluate__(current, frame);
			object_o element = array_pop(frame->stack);
			array_push(array_object, element);
			current = current->next;
		}

		array_push(frame->stack, array_object);
		break;
	}
	case NODE_OBJECT:
	{
		int length = 0;
		ast_node_o *current = ast_node->object.members;

		while (current)
		{
			length++;
			current = current->next;
		}

		current = ast_node->object.members;

		htable_o *object = new_htable(length);

		while (current)
		{
			__orca_evaluate__(current->key_value.key, frame);
			__orca_evaluate__(current->key_value.value, frame);

			object_o value = array_pop(frame->stack);
			object_o key = array_pop(frame->stack);

			htable_set(object, key, value);

			current = current->next;
		}

		array_push(frame->stack, object);
		break;
	}
	case NODE_ARRAY_ACCESS:
	{
		SMART object_o object = NULL;

		if (ast_node->array_access.identifier->node_type == NODE_NUMBER)
		{
			object = resolve_variable(frame, ast_node->array_access.identifier);
		}
		else
		{
			__orca_evaluate__(ast_node->array_access.identifier, frame);
			object = array_pop(frame->stack);
		}

		if (object == NULL)
		{
			string_o *err = string_format("Array is not defined: %q", ast_node->array_access.identifier->word);
			error(cstring(err), PRINT | PANIC);
		}

		__orca_evaluate__(ast_node->array_access.index_exp, frame);

		SMART object_o index = array_pop(frame->stack);

		object_o result = NULL;

		if (object_typeof(object) == NT_ARRAY_O)
		{
			result = array_at(object, (*(long *)index));
		}
		else if (object_typeof(object) == NT_HASH_TABLE_O)
		{
			result = htable_get(object, share(index));
		}

		array_push(frame->stack, share(result));

		break;
	}
	case NODE_FIELD_ACCESS:
	{
		object_o object = resolve_variable(frame, ast_node->field_access.object->word);

		if (object == NULL)
		{
			string_o *err = string_format("Object is not defined: %q", ast_node->field_access.identifier->word);
			error(cstring(err), PRINT | PANIC);
		}

		switch (object_typeof(object))
		{
		case NT_ARRAY_O:
		{
			array_o *array_object = (array_o *)object;

			char *field = cstring(ast_node->field_access.identifier->word);

			if (strcmp(field, "index") == 0)
			{
				array_push(frame->stack, number(array_object->index));
			}
			break;
		}

		default:
			break;
		}
		break;
	}
	case NODE_BINARY_EXP:
	{
		check_return(frame, ast_node->binary_exp.left);
		check_return(frame, ast_node->binary_exp.right);

		__orca_evaluate__(ast_node->binary_exp.right, frame);

		SMART object_o right = array_pop(frame->stack);

		if (ast_node->binary_exp.token_type == TOKEN_EQUALS)
		{
			if (ast_node->binary_exp.left->node_type == NODE_ARRAY_ACCESS)
			{
				ast_node_o *ast_array = ast_node->binary_exp.left;

				object_o object = resolve_variable(frame, ast_array->array_access.identifier->word);

				if (object == NULL)
				{
					string_o *err = string_format("Array is not defined: %q", ast_array->array_access.identifier->word);
					error(cstring(err), PRINT | PANIC);
				}

				__orca_evaluate__(ast_array->array_access.index_exp, frame);

				SMART object_o index = array_pop(frame->stack);

				if (object_typeof(object) == NT_ARRAY_O)
				{
					if (!(array_insert(object, (*(long *)index), share(right))))
					{
						string_o *err = string_format("Index out of bounds: Array %q", ast_array->array_access.identifier->word);
						error(cstring(err), PRINT | PANIC);
					}
				}
				else if (object_typeof(object) == NT_HASH_TABLE_O)
				{
					htable_set(object, share(index), share(right));
				}
			}
			else if (ast_node->binary_exp.left->node_type == NODE_FIELD_ACCESS)
			{
				ast_node_o *ast_field = ast_node->binary_exp.left;

				char *field = cstring(ast_field->field_access.identifier->word);

				object_o object = resolve_variable(frame, ast_field->field_access.object->word);

				if (object == NULL)
				{
					string_o *err = string_from("Object is not defined: %s", field);
					error(cstring(err), PRINT | PANIC);
				}

				switch (object_typeof(object))
				{
				case NT_ARRAY_O:
				{
					array_o *array_object = (array_o *)object;
					if (strcmp(field, "index") == 0)
					{
						long index = (*(long *)right);
						if (index < 0 || index >= array_object->length)
						{
							string_o *err = string_format("Index out of bounds: Array %q", ast_field->field_access.object->word);
							error(cstring(err), PRINT | PANIC);
						}
						array_object->index = (*(long *)right);
					}
					break;
				}

				default:
					break;
				}
			}
			else
			{
				set_variable(frame, ast_node->binary_exp.left->word, right);
			}
		}
		else
		{
			__orca_evaluate__(ast_node->binary_exp.left, frame);
			SMART object_o left = array_pop(frame->stack);

			object_o result = perfom_binary_exp(left, right, ast_node->binary_exp.token_type);

			array_push(frame->stack, result);
		}

		break;
	}

	case NODE_POSTFIX_INC:
	{
		object_o object = resolve_variable(frame, ast_node->postfix_inc.identifier->word);
		if (object == NULL)
		{
			string_o *err = string_format("Undefined variable: %q", ast_node->postfix_inc.identifier->word);
			error(cstring(err), PRINT | PANIC);
		}

		array_push(frame->stack, clone(object));

		*(long *)object = (*(long *)object) + 1;

		break;
	}

	case NODE_RETURN:
	{
		if (ast_node->return_smt.exp != NULL)
		{
			check_return(frame, ast_node->return_smt.exp);
			__orca_evaluate__(ast_node->return_smt.exp, frame);
			frame->return_value = array_pop(frame->stack);
		}
		frame->return_flag = true;
		break;
	}

	case NODE_BREAK:
	{
		frame->break_flag = true;
		break;
	}

	case NODE_CONTINUE:
	{
		frame->continue_flag = true;
		break;
	}

	case NODE_VARIABLE_DEC:
	{
		object_o result;

		if (ast_node->variable_dec.initializer != NULL)
		{
			check_return(frame, ast_node->variable_dec.initializer);
			__orca_evaluate__(ast_node->variable_dec.initializer, frame);

			result = array_pop(frame->stack);
		}
		else
		{
			result = number(0);
		}

		frame_set(frame, share(ast_node->variable_dec.identifier->word), result);
		break;
	}

	case NODE_IF_STATEMENT:
	{
		check_return(frame, ast_node->if_statement.condition);
		__orca_evaluate__(ast_node->if_statement.condition, frame);

		SMART object_o condition = array_pop(frame->stack);
		long condition_result = (*(long *)condition);

		if (condition_result)
		{
			__orca_evaluate__(ast_node->if_statement.if_body, frame);
		}
		else
		{
			if (ast_node->if_statement.else_body != NULL)
			{
				__orca_evaluate__(ast_node->if_statement.else_body, frame);
			}
		}

		break;
	}

	case NODE_WHILE_STATEMENT:
	{
		check_return(frame, ast_node->while_statement.condition);
		__orca_evaluate__(ast_node->while_statement.condition, frame);

		SMART object_o condition = array_pop(frame->stack);
		int condition_result = decay_true(condition);

		while (condition_result)
		{
			__orca_evaluate__(ast_node->while_statement.body, frame);
			__orca_evaluate__(ast_node->while_statement.condition, frame);

			SMART object_o condition2 = array_pop(frame->stack);

			condition_result = decay_true(condition2);
		}

		break;
	}

	case NODE_FOR_STATEMENT:
	{
		__orca_evaluate__(ast_node->for_statement.init, frame);

		int condition_result;
		if (ast_node->for_statement.condition == NULL)
		{
			condition_result = true;
		}
		else
		{
			__orca_evaluate__(ast_node->for_statement.condition, frame);
			SMART object_o condition = array_pop(frame->stack);
			condition_result = decay_true(condition);
		}

		while (condition_result)
		{
			__orca_evaluate__(ast_node->for_statement.body, frame);
			__orca_evaluate__(ast_node->for_statement.update, frame);

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
				__orca_evaluate__(ast_node->for_statement.condition, frame);

				SMART object_o condition2 = array_pop(frame->stack);
				condition_result = decay_true(condition2);
			}
		}

		break;
	}

	case NODE_STATEMENTS:
	{
		ast_node_o *statement = ast_node->statements.list;

		while (statement && !frame->return_flag)
		{
			__orca_evaluate__(statement, frame);
			statement = statement->next;
		}

		break;
	}

	case NODE_COM_STATEMENTS:
	{
		SMART frame_o *new_stack_frame = new_frame(frame);
		ast_node_o *statement = ast_node->compound_statements.list;

		while (statement && !frame->return_flag)
		{
			__orca_evaluate__(statement, new_stack_frame);

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

		break;
	}

	case NODE_FUNCTION_DEC:
	{
		frame_set(frame, share(ast_node->function_dec.identifier->word), share(ast_node));
		break;
	}

	case NODE_FUNCTION_CALL:
	{
		ast_node_o *args = ast_node->function_call.arguments;
		SMART frame_o *new_stack_frame = new_frame(frame);

		SMART array_o *evaluated_args = new_array(0);
		while (args)
		{
			check_return(frame, args);
			__orca_evaluate__(args, frame);
			array_push(evaluated_args, array_pop(frame->stack));
			args = args->next;
		}

		char *func_name = cstring(ast_node->function_call.identifier->word);

		if (strcmp(func_name, "print") == 0)
		{
			print(array_at(evaluated_args, 0));
		}
		else if (strcmp(func_name, "str") == 0)
		{
			string_o *sraso = str(array_at(evaluated_args, 0));
			array_push(frame->stack, sraso);
		}
		else
		{
			ast_node_o *func = resolve_variable(frame, ast_node->function_call.identifier->word);

			if (func == NULL)
			{
				string_o *err = string_format("Undefined function: %q", ast_node->function_call.identifier->word);
				error(cstring(err), PRINT | PANIC);
			}

			ast_node_o *params = func->function_dec.parameters;

			int i = 0;
			while (params != NULL && i < evaluated_args->length)
			{
				frame_set(new_stack_frame, share(params->parameter_dec.identifier->word), share(array_at(evaluated_args, i)));
				params = params->next;
				i++;
			}

			__orca_evaluate__(func->function_dec.body, new_stack_frame);

			ast_node_o *return_type = func->function_dec.return_type;

			if (return_type && strcmp(cstring(return_type->word), "void") == 0)
			{
				if (new_stack_frame->return_value)
				{
					SMART string_o *err = string_from("Function %s does not return a value", func_name);
					error(cstring(err), PRINT | PANIC);
				}
			}
			else
			{
				if (new_stack_frame->return_value == NULL)
				{
					SMART string_o *err = string_from("Function %s returned no value", func_name);
					error(cstring(err), PRINT | PANIC);
				}

				array_push(frame->stack, share(new_stack_frame->return_value));
			}
		}

		break;
	}

	case NODE_METHOD_CALL:
	{
		ast_node_o *args = ast_node->method_call.arguments;
		SMART frame_o *new_stack_frame = new_frame(frame);
		SMART array_o *evaluated_args = new_array(0);
		while (args)
		{
			__orca_evaluate__(args, frame);
			array_push(evaluated_args, array_pop(frame->stack));
			args = args->next;
		}

		object_o object = resolve_variable(frame, ast_node->method_call.object->word);

		switch (object_typeof(object))
		{
		case NT_ARRAY_O:
		{
			array_o *array = (array_o *)object;

			char *method = cstring(ast_node->method_call.identifier->word);
			if (strcmp(method, "push") == 0)
			{
				array_push(array, share(array_at(evaluated_args, 0)));
			}
			else if (strcmp(method, "pop") == 0)
			{
				array_push(frame->stack, array_pop(object));
			}
			else if (strcmp(method, "length") == 0)
			{
				array_push(frame->stack, number(array->length));
			}
			else if (strcmp(method, "now") == 0)
			{
				array_push(frame->stack, share(array_current(array)));
			}
			else if (strcmp(method, "end") == 0)
			{
				array_push(frame->stack, number(array_at_end(array)));
			}
			else if (strcmp(method, "next") == 0)
			{
				int a = array_next(array);
				if (a)
				{
					array_push(frame->stack, number(1));
				}
				else
				{
					array_push(frame->stack, error("Out of list", SILENT));
				}
			}
			else if (strcmp(method, "prev") == 0)
			{
				int b = array_prev(array);
				if (b)
				{
					array_push(frame->stack, number(1));
				}
				else
				{
					array_push(frame->stack, error("Out of list", SILENT));
				}
			}
			break;
		}
		default:
			break;
		}

		break;
	}

	case NODE_IMPORT:
	{
		htable_o *b = orca(share(ast_node->import_smt.path->string));

		string_o *name = htable_get(b, string_from("name"));

		if (name)
		{
			frame_set(frame, clone(name), b);
		}
		else
		{
			error("Export does not have a name", PRINT | PANIC);
		}

		break;
	}

	default:
		break;
	}
}

htable_o *orca_evaluate(ast_node_o *ast_node)
{
	frame_o *global_frame = new_frame(NULL);
	frame_set(global_frame, string_from("export"), new_htable(0));
	stack_push(global_frame);

	__orca_evaluate__(ast_node, global_frame);

	return share(frame_get(global_frame, string_from("export")));
}