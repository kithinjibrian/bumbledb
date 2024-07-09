#include "objects/pickle/pickle.h"

object_o lexer_str(object_o object)
{
	lexer_o *lexer = (lexer_o *)object;
	return string_from("{line: %d, column: %d}", lexer->line, lexer->column);
}

void lexer_visitor(object_o object, fun_process_t fun_process)
{
	lexer_o *lexer = (lexer_o *)object;
	post_visit(lexer->input, fun_process);
}

lexer_o *new_lexer(string_o *buffer)
{
	lexer_o *lexer = alloc(1, sizeof(lexer_o), LEXER_O);
	lexer->input = buffer;
	lexer->line = 1;
	lexer->column = 0;

	static const vtable_t vt = {
		.__str__ = lexer_str,
		.__visitor__ = lexer_visitor,
	};

	object_reg_dunders(lexer, &vt);

	return lexer;
}

int lcolumn(lexer_o *lexer)
{
	lexer->column++;
	return lexer->column;
}

char lchar(lexer_o *lexer)
{
	return string_get_char(lexer->input);
}

lexer_o *lnext(lexer_o *lexer)
{
	if (string_get_char(lexer->input) == '\n')
	{
		lexer->line++;
		lexer->column = 0;
	}
	else
	{
		lexer->column++;
	}

	string_next(lexer->input);

	return lexer;
}

int lindex(lexer_o *lexer)
{
	return lexer->input->index;
}

