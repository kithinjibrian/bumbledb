#pragma once
#ifndef PICKLE_H
#define PICKLE_H

#include <stdio.h>
#include <stdint.h>

#include "objects/pool/pool.h"
#include "objects/pool/alloc.h"
#include "objects/owner/owner.h"
#include "objects/natives/arrays.h"
#include "objects/natives/strings.h"

#define ERROR_UNCLOSED_STRING (1 << 4)

typedef struct array_o array_o;

typedef struct lexer_o
{
	int line;
	int column;
	string_o *input;
} lexer_o;

typedef enum
{
	TOKEN_EOL,
	TOKEN_DOT,
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_PLUS,
	TOKEN_MULT,
	TOKEN_COMMA,
	TOKEN_COLON,
	TOKEN_LESSER,
	TOKEN_MINUS,
	TOKEN_DIVIDE,
	TOKEN_MODULO,
	TOKEN_EQUALS,
	TOKEN_NUMBER,
	TOKEN_LBRACE,
	TOKEN_RBRACE,
	TOKEN_LPAREN,
	TOKEN_RPAREN,
	TOKEN_STRING,
	TOKEN_GREATER,
	TOKEN_LBRACKET,
	TOKEN_RBRACKET,
	TOKEN_IS_EQUAL,
	TOKEN_SEMICOLON,
	TOKEN_UNDERSCORE,
} token_type_e;

typedef struct token_o
{
	int line;
	int column;
	string_o *token;
	token_type_e token_type;
} token_o;

typedef enum
{
	NODE_PAIR,
	NODE_LIST,
	NODE_WORD,
	NODE_ARRAY,
	NODE_BREAK,
	NODE_OBJECT,
	NODE_STRING,
	NODE_NUMBER,
	NODE_RETURN,
	NODE_IMPORT,
	NODE_CONTINUE,
	NODE_KEY_VALUE,
	NODE_ARGUMENTS,
	NODE_BINARY_EXP,
	NODE_STATEMENTS,
	NODE_METHOD_CALL,
	NODE_POSTFIX_INC,
	NODE_VARIABLE_DEC,
	NODE_FUNCTION_DEC,
	NODE_ARRAY_ACCESS,
	NODE_FIELD_ACCESS,
	NODE_IF_STATEMENT,
	NODE_PARAMETER_DEC,
	NODE_FUNCTION_CALL,
	NODE_FOR_STATEMENT,
	NODE_COM_STATEMENTS,
	NODE_WHILE_STATEMENT,
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
		struct
		{
			struct ast_node_o *path;
		} import_smt;
		struct
		{
			struct ast_node_o *exp;
		} return_smt;
		struct
		{
			struct ast_node_o *identifier;
		} postfix_inc;
		struct
		{
			struct ast_node_o *list;
		} statements;
		struct
		{
			struct ast_node_o *list;
		} compound_statements;
		struct
		{
			struct ast_node_o *condition;
			struct ast_node_o *if_body;
			struct ast_node_o *else_body;
		} if_statement;
		struct
		{
			struct ast_node_o *body;
			struct ast_node_o *condition;
		} while_statement;
		struct
		{
			struct ast_node_o *body;
			struct ast_node_o *init;
			struct ast_node_o *condition;
			struct ast_node_o *update;
		} for_statement;
		struct
		{
			struct ast_node_o *args;
		} arguments;
		struct
		{
			struct ast_node_o *identifier;
			struct ast_node_o *index_exp;
		} array_access;
		struct
		{
			struct ast_node_o *object;
			struct ast_node_o *identifier;
		} field_access;
		struct
		{
			struct ast_node_o *value;
			struct ast_node_o *index_exp;
			struct ast_node_o *identifier;
		} array_assign;
		struct
		{
			struct ast_node_o *type;
			struct ast_node_o *identifier;
			struct ast_node_o *initializer;
		} variable_dec;
		struct
		{
			struct ast_node_o *type;
			struct ast_node_o *identifier;
			struct ast_node_o *initializer;
		} parameter_dec;
		struct
		{
			struct ast_node_o *body;
			struct ast_node_o *parameters;
			struct ast_node_o *identifier;
			struct ast_node_o *return_type;
		} function_dec;
		struct
		{
			struct ast_node_o *arguments;
			struct ast_node_o *identifier;
		} function_call;
		struct
		{
			struct ast_node_o *object;
			struct ast_node_o *arguments;
			struct ast_node_o *identifier;
		} method_call;
		struct
		{
			token_type_e token_type;
			struct ast_node_o *right;
			struct ast_node_o *left;
		} binary_exp;

		long *number;
		string_o *word;
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
token_o *new_token(token_type_e token_type, string_o *tkn, int line, int column);

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