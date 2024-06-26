#include "objects/pickle/pickle.h"

void ast_node_print(object_o object, int depth)
{
	ast_node_o *ast_node = (ast_node_o *)object;
	if (object == NULL)
		return;

	switch (ast_node->node_type)
	{
	case NODE_NUMBER:
		//print_offset(ast_node->number, depth + 1);
		break;

	case NODE_STRING:
		//print_offset(ast_node->string, depth + 1);
		break;

	case NODE_KEY_VALUE:
		//print_offset(ast_node->key_value.key, depth + 1);
		printf(" : ");
		ast_node_print(ast_node->key_value.value, depth + 1);
		break;

	case NODE_LIST:
	case NODE_ARRAY:
	case NODE_OBJECT:
	{
		ast_node_o *current = NULL;
		if (ast_node->node_type == NODE_ARRAY)
		{
			current = ast_node->array.elements;
		}
		else if (ast_node->node_type == NODE_LIST)
		{
			current = ast_node->list.head;
		}
		else
		{
			current = ast_node->object.members;
		}
		while (current)
		{
			ast_node_print(current, depth + 1);
			current = current->next;
		}
	}
	break;

	default:
		break;
	}
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

	case NODE_NUMBER:
		post_visit(ast_node->number, fun_process);
		break;

	case NODE_KEY_VALUE:
		post_visit(ast_node->key_value.key, fun_process);
		post_visit(ast_node->key_value.value, fun_process);
		break;

	case NODE_LIST:
	case NODE_ARRAY:
	case NODE_OBJECT:
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

	default:
		break;
	}

	static const vtable_t vt = {
		.__print__ = ast_node_print,
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