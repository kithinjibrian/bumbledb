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
	TOKEN_OR,
	TOKEN_AND,
	TOKEN_DOT,
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_PLUS,
	TOKEN_MULT,
	TOKEN_COMMA,
	TOKEN_COLON,
	TOKEN_CARET,
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
	TOKEN_COMMENT,
	TOKEN_LBRACKET,
	TOKEN_RBRACKET,
	TOKEN_IS_EQUAL,
	TOKEN_QUESTION,
	TOKEN_AMPERSAND,
	TOKEN_SEMICOLON,
	TOKEN_UNDERSCORE,
	TOKEN_EXCLAMATION,
	TOKEN_IS_NOT_EQUAL,
	TOKEN_LESSER_OR_EQUAL,
	TOKEN_GREATER_OR_EQUAL,
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
	NODE_NULL,
	NODE_ARRAY,
	NODE_BREAK,
	NODE_BLOCK,
	NODE_OBJECT,
	NODE_STRING,
	NODE_NUMBER,
	NODE_RETURN,
	NODE_IMPORT,
	NODE_STRUCT,
	NODE_PROGRAM,
	NODE_VARIABLE,
	NODE_CONTINUE,
	NODE_KEY_VALUE,
	NODE_ARGUMENTS,
	NODE_REFERENCE,
	NODE_UNARY_EXP,
	NODE_BINARY_EXP,
	NODE_STATEMENTS,
	NODE_METHOD_CALL,
	NODE_POSTFIX_INC,
	NODE_DEREFERENCE,
	NODE_EXPRESSION,
	NODE_NT_FUNCTION,
	NODE_TERNARY_EXP,
	NODE_FUNCTION_DEC,
	NODE_ARRAY_ACCESS,
	NODE_FIELD_ACCESS,
	NODE_IF_STATEMENT,
	NODE_PARAMETER_DEC,
	NODE_FUNCTION_CALL,
	NODE_FOR_STATEMENT,
	NODE_COM_STATEMENTS,
	NODE_WHILE_STATEMENT,
	NODE_SOURCE_ELEMENTS,
	NODE_SUPER_STATEMENT,
	NODE_STRUCT_INSTANCE,
} node_type_e;

typedef enum
{
	LIST_ARGUMENT = 1000,
	LIST_LEFT_HAND,
	STATEMENT_BLOCK,
	SOURCE_ELEMENTS,
	STATEMENT_PROGRAM,
	STATEMENT_VARIABLE,
	STATEMENT_EXPRESSION,
} statement_type_e;

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
			struct ast_node_o *sources;
		} program;
		struct
		{
			struct ast_node_o *stmts;
		} block_statement;
		struct
		{
			statement_type_e type;
			struct ast_node_o *statement;
		} super_statement;
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
			struct ast_node_o *body;
			struct ast_node_o *identifier;
		} struct_statement;
		struct
		{
			struct ast_node_o *identifier;
			struct ast_node_o *fields;
		} struct_instance;
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
			object_o value;
			struct ast_node_o *type;
			struct ast_node_o *identifier;
			struct ast_node_o *initializer;
		} variable;
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
			struct ast_node_o *call_exp;
		} function_call;
		struct
		{
			struct ast_node_o *arguments;
			struct ast_node_o *identifier;
		} nt_function_call;
		struct
		{
			struct ast_node_o *child;
			struct ast_node_o *parent;
		} field_access;
		struct
		{
			struct ast_node_o *parent;
			struct ast_node_o *expression;
		} array_access;
		struct
		{
			struct ast_node_o *operand;
		} unary_exp;
		struct
		{
			token_type_e token_type;
			struct ast_node_o *right;
			struct ast_node_o *left;
		} binary_exp;
		struct
		{
			int type;
			struct ast_node_o *condition;
			struct ast_node_o *if_body;
			struct ast_node_o *else_body;
		} ternary_exp;

		long *number;
		string_o *word;
		string_o *string;
	};
	struct ast_node_o *next;
} ast_node_o;

typedef object_o (*fun_ast_str)(ast_node_o *);
typedef void (*fun_ast_visit)(ast_node_o *, fun_process_t);

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
void drop_ast_table();
object_o ast_node_str(object_o object);
object_o ast_str(ast_node_o *ast_node);
ast_node_o *new_ast_node(node_type_e node_type);
void ast_node_push(ast_node_o **root_node, ast_node_o *node);
void ast_visit(ast_node_o *ast_node, fun_process_t fun_process);

#endif