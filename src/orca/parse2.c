#include "orca/orca.h"

ast_node_o *statement(array_o *tokens);
ast_node_o *expression(array_o *tokens);
ast_node_o *native_function(array_o *tokens);
ast_node_o *function_expression(array_o *tokens);
ast_node_o *assignment_expression(array_o *tokens);

static array_o *errors_g = NULL;

array_o *parse_errors_get()
{
	if (errors_g == NULL)
	{
		errors_g = new_array(0);
		pool_globals_add(errors_g);
	}

	return errors_g;
}

array_o *push_error(int critical, token_o *token, string_o *err)
{
	array_o *errors = parse_errors_get();
	array_push(errors, array_from(2, token, err));
	if (critical)
	{
		print(errors);
		exit(1);
	}
	return errors;
}

bool check_token_type(array_o *tokens, token_type_e type)
{
	if (token_get_type(array_current(tokens)) == type)
	{
		return true;
	}
	return false;
}

bool check_token_value(array_o *tokens, char *value)
{
	token_o *current = array_current(tokens);

	if (strcmp(cstring(current->token), value) == 0)
	{
		return true;
	}

	return false;
}

void eat_token(array_o *tokens, char *value)
{
	if (check_token_value(tokens, value))
	{
		array_next(tokens);
	}
	else
	{
		array_prev(tokens);
		push_error(1, share(array_current(tokens)), string_from("Expected token: %s", value));
	}
}

ast_node_o *pstring(array_o *tokens)
{
	token_o *tkn = array_current(tokens);

	ast_node_o *ast_node = new_ast_node(NODE_STRING);
	ast_node->string = share(token_get_token(tkn));

	array_next(tokens);

	return ast_node;
}

ast_node_o *pnumber(array_o *tokens)
{
	token_o *tkn = array_current(tokens);

	ast_node_o *ast_node = new_ast_node(NODE_NUMBER);
	int num = atoi(cstring(token_get_token(tkn)));
	ast_node->number = number(num);

	array_next(tokens);

	return ast_node;
}

ast_node_o *identifier(array_o *tokens)
{
	token_o *tkn = array_current(tokens);

	ast_node_o *ast_node = new_ast_node(NODE_WORD);
	ast_node->word = share(token_get_token(tkn));

	array_next(tokens);

	return ast_node;
}

ast_node_o *key(array_o *tokens)
{
	token_o *tkn = array_current(tokens);
	ast_node_o *ast_node = NULL;

	switch (token_get_type(tkn))
	{
	case TOKEN_WORD:
		ast_node = identifier(tokens);
		break;
	case TOKEN_STRING:
		ast_node = pstring(tokens);
		break;
	case TOKEN_NUMBER:
		ast_node = pnumber(tokens);
		break;

	default:
		error("Invalid key", PRINT | PANIC);
		break;
	}

	return ast_node;
}

ast_node_o *key_value(array_o *tokens)
{
	ast_node_o *node = new_ast_node(NODE_KEY_VALUE);
	node->key_value.key = key(tokens);
	eat_token(tokens, ":");
	node->key_value.value = assignment_expression(tokens);

	return node;
}

ast_node_o *key_value_list(array_o *tokens)
{
	ast_node_o *node = key_value(tokens);

	if (check_token_type(tokens, TOKEN_COMMA))
	{
		array_next(tokens);
		ast_node_o *next = key_value_list(tokens);
		ast_node_push(&node, next);
	}

	return node;
}

ast_node_o *htable_literal(array_o *tokens)
{
	eat_token(tokens, "map");
	eat_token(tokens, "{");
	ast_node_o *node = new_ast_node(NODE_OBJECT);
	node->object.members = key_value_list(tokens);

	eat_token(tokens, "}");

	return node;
}

ast_node_o *array_literal(array_o *tokens)
{
	eat_token(tokens, "array");
	eat_token(tokens, "{");

	ast_node_o *node = new_ast_node(NODE_ARRAY);
	node->array.elements = assignment_expression(tokens);

	while (check_token_type(tokens, TOKEN_COMMA))
	{
		array_next(tokens);
		ast_node_o *next = assignment_expression(tokens);
		ast_node_push(&node->array.elements, next);
	}

	eat_token(tokens, "}");

	return node;
}

