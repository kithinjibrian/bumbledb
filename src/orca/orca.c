#include "orca/orca.h"

/*

<program> 						::= <statement_list>

<statement_list> 				::= <statement> | <statement> <statement_list>

<statement> 					::=  <expression_statement>
									| <compound_statement>
									| <jump_statement>
									| <selection-statement>
									| <iteration-statement>
									| <variable_declaration> ";"
									| <function_declaration>
									| <export_statement>

<expression_statement> 			::= <expression> ";"

<expression> 					::= <assignment_expression>

<assignment_expression> 		::=  <equality_expression>
									| "=" <assignment_expression>

<compound_statement> 			::= "{" <statement_list_opt> "}"

<statement_list_opt>			::= <statement_list> | ""

<jump_statement> 				::=  "break" ";"
									| "return" <expression_opt> ";"

<selection-statement> 			::= if "(" <expression> ")" <statement>
									| if "(" <expression> ")" <statement> else <statement>
									| switch "(" <expression> ")" <statement>

<iteration-statement> 			::= while ( <expression> ) <statement>
									| do <statement> while ( <expression> ) ;

<expression_opt> 				::= "" | <expression>

<variable_declaration>			::= "let" <identifier> <type_annotation_opt> <initializer_opt>

<type_annotation_opt> 			::= "" | ":" <type>

<type>							::= "int" | "str" | "bool" | "any" | "void" | <type> "[]"

<initializer_opt> 				::= "" | "=" <constant_expression>

<function_declaration> 			::= "fun" <identifier> "(" <parameter_list_opt> ")" <type_annotation_opt> <compound_statement>

<parameter_list_opt> 			::= "" | <parameter_list>

<parameter_list> 				::= <parameter> | <parameter> "," <parameter_list>

<parameter> 					::= <identifier> <type_annotation_opt>

<export_statement> 				::= "export" <object_literal>

<constant_expression>			::= <equality_expression>

<equality_expression> 			::= <relational_expression>
									| <equality_expression> == <relational_expression>
									| <equality_expression> != <relational_expression>

<relational_expression> 		::= <shift_expression>
									| <relational_expression> "<" <shift_expression>
									| <relational_expression> ">" <shift_expression>
									| <relational_expression> "<=" <shift_expression>
									| <relational_expression> ">=" <shift_expression>

<shift_expression> 				::= <additive_expression>

<additive_expression>			::= <multiplicative_expression>
									| <additive_expression> "+" <multiplicative_expression>
									| <additive_expression> "_" <multiplicative_expression>

<multiplicative_expression>		::= <unary_expression>
									| <multiplicative_expression> "*" <unary_expression>
									| <multiplicative_expression> "/" <unary_expression>
									| <multiplicative_expression> "%" <unary_expression>

<unary_expression>				::= <postfix_expression>

<postfix_expression>			::= <primary_expression>
									| <postfix_expression> "." <identifier>
									| <postfix_expression> "[" <expression> "]"
									| <postfix_expression> "(" <argument_list_opt> ")"
									| <postfix_expression> ++

<primary_expression>			::= <number>
									| <string>
									| ( <constant_expression> )
									| <function_call>
									| <method_call>
									| <identifier>
									| <array_literal>
									| <object_literal>
									| <list_literal>
									| <array_access>
									| <field_access>


<function_call>					::= <identifier> "(" <argument_list_opt> ")"

<method_call>					::= <identifier> "." <identifier> "(" <argument_list_opt> ")"

<argument_list_opt>				::= "" | <argument_list>

<argument_list>					::= <constant_expression> | <constant_expression> "," <argument_list>

<array_literal>					::= "[" <array_elements_opt> "]"

<array_elements_opt>			::= "" | <array_elements>

<array_elements>				::= <constant_expression> | <constant_expression> "," <array_elements>

<list_literal>					::= "("  <array_elements> ")"

<array_elements>				::= <constant_expression> | <constant_expression> "," <array_elements>

<array_access>					::= <identifier> "[" <constant_expression> "]"

<field_access>					::= <identifier> "." <identifier>

<object_literal>				::= "{" <key_value_list_opt> "}"

<key_value_list_opt>			::= "" | <key_value_list>

<key_value_list>				::= <key_value> | <key_value> "," <key_value_list>

<key_value>						::= <key> ":" <constant_expression>

<key>							::= <identifier> | <string>
*/

htable_o *__orca__(string_o *prog)
{
	SMART array_o *tokens = lexer(prog);
	SMART ast_node_o *ast = orca_parse(tokens);
	print(ast);

	htable_o *result = NULL;
	result = orca_evaluate(ast);

	return result;
}

htable_o *orca(string_o *filename)
{
	SMART string_o *fn_smt = filename;
	SMART file_o *file = f_open(cstring(fn_smt), "r");
	if (is_error(file))
	{
		error("File not found", PRINT | PANIC);
	}

	fseek(f_get(file), 0, SEEK_END);
	long size = ftell(f_get(file));
	fseek(f_get(file), 0, SEEK_SET);

	uint8_t buffer[size];
	memset(buffer, 0, size);
	fread(buffer, size, 1, f_get(file));

	string_o *str = string_from("%.*s", size, buffer);

	return __orca__(str);
}