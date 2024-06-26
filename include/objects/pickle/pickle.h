#pragma once
#ifndef DUMP_H
#define DUMP_H

#include <stdio.h>
#include <stdint.h>

#include "objects/object.h"
#include "dsa/tree/ntree.h"

#define ERROR_UNCLOSED_STRING (1 << 4)

typedef struct token_o token_o;

typedef enum
{
	TOKEN_EOL,
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_COMMA,
	TOKEN_COLON,
	TOKEN_GTHAN,
	TOKEN_LTHAN,
	TOKEN_MINUS,
	TOKEN_EQUALS,
	TOKEN_NUMBER,
	TOKEN_LBRACE,
	TOKEN_RBRACE,
	TOKEN_LPAREN,
	TOKEN_RPAREN,
	TOKEN_STRING,
	TOKEN_LBRACKET,
	TOKEN_RBRACKET,
	TOKEN_SEMICOLON,
	TOKEN_UNDERSCORE,
} token_type_e;

typedef enum
{
	NODE_PAIR,
	NODE_LIST,
	NODE_ARRAY,
	NODE_OBJECT,
	NODE_STRING,
	NODE_NUMBER,
	NODE_KEY_VALUE
} node_type_e;

typedef struct ast_node_o
{
	node_type_e node_type;
	union
	{
		struct
		{
			struct ast_node_o *members;
		} object;
		struct
		{
			struct ast_node_o *elements;
		} array;
		struct
		{
			struct ast_node_o *head;
		} list;
		struct
		{
			struct ast_node_o *key;
			struct ast_node_o *value;
		} key_value;
		long *number;
		string_o *string;
	};
	struct ast_node_o *next;
} ast_node_o;

string_o *dump(object_o object);

/*
lexer
*/
array_o *lexer(string_o *buffer);

/*
tokens
*/
token_o *new_token(token_type_e token_type, string_o *tkn);

string_o *token_get_token(token_o *token);
token_type_e token_get_type(token_o *token);

/*
parser
*/
object_o load(string_o *sraso);
ast_node_o *parse(array_o *tokens);
object_o evaluate(ast_node_o *ast_node);

/*
ast
*/
ast_node_o *new_ast_node(node_type_e node_type);
void ast_node_push(ast_node_o **root_node, ast_node_o *node);

#endif