ast_node_o *list_literal(array_o *tokens)
{
	eat_token(tokens, "list");
	eat_token(tokens, "{");

	ast_node_o *node = new_ast_node(NODE_LIST);
	node->list.head = assignment_expression(tokens);

	while (check_token_type(tokens, TOKEN_COMMA))
	{
		array_next(tokens);
		ast_node_o *next = assignment_expression(tokens);
		ast_node_push(&node->list.head, next);
	}

	eat_token(tokens, "}");

	return node;
}

ast_node_o *struct_literal(array_o *tokens)
{
	token_o *struct_name = array_current(tokens);
	array_next(tokens);
	eat_token(tokens, "{");

	ast_node_o *node = new_ast_node(NODE_STRUCT_INSTANCE);
	node->struct_instance.fields = key_value_list(tokens);

	ast_node_o *name = new_ast_node(NODE_WORD);
	name->word = share(struct_name->token);
	node->struct_instance.identifier = name;

	eat_token(tokens, "}");

	return node;
}

ast_node_o *primary_expression(array_o *tokens)
{
	token_o *current = array_current(tokens);
	ast_node_o *node = NULL;

	switch (current->token_type)
	{
	case TOKEN_WORD:
	{
		if (check_token_value(tokens, "true"))
		{
			node = new_ast_node(NODE_NUMBER);
			node->number = number(1);
			array_next(tokens);
			break;
		}
		else if (check_token_value(tokens, "false"))
		{
			node = new_ast_node(NODE_NUMBER);
			node->number = number(0);
			array_next(tokens);
			break;
		}
		else if (check_token_value(tokens, "null"))
		{
			node = new_ast_node(NODE_NULL);
			array_next(tokens);
			break;
		}
		else if (check_token_value(tokens, "map"))
		{
			node = htable_literal(tokens);
			break;
		}
		else if (check_token_value(tokens, "list"))
		{
			node = list_literal(tokens);
			break;
		}
		else if (check_token_value(tokens, "array"))
		{
			node = array_literal(tokens);
			break;
		}

		token_o *next = array_at(tokens, tokens->index + 1);
		token_o *next_next = array_at(tokens, tokens->index + 2);

		if (next->token_type == TOKEN_EXCLAMATION)
		{
			if (next_next->token_type == TOKEN_LPAREN)
			{
				node = identifier(tokens);
				eat_token(tokens, "!");
				break;
			}
			node = native_function(tokens);
			break;
		}
		else if (next->token_type == TOKEN_LBRACE)
		{
			node = struct_literal(tokens);
			break;
		}

		node = identifier(tokens);
		break;
	}

	case TOKEN_STRING:
		node = pstring(tokens);
		break;

	case TOKEN_NUMBER:
		node = pnumber(tokens);
		break;

	case TOKEN_LPAREN:
		array_next(tokens);
		node = expression(tokens);
		eat_token(tokens, ")");
		break;

	case TOKEN_AMPERSAND:
	{
		token_o *next = array_at(tokens, tokens->index + 1);
		if (next->token_type == TOKEN_WORD)
		{
			node = new_ast_node(NODE_REFERENCE);
			node->word = share(next->token);
			array_next(tokens);
			array_next(tokens);
			break;
		}
	}

	default:
		// push_error(1, share(array_current(tokens)), string_from("Unexpected token: %s", cstring(current->token)));
		break;
	}

	return node;
}

ast_node_o *function_or_primary_expression(array_o *tokens)
{
	if (check_token_value(tokens, "fun"))
	{
		return function_expression(tokens);
	}

	return primary_expression(tokens);
}

ast_node_o *member_expression_part(array_o *tokens)
{
	if (check_token_type(tokens, TOKEN_DOT))
	{
		array_next(tokens);
		return identifier(tokens);
	}
	else if (check_token_type(tokens, TOKEN_LBRACKET))
	{
		eat_token(tokens, "[");
		ast_node_o *node = expression(tokens);
		eat_token(tokens, "]");
		return node;
	}

	return NULL;
}

