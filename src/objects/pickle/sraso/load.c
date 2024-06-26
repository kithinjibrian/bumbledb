#include "objects/pickle/pickle.h"

object_o load(string_o *sraso)
{
	SMART string_o *smt_sraso = sraso;
	SMART array_o *tokens = lexer(smt_sraso);

	string_rewind(sraso);

	if (is_error(tokens))
	{
		return NULL;
	}

	SMART ast_node_o *ast_node = parse(tokens);

	if (!is_error(ast_node))
	{
		object_o object = evaluate(ast_node);
		return object;
	}

	return NULL;
}