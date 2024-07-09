#include "objects/pickle/pickle.h"

object_o ast_node_str(object_o object)
{
	if (object == NULL)
		return NULL;
	return ast_str((ast_node_o *)object);
}

void ast_node_visitor(object_o object, fun_process_t fun_process)
{
	ast_node_o *ast_node = (ast_node_o *)object;
	if (object == NULL)
		return;

	ast_visit(ast_node, fun_process);
}

ast_node_o *new_ast_node(node_type_e node_type)
{
	ast_node_o *ast_node = alloc(1, sizeof(ast_node_o), AST_NODE_O);
	ast_node->node_type = node_type;
	ast_node->next = NULL;

	printf("node_type: %d\n", node_type);

	switch (node_type)
	{
	case NODE_NUMBER:
		ast_node->number = NULL;
		break;

	case NODE_STRING:
		ast_node->string = NULL;
		break;

	case NODE_WORD:
		ast_node->word = NULL;
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

	case NODE_UNARY_EXP:
		ast_node->unary_exp.operand = NULL;
		break;

	case NODE_SUPER_STATEMENT:
		ast_node->super_statement.type = -1;
		ast_node->super_statement.statement = NULL;
		break;

	case NODE_RETURN:
		ast_node->return_smt.exp = NULL;
		break;

	case NODE_IMPORT:
		ast_node->import_smt.path = NULL;
		break;

	case NODE_BINARY_EXP:
		ast_node->binary_exp.left = NULL;
		ast_node->binary_exp.right = NULL;
		break;

	case NODE_VARIABLE:
		ast_node->variable.type = NULL;
		ast_node->variable.value = NULL;
		ast_node->variable.identifier = NULL;
		ast_node->variable.initializer = NULL;
		break;

	case NODE_PARAMETER_DEC:
		ast_node->parameter_dec.type = NULL;
		ast_node->parameter_dec.identifier = NULL;
		ast_node->parameter_dec.initializer = NULL;
		break;

	case NODE_FUNCTION_DEC:
		ast_node->function_dec.body = NULL;
		ast_node->function_dec.identifier = NULL;
		ast_node->function_dec.parameters = NULL;
		ast_node->function_dec.return_type = NULL;
		break;

	case NODE_FUNCTION_CALL:
		ast_node->function_call.identifier = NULL;
		ast_node->function_call.arguments = NULL;
		ast_node->function_call.call_exp = NULL;
		break;

	case NODE_NT_FUNCTION:
		ast_node->nt_function_call.identifier = NULL;
		ast_node->nt_function_call.arguments = NULL;
		break;

	case NODE_TERNARY_EXP:
		ast_node->ternary_exp.type = 0;
		ast_node->ternary_exp.condition = NULL;
		ast_node->ternary_exp.if_body = NULL;
		ast_node->ternary_exp.else_body = NULL;
		break;

	case NODE_WHILE_STATEMENT:
		ast_node->while_statement.body = NULL;
		ast_node->while_statement.condition = NULL;
		break;

	case NODE_FOR_STATEMENT:
		ast_node->for_statement.body = NULL;
		ast_node->for_statement.init = NULL;
		ast_node->for_statement.update = NULL;
		ast_node->for_statement.condition = NULL;
		break;

	case NODE_STRUCT:
		ast_node->struct_statement.body = NULL;
		ast_node->struct_statement.identifier = NULL;
		break;

	case NODE_STRUCT_INSTANCE:
		ast_node->struct_instance.fields = NULL;
		ast_node->struct_instance.identifier = NULL;
		break;

	case NODE_FIELD_ACCESS:
		ast_node->field_access.parent = NULL;
		ast_node->field_access.child = NULL;
		break;

	case NODE_ARRAY_ACCESS:
		ast_node->array_access.parent = NULL;
		ast_node->array_access.expression = NULL;
		break;

	case NODE_BLOCK:
		ast_node->block_statement.stmts = NULL;
		break;

	case NODE_PROGRAM:
		ast_node->program.sources = NULL;
		break;

	default:
		break;
	}

	static const vtable_t vt = {
		.__str__ = ast_node_str,
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