ast_node_o *argument_list(array_o *tokens)
{
	ast_node_o *node = assignment_expression(tokens);

	if (check_token_type(tokens, TOKEN_COMMA))
	{
		array_next(tokens);
		ast_node_o *next = argument_list(tokens);
		ast_node_push(&node, next);
	}

	return node;
}

ast_node_o *arguments(array_o *tokens)
{

	ast_node_o *node = NULL;

	eat_token(tokens, "(");

	if (!check_token_type(tokens, TOKEN_RPAREN))
	{
		node = argument_list(tokens);
	}

	eat_token(tokens, ")");

	return node;
}

ast_node_o *native_function(array_o *tokens)
{
	ast_node_o *node = new_ast_node(NODE_NT_FUNCTION);
	node->nt_function_call.identifier = identifier(tokens);
	eat_token(tokens, "!");

	return node;
}

ast_node_o *call_expression(array_o *tokens)
{
	token_o *prev = array_at(tokens, tokens->index - 1);
	token_o *prev_prev = array_at(tokens, tokens->index - 2);

	ast_node_o *node = NULL;
	if (prev->token_type == TOKEN_EXCLAMATION && prev_prev->token_type == TOKEN_WORD)
	{
		node = new_ast_node(NODE_NT_FUNCTION);
		node->nt_function_call.arguments = arguments(tokens);
	}
	else
	{
		node = new_ast_node(NODE_FUNCTION_CALL);
		node->function_call.arguments = arguments(tokens);
		node->function_call.call_exp = NULL;
		while (check_token_type(tokens, TOKEN_DOT) ||
			   check_token_type(tokens, TOKEN_LBRACKET) ||
			   check_token_type(tokens, TOKEN_LPAREN))
		{
			if (check_token_type(tokens, TOKEN_LPAREN))
			{
				ast_node_push(&node->function_call.call_exp, arguments(tokens));
			}
			else if (check_token_type(tokens, TOKEN_LBRACKET))
			{
				eat_token(tokens, "[");
				ast_node_push(&node->function_call.call_exp, expression(tokens));
				eat_token(tokens, "]");
			}
			else if (check_token_type(tokens, TOKEN_DOT))
			{
				eat_token(tokens, ".");
				ast_node_push(&node->function_call.call_exp, identifier(tokens));
			}
		}
	}

	return node;
}

ast_node_o *member_expression(array_o *tokens)
{
	if (check_token_type(tokens, TOKEN_DOT) ||
		check_token_type(tokens, TOKEN_LBRACKET))
	{
		return member_expression_part(tokens);
	}

	return function_or_primary_expression(tokens);
}

ast_node_o *left_hand_expression(array_o *tokens)
{
	ast_node_o *node = member_expression(tokens);

	while (check_token_type(tokens, TOKEN_DOT) ||
		   check_token_type(tokens, TOKEN_LBRACKET) ||
		   check_token_type(tokens, TOKEN_LPAREN))
	{
		if (check_token_type(tokens, TOKEN_LPAREN))
		{
			ast_node_o *next = call_expression(tokens);
			next->function_call.identifier = node;
			node = next;
		}
		else
		{
			ast_node_o *next = member_expression(tokens);
			ast_node_push(&node, next);
		}
	}

	// ast_node_o *chain = new_ast_node(NODE_SUPER_STATEMENT);
	// chain->super_statement.statement = node;
	// chain->super_statement.type = LIST_LEFT_HAND;

	return node;
}

ast_node_o *post_fix_expression(array_o *tokens)
{
	ast_node_o *node = left_hand_expression(tokens);
	return node;
}

ast_node_o *unary_expression(array_o *tokens)
{
	ast_node_o *node = NULL;

	if (check_token_type(tokens, TOKEN_MULT))
	{
		array_next(tokens);
		node = new_ast_node(NODE_UNARY_EXP);
		node->unary_exp.operand = unary_expression(tokens);
	}
	else
	{
		node = post_fix_expression(tokens);
	}

	return node;
}