token_o *tokenize(lexer_o *lexer)
{

	while (isspace(lchar(lexer)))
	{
		lnext(lexer);
	}

	int index_start = lindex(lexer);

	char c = lchar(lexer);

	switch (c)
	{
	case '\0':
	{
		lcolumn(lexer);
		return new_token(TOKEN_EOL, string_from("<<EOL>>"), lexer->line, lexer->column);
	};
	case '!':
	{
		lcolumn(lexer);
		return new_token(TOKEN_EXCLAMATION, string_from("!"), lexer->line, lexer->column);
	};
	case '.':
	{
		lcolumn(lexer);
		return new_token(TOKEN_DOT, string_from("."), lexer->line, lexer->column);
	};
	case ',':
	{
		lcolumn(lexer);
		return new_token(
			TOKEN_COMMA,
			string_slice(lexer->input, index_start, index_start + 1),
			lexer->line,
			lexer->column);
	};
	case ';':
	{
		lcolumn(lexer);
		return new_token(TOKEN_SEMICOLON,
						 string_slice(lexer->input, index_start, index_start + 1),
						 lexer->line,
						 lexer->column);
	};
	case ':':
	{
		lcolumn(lexer);
		return new_token(TOKEN_COLON,
						 string_slice(lexer->input, index_start, index_start + 1),
						 lexer->line,
						 lexer->column);
	}
	case '=':
	{
		lcolumn(lexer);
		return new_token(TOKEN_EQUALS,
						 string_slice(lexer->input, index_start, index_start + 1),
						 lexer->line,
						 lexer->column);
	};
	case '[':
	{
		lcolumn(lexer);
		return new_token(TOKEN_LBRACKET,
						 string_slice(lexer->input, index_start, index_start + 1),
						 lexer->line,
						 lexer->column);
	};
	case ']':
	{
		lcolumn(lexer);
		return new_token(TOKEN_RBRACKET,
						 string_slice(lexer->input, index_start, index_start + 1),
						 lexer->line,
						 lexer->column);
	};
	case '{':
	{
		lcolumn(lexer);
		return new_token(TOKEN_LBRACE,
						 string_slice(lexer->input, index_start, index_start + 1),
						 lexer->line,
						 lexer->column);
	};
	case '}':
	{
		lcolumn(lexer);
		return new_token(TOKEN_RBRACE,
						 string_slice(lexer->input, index_start, index_start + 1),
						 lexer->line,
						 lexer->column);
	}
	case '(':
	{
		lcolumn(lexer);
		return new_token(TOKEN_LPAREN,
						 string_slice(lexer->input, index_start, index_start + 1),
						 lexer->line,
						 lexer->column);
	};
	case ')':
	{
		lcolumn(lexer);
		return new_token(TOKEN_RPAREN,
						 string_slice(lexer->input, index_start, index_start + 1),
						 lexer->line,
						 lexer->column);
	}
	case '+':
	{
		lcolumn(lexer);
		return new_token(TOKEN_PLUS,
						 string_slice(lexer->input, index_start, index_start + 1),
						 lexer->line,
						 lexer->column);
	}
	case '-':
	{
		lcolumn(lexer);
		return new_token(TOKEN_MINUS,
						 string_slice(lexer->input, index_start, index_start + 1),
						 lexer->line,
						 lexer->column);
	}
	case '*':
	{
		lcolumn(lexer);
		return new_token(TOKEN_MULT,
						 string_slice(lexer->input, index_start, index_start + 1),
						 lexer->line,
						 lexer->column);
	}
	case '/':
	{
		lcolumn(lexer);
		if (string_at(lexer->input, index_start + 1) == '/')
		{
			lnext(lexer);
			lnext(lexer);

			while (!string_at_end(lexer->input) && lchar(lexer) != '\n')
			{
				lnext(lexer);
			}

			return new_token(TOKEN_COMMENT, string_from("<<COMMENT>>"), lexer->line, lexer->column);
		}
		else if (string_at(lexer->input, lexer->input->index + 1) == '*')
		{

			lnext(lexer);
			lnext(lexer);

			while (!string_at_end(lexer->input))
			{
				if (lchar(lexer) == '*' && string_at(lexer->input, lexer->input->index + 1) == '/')
				{
					lnext(lexer);
					lnext(lexer);
					break;
				}
				lnext(lexer);
			}

			return new_token(TOKEN_COMMENT, string_from("<<COMMENT>>"), lexer->line, lexer->column);
		}
		else
		{
			return new_token(TOKEN_DIVIDE,
							 string_slice(lexer->input, index_start, index_start + 1),
							 lexer->line,
							 lexer->column);
		}

		break;
	}
	case '%':
	{
		lcolumn(lexer);
		return new_token(TOKEN_MODULO,
						 string_slice(lexer->input, index_start, index_start + 1),
						 lexer->line,
						 lexer->column);
	}
	case '>':
	{
		lcolumn(lexer);
		return new_token(TOKEN_GREATER,
						 string_slice(lexer->input, index_start, index_start + 1),
						 lexer->line,
						 lexer->column);
	}
	case '<':
	{
		lcolumn(lexer);
		return new_token(TOKEN_LESSER,
						 string_slice(lexer->input, index_start, index_start + 1),
						 lexer->line,
						 lexer->column);
	}
	case '|':
	{
		lcolumn(lexer);
		return new_token(TOKEN_PIPE,
						 string_slice(lexer->input, index_start, index_start + 1),
						 lexer->line,
						 lexer->column);
	}
	case '&':
	{
		lcolumn(lexer);
		return new_token(TOKEN_AMPERSAND,
						 string_slice(lexer->input, index_start, index_start + 1),
						 lexer->line,
						 lexer->column);
	}
	case '^':
	{
		lcolumn(lexer);
		return new_token(TOKEN_CARET,
						 string_slice(lexer->input, index_start, index_start + 1),
						 lexer->line,
						 lexer->column);
	}
	case '?':
	{
		lcolumn(lexer);
		return new_token(TOKEN_QUESTION,
						 string_slice(lexer->input, index_start, index_start + 1),
						 lexer->line,
						 lexer->column);
	}
	}

	if (c == '"' || c == '\'')
	{
		int col = lcolumn(lexer);

		char quote_type = c;
		lnext(lexer);

		while (!string_at_end(lexer->input))
		{
			c = lchar(lexer);
			if (c == quote_type)
			{
				if (lindex(lexer) > index_start + 1 && string_at(lexer->input, lindex(lexer) - 1) == '\\')
				{
					lnext(lexer);
					continue;
				}
				else
				{
					return new_token(TOKEN_STRING,
									 string_slice(lexer->input, index_start + 1, lindex(lexer)),
									 lexer->line, col);
				}
			}
			lnext(lexer);
		}

		return error("fun tokenize(): Unclosed string", SILENT | ERROR_UNCLOSED_STRING);
	}

	if (isdigit(c))
	{
		int col = lcolumn(lexer);

		while (isdigit(string_at(lexer->input, lindex(lexer) + 1)))
		{
			lnext(lexer);
		}
		return new_token(TOKEN_NUMBER,
						 string_slice(lexer->input, index_start, lindex(lexer) + 1),
						 lexer->line, col);
	}

	if (isalpha(c) || c == '_')
	{
		int col = lcolumn(lexer);

		while (isalpha(string_at(lexer->input, lindex(lexer) + 1)) ||
			   string_at(lexer->input, lindex(lexer) + 1) == '_' ||
			   isdigit(string_at(lexer->input, lindex(lexer) + 1)))
		{
			lnext(lexer);
		}

		return new_token(TOKEN_WORD,
						 string_slice(lexer->input, index_start, lindex(lexer) + 1),
						 lexer->line, col);
	}

	return error("fun token(): Unknown token", SILENT);
}

array_o *lexer(string_o *buffer)
{
	SMART array_o *tokens = new_array(0);
	SMART lexer_o *lexer = new_lexer(buffer);

	while (!string_at_end(lexer->input))
	{
		SMART token_o *lexeme = tokenize(lexer);

		if (is_error(lexeme))
		{
			if (error_type((error_o *)lexeme) & ERROR_UNCLOSED_STRING)
			{
				RETURN(lexeme);
			}
			break;
		}

		array_push(tokens, MOVE(lexeme));

		string_next(lexer->input);
	}
	RETURN(tokens);
}