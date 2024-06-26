#include "objects/pickle/pickle.h"

object_o evaluate(ast_node_o *ast_node)
{
	switch (ast_node->node_type)
	{
	case NODE_STRING:
	{
		return share(ast_node->string);
	}
	case NODE_NUMBER:
	{
		return share(ast_node->number);
	}
	case NODE_KEY_VALUE:
	{
		ast_node_o *key = ast_node->key_value.key;
		ast_node_o *value = ast_node->key_value.value;
		return new_keyvalue(evaluate(key), evaluate(value));
	}
	case NODE_LIST:
	{
		list_node_o *node = NULL;
		ast_node_o *current = ast_node->array.elements;
		while (current)
		{
			list_push(&node, evaluate(current));
			current = current->next;
		}
		return node;
	}
	case NODE_ARRAY:
	{
		array_o *array = new_array(0);
		ast_node_o *current = ast_node->array.elements;
		while (current)
		{
			array_push(array, evaluate(current));
			current = current->next;
		}
		return array;
	}
	case NODE_OBJECT:
	{
		ast_node_o *current = ast_node->object.members;
		htable_o *htable = new_htable(4);
		while (current)
		{
			object_o key = evaluate(current->key_value.key);
			object_o value = evaluate(current->key_value.value);
			htable_set(htable, key, value);
			current = current->next;
		}

		return htable;
	}
	default:
		printf("node %d\n", ast_node->node_type);
		break;
	}

	return NULL;
}