ast_node_o *multiplicative_expression(array_o *tokens)
{
	ast_node_o *left = unary_expression(tokens);

	token_o *current = array_current(tokens);

	while (current && (current->token_type == TOKEN_MULT ||
					   current->token_type == TOKEN_DIVIDE ||
					   current->token_type == TOKEN_MODULO))
	{
		array_next(tokens);
		ast_node_o *right = unary_expression(tokens);

		ast_node_o *exp = new_ast_node(NODE_BINARY_EXP);
		exp->binary_exp.left = left;
		exp->binary_exp.right = right;
		exp->binary_exp.token_type = current->token_type;

		left = exp;

		current = array_current(tokens);
	}

	return left;
}

ast_node_o *additive_expression(array_o *tokens)
{
	ast_node_o *left = multiplicative_expression(tokens);

	token_o *current = array_current(tokens);

	while (current && (current->token_type == TOKEN_PLUS || current->token_type == TOKEN_MINUS))
	{
		array_next(tokens);
		ast_node_o *right = multiplicative_expression(tokens);

		ast_node_o *exp = new_ast_node(NODE_BINARY_EXP);
		exp->binary_exp.left = left;
		exp->binary_exp.right = right;
		exp->binary_exp.token_type = current->token_type;

		left = exp;

		current = array_current(tokens);
	}

	return left;
}

ast_node_o *shift_expression(array_o *tokens)
{
	ast_node_o *left = additive_expression(tokens);

	return left;
}

ast_node_o *relational_expression(array_o *tokens)
{
	ast_node_o *left = shift_expression(tokens);

	token_o *current = array_current(tokens);
	token_o *next = array_at(tokens, tokens->index + 1);

	while (current && (current->token_type == TOKEN_GREATER ||
					   current->token_type == TOKEN_LESSER ||
					   (current->token_type == TOKEN_GREATER && next->token_type == TOKEN_EQUALS) ||
					   (current->token_type == TOKEN_LESSER && next->token_type == TOKEN_EQUALS)))
	{
		array_next(tokens);

		if (next->token_type == TOKEN_EQUALS)
		{
			array_next(tokens);
		}

		ast_node_o *right = shift_expression(tokens);

		ast_node_o *exp = new_ast_node(NODE_BINARY_EXP);
		exp->binary_exp.left = left;
		exp->binary_exp.right = right;

		if (next->token_type == TOKEN_EQUALS)
		{
			if (current->token_type == TOKEN_GREATER)
			{
				exp->binary_exp.token_type = TOKEN_GREATER_OR_EQUAL;
			}
			else if (current->token_type == TOKEN_LESSER)
			{
				exp->binary_exp.token_type = TOKEN_LESSER_OR_EQUAL;
			}
		}
		else
		{
			exp->binary_exp.token_type = current->token_type;
		}

		left = exp;

		current = array_current(tokens);
		next = array_at(tokens, tokens->index + 1);
	}

	return left;
}

ast_node_o *equality_expression(array_o *tokens)
{
	ast_node_o *left = relational_expression(tokens);

	token_o *current = array_current(tokens);
	token_o *next = array_at(tokens, tokens->index + 1);

	while (current && ((current->token_type == TOKEN_EQUALS || current->token_type == TOKEN_EXCLAMATION) &&
					   next->token_type == TOKEN_EQUALS))
	{
		array_next(tokens);
		array_next(tokens);

		ast_node_o *right = relational_expression(tokens);

		ast_node_o *exp = new_ast_node(NODE_BINARY_EXP);
		exp->binary_exp.left = left;
		exp->binary_exp.right = right;

		if (current->token_type == TOKEN_EQUALS)
			exp->binary_exp.token_type = TOKEN_IS_EQUAL;
		else
			exp->binary_exp.token_type = TOKEN_IS_NOT_EQUAL;

		left = exp;

		current = array_current(tokens);
		next = array_at(tokens, tokens->index + 1);
	}

	return left;
}

