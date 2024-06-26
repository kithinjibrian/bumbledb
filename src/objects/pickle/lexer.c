#include "objects/pickle/pickle.h"

token_o *tokenize(string_o *buffer)
{

	while (isspace(string_get_char(buffer)))
	{
		string_next(buffer);
	}

	int index_start = string_index(buffer);

	char c = string_get_char(buffer);

	switch (c)
	{
	case '\0':
	{
		return new_token(TOKEN_EOL, string_from("<<EOL>>"));
	};
	case ',':
	{
		return new_token(TOKEN_COMMA, string_slice(buffer, index_start, index_start + 1));
	};
	case ';':
	{
		return new_token(TOKEN_SEMICOLON, string_slice(buffer, index_start, index_start + 1));
	};
	case ':':
	{
		return new_token(TOKEN_COLON, string_slice(buffer, index_start, index_start + 1));
	}
	case '=':
	{
		return new_token(TOKEN_EQUALS, string_slice(buffer, index_start, index_start + 1));
	};
	case '[':
	{
		return new_token(TOKEN_LBRACKET, string_slice(buffer, index_start, index_start + 1));
	};
	case ']':
	{
		return new_token(TOKEN_RBRACKET, string_slice(buffer, index_start, index_start + 1));
	};
	case '{':
	{
		return new_token(TOKEN_LBRACE, string_slice(buffer, index_start, index_start + 1));
	};
	case '}':
	{
		return new_token(TOKEN_RBRACE, string_slice(buffer, index_start, index_start + 1));
	}
	case '(':
	{
		return new_token(TOKEN_LPAREN, string_slice(buffer, index_start, index_start + 1));
	};
	case ')':
	{
		return new_token(TOKEN_RPAREN, string_slice(buffer, index_start, index_start + 1));
	}
	case '-':
	{
		return new_token(TOKEN_MINUS, string_slice(buffer, index_start, index_start + 1));
	}
	case '>':
	{
		return new_token(TOKEN_GTHAN, string_slice(buffer, index_start, index_start + 1));
	}
	case '<':
	{
		return new_token(TOKEN_LTHAN, string_slice(buffer, index_start, index_start + 1));
	}
	case '_':
	{
		return new_token(TOKEN_UNDERSCORE, string_slice(buffer, index_start, index_start + 1));
	}
	case '|':
	{
		return new_token(TOKEN_PIPE, string_slice(buffer, index_start, index_start + 1));
	}
	}

	if (c == '"' || c == '\'')
	{
		char quote_type = c;
		string_next(buffer);
		while (!string_at_end(buffer))
		{
			c = string_get_char(buffer);
			if (c == quote_type)
			{
				if (string_index(buffer) > index_start + 1 && string_at(buffer, string_index(buffer) - 1) == '\\')
				{
					string_next(buffer);
					continue;
				}
				else
				{
					return new_token(TOKEN_STRING, string_slice(buffer, index_start + 1, string_index(buffer)));
				}
			}
			string_next(buffer);
		}

		return error("fun tokenize(): Unclosed string", SILENT | ERROR_UNCLOSED_STRING);
	}

	if (isdigit(c))
	{
		while (isdigit(string_at(buffer, string_index(buffer) + 1)))
		{
			string_next(buffer);
		}
		return new_token(TOKEN_NUMBER, string_slice(buffer, index_start, string_index(buffer) + 1));
	}

	if (isalpha(c))
	{
		while (isalpha(string_at(buffer, string_index(buffer) + 1)))
		{
			string_next(buffer);
		}

		return new_token(TOKEN_WORD, string_slice(buffer, index_start, string_index(buffer) + 1));
	}

	return error("fun token(): Unknown token", SILENT);
}

array_o *lexer(string_o *buffer)
{
	SMART array_o *tokens = new_array(0);
	while (!string_at_end(buffer))
	{
		SMART token_o *lexeme = tokenize(buffer);

		if (is_error(lexeme))
		{
			if (error_type((error_o *)lexeme) & ERROR_UNCLOSED_STRING)
			{
				RETURN(lexeme);
			}
			break;
		}

		array_push(tokens, MOVE(lexeme));

		string_next(buffer);
	}
	RETURN(tokens);
}