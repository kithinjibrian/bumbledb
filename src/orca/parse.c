#include "orca/orca.h"
/*
ast_node_o *orca_parse_key(array_o *tokens);
ast_node_o *orca_parse_word(array_o *tokens);
ast_node_o *orca_parse_number(array_o *tokens);
ast_node_o *orca_parse_string(array_o *tokens);
ast_node_o *orca_parse_shift_exp(array_o *tokens);
ast_node_o *orca_parse_unary_exp(array_o *tokens);
ast_node_o *orca_parse_statement(array_o *tokens);
ast_node_o *orca_parse_parameter(array_o *tokens);
ast_node_o *orca_parse_key_value(array_o *tokens);
ast_node_o *orca_parse_expression(array_o *tokens);
ast_node_o *orca_parse_method_call(array_o *tokens);
ast_node_o *orca_parse_primary_exp(array_o *tokens);
ast_node_o *orca_parse_postfix_exp(array_o *tokens);
ast_node_o *orca_parse_equality_exp(array_o *tokens);
ast_node_o *orca_parse_additive_exp(array_o *tokens);
ast_node_o *orca_parse_variable_dec(array_o *tokens);
ast_node_o *orca_parse_array_access(array_o *tokens);
ast_node_o *orca_parse_function_dec(array_o *tokens);
ast_node_o *orca_parse_field_access(array_o *tokens);
ast_node_o *orca_parse_function_call(array_o *tokens);
ast_node_o *orca_parse_argument_list(array_o *tokens);
ast_node_o *orca_parse_array_literal(array_o *tokens);
ast_node_o *orca_parse_for_statement(array_o *tokens);
ast_node_o *orca_parse_relational_exp(array_o *tokens);
ast_node_o *orca_parse_statement_list(array_o *tokens);
ast_node_o *orca_parse_parameter_list(array_o *tokens);
ast_node_o *orca_parse_array_elements(array_o *tokens);
ast_node_o *orca_parse_object_literal(array_o *tokens);
ast_node_o *orca_parse_object_members(array_o *tokens);
ast_node_o *orca_parse_while_statement(array_o *tokens);
ast_node_o *orca_parse_argument_list_opt(array_o *tokens);
ast_node_o *orca_parse_multiplicative_exp(array_o *tokens);
ast_node_o *orca_parse_parameter_list_opt(array_o *tokens);
ast_node_o *orca_parse_compound_statement(array_o *tokens);
ast_node_o *orca_parse_selection_statement(array_o *tokens);
ast_node_o *orca_parse_type_annotation_opt(array_o *tokens);
ast_node_o *orca_parse_iteration_statement(array_o *tokens);
ast_node_o *orca_parse_expression_statement(array_o *tokens);
ast_node_o *orca_parse_assignment_expression(array_o *tokens);

bool match_type(array_o *tokens, token_type_e type)
{
	if (token_get_type(array_current(tokens)) == type)
	{
		array_next(tokens);
		return true;
	}
	SMART string_o *err = string_from("Expected token: %d", type);
	error(cstring(err), PRINT | PANIC);
	return false;
}

bool nmatch_type(array_o *tokens, token_type_e type)
{
	if (token_get_type(array_current(tokens)) == type)
	{
		return true;
	}
	return false;
}

bool match(array_o *tokens, char *str)
{
	token_o *current = array_current(tokens);

	if (strcmp(cstring(current->token), str) == 0)
	{
		array_next(tokens);
		return true;
	}

	SMART string_o *err = string_from("Unexpected token: %s", str);
	error(cstring(err), PRINT | PANIC);

	return false;
}

bool nmatch(array_o *tokens, char *str)
{
	token_o *current = array_current(tokens);

	if (strcmp(cstring(current->token), str) == 0)
	{
		return true;
	}

	return false;
}

ast_node_o *orca_parse(array_o *tokens)
{
	ast_node_o *node = new_ast_node(NODE_STATEMENTS);
	node->statements.list = orca_parse_statement_list(tokens);

	// printf("%d\n", node->statements.list->node_type);

	// print(array_current(tokens));
	return node;
}

ast_node_o *orca_parse_statement_list(array_o *tokens)
{
	ast_node_o *stmnt = orca_parse_statement(tokens);

	while (token_get_type(array_current(tokens)) != TOKEN_EOL)
	{
		ast_node_o *next_stmnt = orca_parse_statement(tokens);
		if (next_stmnt == NULL)
		{
			break;
		}
		ast_node_push(&stmnt, next_stmnt);
	}

	return stmnt;
}

ast_node_o *orca_parse_statement(array_o *tokens)
{

	if (nmatch(tokens, "let"))
	{
		return orca_parse_variable_dec(tokens);
	}
	else if (nmatch(tokens, "fun"))
	{
		return orca_parse_function_dec(tokens);
	}
	else if (nmatch(tokens, "if") || nmatch(tokens, "switch"))
	{
		return orca_parse_selection_statement(tokens);
	}
	else if (nmatch_type(tokens, TOKEN_LBRACE))
	{
		return orca_parse_compound_statement(tokens);
	}
	else if (nmatch(tokens, "while") || nmatch(tokens, "for"))
	{
		return orca_parse_iteration_statement(tokens);
	}
	else if (nmatch(tokens, "break"))
	{
		array_next(tokens);
		array_next(tokens);
		return new_ast_node(NODE_BREAK);
	}
	else if (nmatch(tokens, "continue"))
	{
		array_next(tokens);
		array_next(tokens);
		return new_ast_node(NODE_CONTINUE);
	}
	else if (nmatch(tokens, "return"))
	{
		ast_node_o *node = new_ast_node(NODE_RETURN);
		array_next(tokens);
		node->return_smt.exp = orca_parse_expression_statement(tokens);

		return node;
	}
	else if (nmatch(tokens, "import"))
	{
		ast_node_o *node = new_ast_node(NODE_IMPORT);
		array_next(tokens);
		node->import_smt.path = orca_parse_string(tokens);
		array_next(tokens);

		return node;
	}
	else
	{
		return orca_parse_expression_statement(tokens);
	}

	return NULL;
}

ast_node_o *orca_parse_expression_statement(array_o *tokens)
{
	ast_node_o *node = orca_parse_expression(tokens);

	if (nmatch_type(tokens, TOKEN_SEMICOLON))
	{
		array_next(tokens);
	}

	return node;
}

ast_node_o *orca_parse_expression(array_o *tokens)
{
	ast_node_o *node = orca_parse_assignment_expression(tokens);

	if (nmatch_type(tokens, TOKEN_COMMA))
	{
		array_next(tokens);
		ast_node_o *next = orca_parse_assignment_expression(tokens);
		ast_node_push(&node, next);
	}

	return node;
}

ast_node_o *orca_parse_assignment_expression(array_o *tokens)
{
	ast_node_o *node = orca_parse_equality_exp(tokens);

	if (nmatch_type(tokens, TOKEN_EQUALS))
	{
		ast_node_o *new_node = new_ast_node(NODE_BINARY_EXP);
		new_node->binary_exp.token_type = TOKEN_EQUALS;
		new_node->binary_exp.left = node;

		array_next(tokens);
		new_node->binary_exp.right = orca_parse_assignment_expression(tokens);

		return new_node;
	}

	return node;
}

ast_node_o *orca_parse_compound_statement(array_o *tokens)
{
	ast_node_o *node = NULL;

	match_type(tokens, TOKEN_LBRACE);

	if (token_get_type(array_current(tokens)) != TOKEN_RBRACE)
	{
		node = new_ast_node(NODE_COM_STATEMENTS);
		node->compound_statements.list = orca_parse_statement_list(tokens);
	}

	match_type(tokens, TOKEN_RBRACE);

	return node;
}

ast_node_o *orca_parse_selection_statement(array_o *tokens)
{
	ast_node_o *node = new_ast_node(NODE_IF_STATEMENT);
	array_next(tokens);

	match_type(tokens, TOKEN_LPAREN);
	node->if_statement.condition = orca_parse_equality_exp(tokens);
	match_type(tokens, TOKEN_RPAREN);

	node->if_statement.if_body = orca_parse_statement(tokens);

	if (nmatch(tokens, "else"))
	{
		array_next(tokens);
		node->if_statement.else_body = orca_parse_statement(tokens);
	}

	return node;
}

ast_node_o *orca_parse_iteration_statement(array_o *tokens)
{
	if (nmatch(tokens, "for"))
	{
		return orca_parse_for_statement(tokens);
	}
	else if (nmatch(tokens, "while"))
	{
		return orca_parse_while_statement(tokens);
	}
	return NULL;
}

ast_node_o *orca_parse_for_statement(array_o *tokens)
{
	ast_node_o *node = new_ast_node(NODE_FOR_STATEMENT);
	match(tokens, "for");
	match(tokens, "(");
	node->for_statement.init = orca_parse_statement(tokens);
	node->for_statement.condition = orca_parse_equality_exp(tokens);
	match(tokens, ";");
	node->for_statement.update = orca_parse_statement(tokens);
	match(tokens, ")");
	node->for_statement.body = orca_parse_statement(tokens);
	return node;
}

ast_node_o *orca_parse_while_statement(array_o *tokens)
{
	ast_node_o *node = new_ast_node(NODE_WHILE_STATEMENT);
	array_next(tokens);
	match_type(tokens, TOKEN_LPAREN);
	node->while_statement.condition = orca_parse_equality_exp(tokens);
	match_type(tokens, TOKEN_RPAREN);
	node->while_statement.body = orca_parse_statement(tokens);

	return node;
}

ast_node_o *orca_parse_function_dec(array_o *tokens)
{
	ast_node_o *function = new_ast_node(NODE_FUNCTION_DEC);
	array_next(tokens);
	function->function_dec.identifier = orca_parse_word(tokens);
	function->function_dec.parameters = orca_parse_parameter_list_opt(tokens);
	function->function_dec.return_type = orca_parse_type_annotation_opt(tokens);

	function->function_dec.body = orca_parse_compound_statement(tokens);

	return function;
}

ast_node_o *orca_parse_parameter_list_opt(array_o *tokens)
{
	ast_node_o *param_list = NULL;

	array_next(tokens);

	if (token_get_type(array_current(tokens)) != TOKEN_RPAREN)
	{
		param_list = orca_parse_parameter_list(tokens);
	}

	array_next(tokens);

	return param_list;
}

ast_node_o *orca_parse_parameter_list(array_o *tokens)
{
	ast_node_o *list = orca_parse_parameter(tokens);

	if (token_get_type(array_current(tokens)) == TOKEN_COMMA)
	{
		array_next(tokens);
		ast_node_o *next_list = orca_parse_parameter_list(tokens);
		ast_node_push(&list, next_list);
	}

	return list;
}

ast_node_o *orca_parse_parameter(array_o *tokens)
{
	ast_node_o *param = new_ast_node(NODE_PARAMETER_DEC);
	param->parameter_dec.identifier = orca_parse_word(tokens);
	param->parameter_dec.type = orca_parse_type_annotation_opt(tokens);

	return param;
}

ast_node_o *orca_parse_variable_dec(array_o *tokens)
{
	ast_node_o *variable = new_ast_node(NODE_VARIABLE);
	array_next(tokens);
	variable->variable_dec.identifier = orca_parse_word(tokens);
	variable->variable_dec.type = orca_parse_type_annotation_opt(tokens);

	if (token_get_type(array_current(tokens)) == TOKEN_EQUALS)
	{
		array_next(tokens);
		variable->variable_dec.initializer = orca_parse_equality_exp(tokens);
	}

	match_type(tokens, TOKEN_SEMICOLON);

	return variable;
}

ast_node_o *orca_parse_type_annotation_opt(array_o *tokens)
{
	token_o *current = array_current(tokens);

	if (current->token_type == TOKEN_COLON)
	{
		array_next(tokens);
		ast_node_o *type = orca_parse_word(tokens);
		if (token_get_type(array_current(tokens)) == TOKEN_LBRACKET)
		{
			array_next(tokens);
			if (token_get_type(array_current(tokens)) == TOKEN_RBRACKET)
			{
				array_next(tokens);
				return type;
			}
		}
		return type;
	}

	return NULL;
}

ast_node_o *orca_parse_equality_exp(array_o *tokens)
{
	ast_node_o *left = orca_parse_relational_exp(tokens);

	token_o *current = array_current(tokens);
	token_o *next = array_at(tokens, tokens->index + 1);

	while (current && ((current->token_type == TOKEN_EQUALS &&
						next->token_type == TOKEN_EQUALS)))
	{
		token_type_e token_type = TOKEN_IS_EQUAL;
		array_next(tokens);
		array_next(tokens);

		ast_node_o *right = orca_parse_relational_exp(tokens);

		ast_node_o *exp = new_ast_node(NODE_BINARY_EXP);
		exp->binary_exp.left = left;
		exp->binary_exp.right = right;
		exp->binary_exp.token_type = token_type;

		left = exp;

		current = array_current(tokens);
	}

	return left;
}

ast_node_o *orca_parse_relational_exp(array_o *tokens)
{
	ast_node_o *left = orca_parse_shift_exp(tokens);

	token_o *current = array_current(tokens);

	while (current && (current->token_type == TOKEN_GREATER ||
					   current->token_type == TOKEN_LESSER))
	{
		token_type_e token_type = current->token_type;
		array_next(tokens);

		ast_node_o *right = orca_parse_shift_exp(tokens);

		ast_node_o *exp = new_ast_node(NODE_BINARY_EXP);
		exp->binary_exp.left = left;
		exp->binary_exp.right = right;
		exp->binary_exp.token_type = token_type;

		left = exp;

		current = array_current(tokens);
	}

	return left;
}

ast_node_o *orca_parse_shift_exp(array_o *tokens)
{
	return orca_parse_additive_exp(tokens);
}

ast_node_o *orca_parse_additive_exp(array_o *tokens)
{
	ast_node_o *left = orca_parse_multiplicative_exp(tokens);

	token_o *current = array_current(tokens);

	while (current && (current->token_type == TOKEN_PLUS ||
					   current->token_type == TOKEN_MINUS))
	{
		token_type_e token_type = current->token_type;
		array_next(tokens);

		ast_node_o *right = orca_parse_multiplicative_exp(tokens);

		ast_node_o *exp = new_ast_node(NODE_BINARY_EXP);
		exp->binary_exp.left = left;
		exp->binary_exp.right = right;
		exp->binary_exp.token_type = token_type;

		left = exp;

		current = array_current(tokens);
	}

	return left;
}

ast_node_o *orca_parse_multiplicative_exp(array_o *tokens)
{
	ast_node_o *left = orca_parse_unary_exp(tokens);

	token_o *current = array_current(tokens);

	while (current && (current->token_type == TOKEN_MULT ||
					   current->token_type == TOKEN_DIVIDE ||
					   current->token_type == TOKEN_MODULO))
	{
		token_type_e token_type = current->token_type;
		array_next(tokens);

		ast_node_o *right = orca_parse_unary_exp(tokens);

		ast_node_o *exp = new_ast_node(NODE_BINARY_EXP);
		exp->binary_exp.left = left;
		exp->binary_exp.right = right;
		exp->binary_exp.token_type = token_type;

		left = exp;

		current = array_current(tokens);
	}

	return left;
}

ast_node_o *orca_parse_unary_exp(array_o *tokens)
{
	ast_node_o *node = orca_parse_postfix_exp(tokens);

	return node;
}

ast_node_o *orca_parse_postfix_exp(array_o *tokens)
{
	ast_node_o *node = orca_parse_primary_exp(tokens);

	while (1)
	{
		if (nmatch_type(tokens, TOKEN_PLUS))
		{
			array_next(tokens);
			if (nmatch_type(tokens, TOKEN_PLUS))
			{
				array_next(tokens);
				ast_node_o *exp = new_ast_node(NODE_POSTFIX_INC);
				exp->postfix_inc.identifier = node;
				node = exp;
			}
			else
			{
				array_prev(tokens);
				break;
			}
		}
		else if (nmatch_type(tokens, TOKEN_LBRACKET))
		{
			ast_node_o *array_access = new_ast_node(NODE_ARRAY_ACCESS);
			array_access->array_access.identifier = node;

			array_next(tokens);
			array_access->array_access.index_exp = orca_parse_expression(tokens);
			array_next(tokens);

			node = array_access;
		}
		else if (nmatch_type(tokens, TOKEN_LPAREN))
		{
			ast_node_o *fun_call = new_ast_node(NODE_FUNCTION_CALL);
			fun_call->function_call.identifier = node;

			array_next(tokens);
			fun_call->function_call.arguments = orca_parse_expression(tokens);
			array_next(tokens);

			node = fun_call;
		}
		else if (nmatch_type(tokens, TOKEN_EXCLAMATION))
		{
			array_next(tokens);

			ast_node_o *nt_fun_call = new_ast_node(NODE_NT_FUNCTION);
			nt_fun_call->nt_function_call.identifier = node;

			array_next(tokens);
			nt_fun_call->function_call.arguments = orca_parse_expression(tokens);
			array_next(tokens);

			node = nt_fun_call;
		}
		else if (nmatch_type(tokens, TOKEN_DOT))
		{
			ast_node_o *fa = new_ast_node(NODE_FIELD_ACCESS);
			fa->field_access.object = node;

			array_next(tokens);
			fa->field_access.identifier = orca_parse_word(tokens);

			node = fa;
		}
		else
		{
			break;
		}
	}
	return node;
}

ast_node_o *orca_parse_primary_exp(array_o *tokens)
{
	ast_node_o *node = NULL;
	token_o *token = array_current(tokens);

	switch (token->token_type)
	{
	case TOKEN_NUMBER:
		node = orca_parse_number(tokens);
		break;

	case TOKEN_STRING:
		node = orca_parse_string(tokens);
		break;

	case TOKEN_WORD:
		node = orca_parse_word(tokens);
		break;

	case TOKEN_LPAREN:
		node = orca_parse_equality_exp(tokens);
		break;

	case TOKEN_LBRACKET:
		node = orca_parse_array_literal(tokens);
		break;

	case TOKEN_LBRACE:
		node = orca_parse_object_literal(tokens);
		break;

	default:
		break;
	}

	return node;
}

ast_node_o *orca_parse_function_call(array_o *tokens)
{
	ast_node_o *ast_node = new_ast_node(NODE_FUNCTION_CALL);
	ast_node->function_call.identifier = orca_parse_word(tokens);

	array_next(tokens);
	ast_node->function_call.arguments = orca_parse_assignment_expression(tokens);
	array_next(tokens);

	return ast_node;
}

ast_node_o *orca_parse_method_call(array_o *tokens)
{
	ast_node_o *ast_node = new_ast_node(NODE_METHOD_CALL);
	ast_node->method_call.object = orca_parse_word(tokens);

	match_type(tokens, TOKEN_DOT);
	ast_node->method_call.identifier = orca_parse_word(tokens);
	match_type(tokens, TOKEN_LPAREN);
	ast_node->method_call.arguments = orca_parse_argument_list_opt(tokens);
	match_type(tokens, TOKEN_RPAREN);

	return ast_node;
}

ast_node_o *orca_parse_argument_list_opt(array_o *tokens)
{
	ast_node_o *ast_node = NULL;
	if (token_get_type(array_current(tokens)) != TOKEN_LPAREN)
	{
		return orca_parse_argument_list(tokens);
	}

	return ast_node;
}

ast_node_o *orca_parse_argument_list(array_o *tokens)
{
	ast_node_o *ast_node = orca_parse_assignment_expression(tokens);

	if (token_get_type(array_current(tokens)) == TOKEN_COMMA)
	{
		array_next(tokens);
		ast_node_o *next = orca_parse_argument_list(tokens);
		ast_node_push(&ast_node, next);
	}

	return ast_node;
}

ast_node_o *orca_parse_array_access(array_o *tokens)
{
	ast_node_o *node = orca_parse_word(tokens);
	token_o *current = array_current(tokens);
	while (current && current->token_type == TOKEN_LBRACKET)
	{
		array_next(tokens);

		ast_node_o *index = orca_parse_equality_exp(tokens);

		array_next(tokens);

		ast_node_o *new_node = new_ast_node(NODE_ARRAY_ACCESS);
		new_node->array_access.identifier = node;
		new_node->array_access.index_exp = index;

		node = new_node;

		current = array_current(tokens);
	}

	return node;
}

ast_node_o *orca_parse_field_access(array_o *tokens)
{
	ast_node_o *node = new_ast_node(NODE_FIELD_ACCESS);
	node->field_access.object = orca_parse_word(tokens);

	match_type(tokens, TOKEN_DOT);

	node->field_access.identifier = orca_parse_word(tokens);

	return node;
}

ast_node_o *orca_parse_array_literal(array_o *tokens)
{
	ast_node_o *node = new_ast_node(NODE_ARRAY);
	array_next(tokens);
	if (token_get_type(array_current(tokens)) != TOKEN_RBRACKET)
	{
		node->array.elements = orca_parse_array_elements(tokens);
	}
	array_next(tokens);

	return node;
}

ast_node_o *orca_parse_array_elements(array_o *tokens)
{
	ast_node_o *ast_node = orca_parse_equality_exp(tokens);

	if (token_get_type(array_current(tokens)) == TOKEN_COMMA)
	{
		array_next(tokens);
		ast_node_o *next = orca_parse_array_elements(tokens);
		ast_node_push(&ast_node, next);
	}

	return ast_node;
}

ast_node_o *orca_parse_object_literal(array_o *tokens)
{
	ast_node_o *node = new_ast_node(NODE_OBJECT);
	match_type(tokens, TOKEN_LBRACE);
	if (!(nmatch_type(tokens, TOKEN_RBRACE)))
	{
		node->object.members = orca_parse_object_members(tokens);
	}
	match_type(tokens, TOKEN_RBRACE);
	return node;
}

ast_node_o *orca_parse_object_members(array_o *tokens)
{
	ast_node_o *ast_node = orca_parse_key_value(tokens);

	if (nmatch_type(tokens, TOKEN_COMMA))
	{
		array_next(tokens);
		ast_node_o *next = orca_parse_object_members(tokens);
		ast_node_push(&ast_node, next);
	}

	return ast_node;
}

ast_node_o *orca_parse_key_value(array_o *tokens)
{
	ast_node_o *key_value = new_ast_node(NODE_KEY_VALUE);
	key_value->key_value.key = orca_parse_key(tokens);
	if (nmatch_type(tokens, TOKEN_COLON))
	{
		array_next(tokens);
		key_value->key_value.value = orca_parse_equality_exp(tokens);
	}
	return key_value;
}

ast_node_o *orca_parse_key(array_o *tokens)
{
	token_o *tkn = array_current(tokens);
	ast_node_o *ast_node = NULL;

	switch (token_get_type(tkn))
	{
	case TOKEN_STRING:
		ast_node = orca_parse_string(tokens);
		break;
	case TOKEN_NUMBER:
		ast_node = orca_parse_number(tokens);
		break;

	default:
		error("Invalid key", PRINT | PANIC);
		break;
	}

	return ast_node;
}

ast_node_o *orca_parse_word(array_o *tokens)
{
	token_o *tkn = array_current(tokens);

	ast_node_o *ast_node = new_ast_node(NODE_WORD);
	ast_node->word = share(token_get_token(tkn));

	array_next(tokens);

	return ast_node;
}

ast_node_o *orca_parse_string(array_o *tokens)
{
	token_o *tkn = array_current(tokens);

	ast_node_o *ast_node = new_ast_node(NODE_STRING);
	ast_node->string = share(token_get_token(tkn));

	array_next(tokens);

	return ast_node;
}

ast_node_o *orca_parse_number(array_o *tokens)
{
	token_o *tkn = array_current(tokens);

	ast_node_o *ast_node = new_ast_node(NODE_NUMBER);
	int num = atoi(cstring(token_get_token(tkn)));
	ast_node->number = number(num);

	array_next(tokens);

	return ast_node;
}*/