ast_node_o *bitwise_and_expression(array_o *tokens)
{
	ast_node_o *left = equality_expression(tokens);

	token_o *current = array_current(tokens);

	while (current && current->token_type == TOKEN_AMPERSAND)
	{
		array_next(tokens);
		ast_node_o *right = equality_expression(tokens);

		ast_node_o *exp = new_ast_node(NODE_BINARY_EXP);
		exp->binary_exp.left = left;
		exp->binary_exp.right = right;
		exp->binary_exp.token_type = TOKEN_AND;

		left = exp;

		current = array_current(tokens);
	}

	return left;
}

ast_node_o *bitwise_xor_expression(array_o *tokens)
{
	ast_node_o *left = bitwise_and_expression(tokens);

	token_o *current = array_current(tokens);

	while (current && current->token_type == TOKEN_CARET)
	{
		array_next(tokens);
		ast_node_o *right = bitwise_and_expression(tokens);

		ast_node_o *exp = new_ast_node(NODE_BINARY_EXP);
		exp->binary_exp.left = left;
		exp->binary_exp.right = right;
		exp->binary_exp.token_type = TOKEN_CARET;

		left = exp;

		current = array_current(tokens);
	}
	return left;
}

ast_node_o *bitwise_or_expression(array_o *tokens)
{
	ast_node_o *left = bitwise_xor_expression(tokens);

	token_o *current = array_current(tokens);

	while (current && current->token_type == TOKEN_PIPE)
	{
		array_next(tokens);
		ast_node_o *right = bitwise_xor_expression(tokens);

		ast_node_o *exp = new_ast_node(NODE_BINARY_EXP);
		exp->binary_exp.left = left;
		exp->binary_exp.right = right;
		exp->binary_exp.token_type = TOKEN_PIPE;

		left = exp;

		current = array_current(tokens);
	}

	return left;
}

ast_node_o *logical_and_expression(array_o *tokens)
{
	ast_node_o *left = bitwise_or_expression(tokens);

	token_o *current = array_current(tokens);
	token_o *next = array_at(tokens, tokens->index + 1);

	while (current && (current->token_type == TOKEN_AMPERSAND && next->token_type == TOKEN_AMPERSAND))
	{
		array_next(tokens);
		array_next(tokens);

		ast_node_o *right = bitwise_or_expression(tokens);

		ast_node_o *exp = new_ast_node(NODE_BINARY_EXP);
		exp->binary_exp.left = left;
		exp->binary_exp.right = right;
		exp->binary_exp.token_type = TOKEN_AND;

		left = exp;

		current = array_current(tokens);
		next = array_at(tokens, tokens->index + 1);
	}
	return left;
}

ast_node_o *logical_or_expression(array_o *tokens)
{
	ast_node_o *left = logical_and_expression(tokens);

	token_o *current = array_current(tokens);
	token_o *next = array_at(tokens, tokens->index + 1);

	while (current && (current->token_type == TOKEN_PIPE && next->token_type == TOKEN_PIPE))
	{
		array_next(tokens);
		array_next(tokens);

		ast_node_o *right = logical_and_expression(tokens);

		ast_node_o *exp = new_ast_node(NODE_BINARY_EXP);
		exp->binary_exp.left = left;
		exp->binary_exp.right = right;
		exp->binary_exp.token_type = TOKEN_OR;

		left = exp;

		current = array_current(tokens);
		next = array_at(tokens, tokens->index + 1);
	}

	return left;
}

ast_node_o *conditional_expression(array_o *tokens)
{
	ast_node_o *condition = logical_or_expression(tokens);

	if (check_token_type(tokens, TOKEN_QUESTION))
	{
		array_next(tokens);
		ast_node_o *if_body = assignment_expression(tokens);
		eat_token(tokens, ":");
		ast_node_o *else_body = assignment_expression(tokens);

		ast_node_o *exp = new_ast_node(NODE_TERNARY_EXP);
		exp->ternary_exp.condition = condition;
		exp->ternary_exp.if_body = if_body;
		exp->ternary_exp.else_body = else_body;
		condition = exp;
	}

	return condition;
}

