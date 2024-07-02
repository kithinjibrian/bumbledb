#include "objects/pickle/pickle.h"

ast_node_o *parse_key(array_o *tokens);
ast_node_o *parse_list(array_o *tokens);
ast_node_o *parse_value(array_o *tokens);
ast_node_o *parse_array(array_o *tokens);
ast_node_o *parse_values(array_o *tokens);
ast_node_o *parse_object(array_o *tokens);
ast_node_o *parse_string(array_o *tokens);
ast_node_o *parse_number(array_o *tokens);
ast_node_o *parse_keyvalue(array_o *tokens);

/*
<start>		::= <value>
<value>		::= <string> | <number> | <keyvalue> | <object> | <array> | <list>
<keyvalue>	::= <key> ":" <value>
<key>       ::= <string> | <number>
<object>	::= "{" <values> "}"
<array>		::= "[" <values> "]"
<list>		::= "(" <values> ")"
<values>	::= <value> | <value> "," <values>
*/

ast_node_o *parse(array_o *tokens)
{
	return parse_value(tokens);
}

ast_node_o *parse_object(array_o *tokens)
{
	if (token_get_type(array_current(tokens)) != TOKEN_LBRACE)
	{
		return error("Expected '{'", SILENT);
	}

	array_next(tokens);

	ast_node_o *ast_node = new_ast_node(NODE_OBJECT);
	ast_node_o *keyvalue = parse_values(tokens);

	if (is_error(keyvalue))
	{
		DROP(ast_node);
		return keyvalue;
	}

	if (keyvalue->node_type != NODE_KEY_VALUE)
	{
		DROP(keyvalue);
		DROP(ast_node);
		return error("Expected key-value pair", SILENT);
	}

	ast_node->object.members = keyvalue;

	if (token_get_type(array_current(tokens)) != TOKEN_RBRACE)
	{
		DROP(ast_node);
		return error("Expected '}'", SILENT);
	}

	array_next(tokens);

	return ast_node;
}

ast_node_o *parse_array(array_o *tokens)
{

	if (token_get_type(array_current(tokens)) != TOKEN_LBRACKET)
	{
		return error("Expected '['", SILENT);
	}

	array_next(tokens);
	ast_node_o *ast_node = new_ast_node(NODE_ARRAY);
	object_o values = parse_values(tokens);

	if (is_error(values))
	{
		DROP(ast_node);
		return values;
	}

	if (token_get_type(array_current(tokens)) != TOKEN_RBRACKET)
	{
		DROP(values);
		DROP(ast_node);
		return error("Expected ']'", SILENT);
	}

	ast_node->array.elements = values;

	array_next(tokens);

	return ast_node;
}

ast_node_o *parse_list(array_o *tokens)
{

	if (token_get_type(array_current(tokens)) != TOKEN_LPAREN)
	{
		return error("Expected '('", SILENT);
	}

	array_next(tokens);
	ast_node_o *ast_node = new_ast_node(NODE_LIST);
	object_o head = parse_values(tokens);

	if (is_error(head))
	{
		DROP(ast_node);
		return head;
	}

	if (token_get_type(array_current(tokens)) != TOKEN_RPAREN)
	{
		DROP(head);
		DROP(ast_node);
		return error("Expected ')'", SILENT);
	}

	ast_node->list.head = head;

	array_next(tokens);
	return ast_node;
}

ast_node_o *parse_values(array_o *tokens)
{
	ast_node_o *ast_node = parse_value(tokens);

	if (token_get_type(array_current(tokens)) == TOKEN_COMMA)
	{
		array_next(tokens);
		ast_node_o *next_pairs = parse_values(tokens);
		ast_node_push(&ast_node, next_pairs);
	}

	return ast_node;
}

ast_node_o *parse_value(array_o *tokens)
{
	token_o *current = array_current(tokens);
	token_o *ahead = array_at(tokens, tokens->index + 1);

	ast_node_o *ast_node = NULL;

	if (token_get_type(ahead) == TOKEN_COLON)
	{
		ast_node = parse_keyvalue(tokens);
	}
	else
	{
		switch (token_get_type(current))
		{
		case TOKEN_WORD:
		case TOKEN_STRING:
			ast_node = parse_string(tokens);
			break;

		case TOKEN_NUMBER:
			ast_node = parse_number(tokens);
			break;

		case TOKEN_LBRACE:
			ast_node = parse_object(tokens);
			break;

		case TOKEN_LBRACKET:
			ast_node = parse_array(tokens);
			break;

		case TOKEN_LPAREN:
			ast_node = parse_list(tokens);
			break;

		default:
			ast_node = error("Expected a value!", SILENT);
			break;
		}
	}

	return ast_node;
}

ast_node_o *parse_keyvalue(array_o *tokens)
{
	object_o key = parse_key(tokens);

	ast_node_o *ast_node = new_ast_node(NODE_KEY_VALUE);
	ast_node->key_value.key = key;

	if (token_get_type(array_current(tokens)) != TOKEN_COLON)
	{
		return error("fun parse_keyvalue(): Expected ':'.\n", SILENT);
	}

	array_next(tokens);
	object_o value = parse_value(tokens);

	if (is_error(value))
	{
		DROP(ast_node);
		return value;
	}

	ast_node->key_value.value = value;

	return ast_node;
}

ast_node_o *parse_key(array_o *tokens)
{
	ast_node_o *ast_node = NULL;
	token_o *current = array_current(tokens);

	switch (token_get_type(current))
	{
	case TOKEN_WORD:
	case TOKEN_STRING:
		ast_node = parse_string(tokens);
		break;

	case TOKEN_NUMBER:
		ast_node = parse_number(tokens);
		break;

	default:
		break;
	}

	return ast_node;
}

ast_node_o *parse_string(array_o *tokens)
{
	token_o *tkn = array_current(tokens);

	ast_node_o *ast_node = new_ast_node(NODE_STRING);
	ast_node->string = share(token_get_token(tkn));

	array_next(tokens);

	return ast_node;
}

ast_node_o *parse_number(array_o *tokens)
{
	token_o *tkn = array_current(tokens);

	ast_node_o *ast_node = new_ast_node(NODE_NUMBER);
	int num = atoi(cstring(token_get_token(tkn)));
	ast_node->number = number(num);

	array_next(tokens);

	return ast_node;
}