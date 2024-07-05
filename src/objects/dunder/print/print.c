#include "objects/dunder/print.h"

void __print__(ast_node_o *ast_node, int depth)
{
	char space[depth];
	memset(space, ' ', depth);
	space[depth] = '\0';

	switch (ast_node->node_type)
	{
	case NODE_NUMBER:
	{
		printf("%ld", (*(long *)ast_node->number));
		break;
	}
	case NODE_STRING:
	{
		printf(YELLOW "\"%s\"" RESET, cstring(ast_node->string));
		break;
	}
	case NODE_KEY_VALUE:
	{
		__print__(ast_node->key_value.key, depth + 1);
		printf(" : ");
		__print__(ast_node->key_value.value, depth + 1);
		break;
	}
	case NODE_LIST:
	{
		ast_node_o *current = ast_node->list.head;
		printf(GREEN "( " RESET);
		while (current)
		{
			__print__(current, depth + 1);
			if (current->next)
			{
				printf(BLUE ", " RESET);
			}
			current = current->next;
		}
		printf(GREEN " )" RESET);
		break;
	}
	case NODE_ARRAY:
	{
		ast_node_o *current = ast_node->array.elements;
		printf(GREEN "[" RESET);
		while (current)
		{
			__print__(current, depth + 1);

			if (current->next)
			{
				printf(BLUE ", " RESET);
			}

			current = current->next;
		}
		printf(GREEN "]" RESET);
		break;
	}
	case NODE_OBJECT:
	{
		ast_node_o *current = ast_node->object.members;

		printf(GREEN "{\n" RESET);

		while (current)
		{
			printf("%s", space);

			__print__(current->key_value.key, depth + 1);

			printf(" : ");

			__print__(current->key_value.value, depth + 1);

			if (current->next)
			{
				printf(BLUE ",\n" RESET);
			}

			current = current->next;
		}

		printf(GREEN "\n%s}" RESET, space);
		break;
	}
	default:
		printf("%d\n", ast_node->node_type);
		break;
	}
}

void print(object_o object)
{
	SMART string_o *s = str(object);
	printf("%s\n", cstring(s));
}

void pretty_print(object_o object)
{
	if (object == NULL)
	{
		printf("null\n");
		return;
	}

	SMART array_o *tokens = NULL;
	if (object_typeof(object) == NT_STRING_O)
	{
		tokens = lexer(string_format("\"%q\"", object));
	}
	else
	{
		tokens = lexer(str(object));

		if (is_error(tokens))
		{
			return;
		}
	}

	SMART ast_node_o *ast = parse(tokens);

	if (is_error(ast))
	{
		return;
	}

	__print__(ast, 0);
	printf("\n");
}