ast_node_o *assignment_expression(array_o *tokens)
{
	int index = tokens->index;

	ast_node_o *node = unary_expression(tokens);

	if (check_token_type(tokens, TOKEN_EQUALS))
	{
		array_next(tokens);

		ast_node_o *right = assignment_expression(tokens);

		ast_node_o *exp = new_ast_node(NODE_BINARY_EXP);

		exp->binary_exp.left = node;
		exp->binary_exp.right = right;
		exp->binary_exp.token_type = TOKEN_EQUALS;

		node = exp;
	}
	else
	{
		DROP(node);
		tokens->index = index;

		node = conditional_expression(tokens);
	}

	return node;
}

ast_node_o *expression(array_o *tokens)
{
	ast_node_o *node = assignment_expression(tokens);
	while (check_token_type(tokens, TOKEN_COMMA))
	{
		array_next(tokens);
		ast_node_o *next = assignment_expression(tokens);
		ast_node_push(&node, next);
	}
	return node;
}

ast_node_o *expression_statement(array_o *tokens)
{
	ast_node_o *node = new_ast_node(NODE_SUPER_STATEMENT);
	node->super_statement.statement = expression(tokens);
	node->super_statement.type = STATEMENT_EXPRESSION;

	if (!check_token_type(tokens, TOKEN_SEMICOLON))
	{
		array_prev(tokens);
		push_error(1, share(array_current(tokens)), string_from("Expected a semicolon after expression statement"));
	}

	array_next(tokens);

	return node;
}

ast_node_o *type_annotation_opt(array_o *tokens)
{
	if (check_token_type(tokens, TOKEN_COLON))
	{
		array_next(tokens);
		ast_node_o *type = identifier(tokens);
		return type;
	}

	return NULL;
}

ast_node_o *initializer_opt(array_o *tokens)
{
	if (check_token_type(tokens, TOKEN_EQUALS))
	{
		array_next(tokens);
		return assignment_expression(tokens);
	}

	return NULL;
}

ast_node_o *declaration(array_o *tokens)
{
	ast_node_o *node = new_ast_node(NODE_VARIABLE);
	node->variable.identifier = identifier(tokens);
	node->variable.type = type_annotation_opt(tokens);
	node->variable.initializer = initializer_opt(tokens);

	return node;
}

ast_node_o *declaration_list(array_o *tokens)
{
	ast_node_o *node = declaration(tokens);

	while (check_token_type(tokens, TOKEN_COMMA))
	{
		array_next(tokens);
		ast_node_o *next = declaration(tokens);
		ast_node_push(&node, next);
	}
	return node;
}

ast_node_o *variable_statement(array_o *tokens)
{
	array_next(tokens);

	ast_node_o *variable = new_ast_node(NODE_SUPER_STATEMENT);
	variable->super_statement.statement = declaration_list(tokens);
	variable->super_statement.type = STATEMENT_VARIABLE;

	if (!check_token_type(tokens, TOKEN_SEMICOLON))
	{
		array_prev(tokens);
		push_error(1, share(array_current(tokens)), string_from("Expected a semicolon after variable declaration"));
	}

	array_next(tokens);

	return variable;
}

ast_node_o *continue_statement(array_o *tokens)
{
	array_next(tokens);

	if (!check_token_type(tokens, TOKEN_SEMICOLON))
	{
		array_prev(tokens);
		push_error(1, share(array_current(tokens)), string_from("Expected a semicolon after continue statement"));
	}

	array_next(tokens);

	ast_node_o *node = new_ast_node(NODE_CONTINUE);

	return node;
}

ast_node_o *break_statement(array_o *tokens)
{
	array_next(tokens);

	if (!check_token_type(tokens, TOKEN_SEMICOLON))
	{
		array_prev(tokens);
		push_error(1, share(array_current(tokens)), string_from("Expected a semicolon after break statement"));
	}

	array_next(tokens);

	ast_node_o *node = new_ast_node(NODE_BREAK);

	return node;
}

ast_node_o *return_statement(array_o *tokens)
{
	array_next(tokens);

	ast_node_o *node = new_ast_node(NODE_RETURN);

	if (!check_token_type(tokens, TOKEN_SEMICOLON))
	{
		node->return_smt.exp = expression(tokens);
	}

	if (!check_token_type(tokens, TOKEN_SEMICOLON))
	{
		array_prev(tokens);
		push_error(1, share(array_current(tokens)), string_from("Expected a semicolon after return statement"));
	}

	array_next(tokens);
	return node;
}

