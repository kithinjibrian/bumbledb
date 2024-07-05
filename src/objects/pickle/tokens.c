#include "objects/pickle/pickle.h"

void token_visitor(object_o object, fun_process_t fun_process)
{
	token_o *token = (token_o *)object;
	post_visit(token->token, fun_process);
}

object_o token_str(object_o object)
{
	token_o *token = (token_o *)object;
	SMART string_o *fmt = str(token->token);
	return string_from("{ type: %d, token: \"%s\", line: %d, column: %d }",
					   token->token_type,
					   cstring(fmt),
					   token->line,
					   token->column);
}

token_o *new_token(token_type_e token_type, string_o *tkn, int line, int column)
{
	token_o *token = alloc(1, sizeof(token_o), TOKEN_O);
	token->line = line;
	token->column = column;
	token->token = tkn;
	token->token_type = token_type;

	static const vtable_t vt = {
		.__str__ = token_str,
		.__visitor__ = token_visitor,
	};

	object_reg_dunders(token, &vt);

	return token;
}

token_type_e token_get_type(token_o *token)
{
	return token->token_type;
}

string_o *token_get_token(token_o *token)
{
	return token->token;
}