ast_node_o *for_statement(array_o *tokens)
{
	ast_node_o *node = new_ast_node(NODE_FOR_STATEMENT);
	eat_token(tokens, "for");
	eat_token(tokens, "(");
	node->for_statement.init = statement(tokens);
	node->for_statement.condition = expression(tokens);
	eat_token(tokens, ";");
	node->for_statement.update = expression(tokens);
	eat_token(tokens, ")");
	node->for_statement.body = statement(tokens);
	return node;
}

ast_node_o *while_statement(array_o *tokens)
{
	eat_token(tokens, "while");
	eat_token(tokens, "(");
	ast_node_o *condition = expression(tokens);
	eat_token(tokens, ")");
	ast_node_o *body = statement(tokens);

	ast_node_o *node = new_ast_node(NODE_WHILE_STATEMENT);
	node->while_statement.condition = condition;
	node->while_statement.body = body;

	return node;
}

ast_node_o *if_statement(array_o *tokens)
{
	eat_token(tokens, "if");
	eat_token(tokens, "(");
	ast_node_o *condition = expression(tokens);
	eat_token(tokens, ")");
	ast_node_o *if_body = statement(tokens);

	ast_node_o *node = new_ast_node(NODE_TERNARY_EXP);
	node->ternary_exp.type = 1;
	node->ternary_exp.condition = condition;
	node->ternary_exp.if_body = if_body;
	node->ternary_exp.else_body = NULL;

	if (check_token_value(tokens, "else"))
	{
		array_next(tokens);
		node->ternary_exp.else_body = statement(tokens);
	}

	return node;
}

ast_node_o *statement_list(array_o *tokens)
{
	ast_node_o *node = statement(tokens);
	if (node && !check_token_type(tokens, TOKEN_RBRACE))
	{
		ast_node_o *next = statement_list(tokens);
		ast_node_push(&node, next);
		return node;
	}

	return node;
}

ast_node_o *block_statement(array_o *tokens)
{
	ast_node_o *node = NULL;

	eat_token(tokens, "{");
	if (!check_token_type(tokens, TOKEN_RBRACE))
	{

		node = new_ast_node(NODE_SUPER_STATEMENT);
		node->super_statement.statement = statement_list(tokens);
		node->super_statement.type = STATEMENT_BLOCK;
	}

	eat_token(tokens, "}");

	return node;
}

ast_node_o *import_statement(array_o *tokens)
{
	eat_token(tokens, "import");
	ast_node_o *node = new_ast_node(NODE_IMPORT);
	node->import_smt.path = identifier(tokens);

	eat_token(tokens, ";");

	return node;
}

ast_node_o *parameter(array_o *tokens)
{
	ast_node_o *node = new_ast_node(NODE_VARIABLE);
	node->variable.identifier = identifier(tokens);
	node->variable.type = type_annotation_opt(tokens);
	node->variable.initializer = initializer_opt(tokens);

	return node;
}

ast_node_o *parameter_list(array_o *tokens)
{
	ast_node_o *node = parameter(tokens);

	if (check_token_type(tokens, TOKEN_COMMA))
	{
		array_next(tokens);
		ast_node_o *next = parameter_list(tokens);
		ast_node_push(&node, next);
		return node;
	}

	return node;
}

ast_node_o *member_declaration(array_o *tokens)
{
	ast_node_o *node = new_ast_node(NODE_FUNCTION_DEC);

	node->function_dec.identifier = identifier(tokens);

	eat_token(tokens, "(");

	if (!check_token_type(tokens, TOKEN_RPAREN))
	{
		node->function_dec.parameters = parameter_list(tokens);
	}

	eat_token(tokens, ")");

	node->function_dec.return_type = type_annotation_opt(tokens);
	node->function_dec.body = block_statement(tokens);

	return node;
}

ast_node_o *struct_body(array_o *tokens)
{
	ast_node_o *node = NULL;
	token_o *next = array_at(tokens, tokens->index + 1);
	while (!check_token_type(tokens, TOKEN_RBRACE))
	{
		if (next->token_type == TOKEN_LPAREN)
		{
			ast_node_push(&node, member_declaration(tokens));
		}
		else
		{
			ast_node_push(&node, declaration(tokens));
			eat_token(tokens, ";");
		}

		next = array_at(tokens, tokens->index + 1);
	}

	return node;
}

ast_node_o *struct_statement(array_o *tokens)
{
	eat_token(tokens, "struct");
	ast_node_o *node = new_ast_node(NODE_STRUCT);
	node->struct_statement.identifier = identifier(tokens);
	eat_token(tokens, "{");
	node->struct_statement.body = struct_body(tokens);
	eat_token(tokens, "}");
	eat_token(tokens, ";");
	return node;
}

ast_node_o *statement(array_o *tokens)
{
	if (check_token_value(tokens, "{"))
	{
		return block_statement(tokens);
	}
	else if (check_token_value(tokens, "break"))
	{
		return break_statement(tokens);
	}
	else if (check_token_value(tokens, "continue"))
	{
		return continue_statement(tokens);
	}
	else if (check_token_value(tokens, "return"))
	{
		return return_statement(tokens);
	}
	else if (check_token_value(tokens, "let"))
	{
		return variable_statement(tokens);
	}
	else if (check_token_value(tokens, "if"))
	{
		return if_statement(tokens);
	}
	else if (check_token_value(tokens, "while"))
	{
		return while_statement(tokens);
	}
	else if (check_token_value(tokens, "for"))
	{
		return for_statement(tokens);
	}
	else if (check_token_value(tokens, "import"))
	{
		return import_statement(tokens);
	}
	else if (check_token_value(tokens, "struct"))
	{
		return struct_statement(tokens);
	}
	else if (check_token_type(tokens, TOKEN_COMMENT))
	{
		array_next(tokens);
		return NULL;
	}
	else
	{
		return expression_statement(tokens);
	}

	return NULL;
}

ast_node_o *function_expression(array_o *tokens)
{
	ast_node_o *node = new_ast_node(NODE_FUNCTION_DEC);

	eat_token(tokens, "fun");

	if (!check_token_type(tokens, TOKEN_LPAREN))
	{
		node->function_dec.identifier = identifier(tokens);
	}

	eat_token(tokens, "(");

	if (!check_token_type(tokens, TOKEN_RPAREN))
	{
		node->function_dec.parameters = parameter_list(tokens);
	}

	eat_token(tokens, ")");

	node->function_dec.return_type = type_annotation_opt(tokens);
	node->function_dec.body = block_statement(tokens);

	return node;
}

ast_node_o *function_declaration(array_o *tokens)
{
	ast_node_o *node = new_ast_node(NODE_FUNCTION_DEC);

	eat_token(tokens, "fun");

	node->function_dec.identifier = identifier(tokens);

	eat_token(tokens, "(");

	if (!check_token_type(tokens, TOKEN_RPAREN))
	{
		node->function_dec.parameters = parameter_list(tokens);
	}

	eat_token(tokens, ")");

	node->function_dec.return_type = type_annotation_opt(tokens);
	node->function_dec.body = block_statement(tokens);

	return node;
}

ast_node_o *source_element(array_o *tokens)
{
	if (check_token_value(tokens, "fun"))
	{
		return function_declaration(tokens);
	}

	return statement(tokens);
}

ast_node_o *source_elements(array_o *tokens)
{
	ast_node_o *node = source_element(tokens);

	while (!check_token_type(tokens, TOKEN_EOL))
	{
		ast_node_o *next = source_element(tokens);

		if (next == NULL)
		{
			break;
		}

		ast_node_push(&node, next);
	}

	return node;
}

ast_node_o *orca_parse2(array_o *tokens)
{
	if (tokens == NULL)
	{
		return NULL;
	}

	ast_node_o *program = new_ast_node(NODE_SUPER_STATEMENT);
	program->super_statement.statement = source_elements(tokens);
	program->super_statement.type = STATEMENT_PROGRAM;

	return program;
}