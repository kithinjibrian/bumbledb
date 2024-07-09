#include "orca/orca.h"

/*
<program> 						::= <statement_list>

<statement_list> 				::= <statement> | <statement> <statement_list>

<statement> 					::=  <expression_statement>
									| <compound_statement>
									| <jump_statement>
									| <selection-statement>
									| <iteration-statement>
									| <export_statement>
									| <function_declaration>
									| <variable_dec>
									| <htable_desc>

<expression_statement> 			::= <expression> ";"

<expression> 					::= <assignment_expression>
									| <expression> "," <assignment_expression>

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
									| <postfix_expression> "!" "(" <argument_list_opt> ")"
									| <postfix_expression> ++

<primary_expression>			::= <number>
									| <string>
									| ( <constant_expression> )
									| <identifier>
									| <array_literal>
									| <htable_literal>
									| <list_literal>


<function_call>					::= <identifier> "(" <argument_list_opt> ")"

<method_call>					::= <identifier> "." <identifier> "(" <argument_list_opt> ")"

<argument_list_opt>				::= "" | <argument_list>

<argument_list>					::= <assignment_expression> | <assignment_expression> "," <argument_list>

<array_literal>					::= "[" <array_elements_opt> "]"

<array_elements_opt>			::= "" | <array_elements>

<array_elements>				::= <constant_expression> | <constant_expression> "," <array_elements>

<list_literal>					::= "("  <array_elements> ")"

<array_elements>				::= <constant_expression> | <constant_expression> "," <array_elements>

<array_access>					::= <identifier> "[" <constant_expression> "]"

<field_access>					::= <identifier> "." <identifier>

<htable_literal>				::= "{" <key_value_list_opt> "}"

<key_value_list_opt>			::= "" | <key_value_list>

<key_value_list>				::= <key_value> | <key_value> "," <key_value_list>

<key_value>						::= <key> ":" <constant_expression>

<key>							::= <identifier> | <string>
*/

/*
<program>								::= ""
											| <source_elements> <EOL>

<source_elements>						::= <source_element>
											| <source_element> <source_elements>

<source_element>						::=  <statement>
											| <function_expression>

<function_expression> 					::= "fun" <identifier>? "(" <parameter_list_opt> ")" <type_annotation_opt> <function_body>

<parameter_list_opt> 					::= ""
											| <parameter_list>

<parameter_list>						::= <parameter>
											| <parameter> "," <parameter_list>

<parameter>								::= <identifier> <type_annotation_opt>

<primary_expression>					::= <identifier>
											| <literal>
											| "(" <expression> ")"
											| <htable_literal>
											| <list_literal>
											| <array_literal>
											| <native_function>

<literal>								::= <number>
											| <string>
											| <boolean>
											| <null>

<htable_literal>						::= "map{" <key_value_list_opt> "}"

<key_value_list_opt>					::= <key_value_list>
											| ""

<key_value_list>						::= <key_value>
											| <key_value> "," <key_value_list>

<key_value>								::= <key> ":" <assignment_expression>

<key>									::= <string>
											| <number>

<list_literal>							::= "list{" <expression> "}"

<array_literal>							::= "array{" <expression> "}"

<native_function>						::= <identifier> "!" "(" <argument_list_opt> ")"

<identifier>							::= <word>

<statement>      						::= <block_statement>
											| <expression_statement>
											| <variable_statement>
											| <if_statement>
											| <iteration_statement>
											| <continue_statement>
											| <break_statement>
											| <return_statement>
											| <export_statement>
											| <struct_declaration>

<block_statement>          				::= "{" <statement_list_opt> "}"

<statement_list_opt>					::= <statement_list>
											| ""

<statement_list> 						::= <statement>
											| <statement> <statement_list>

<expression_statement>					::= <expression> ";"

<expression>							::= <assignment_expression>
											| <expression> "," <assignment_expression>

<assignment_expression>					::= <conditional_expression>
											| <unary_expression> <assignment_operator> <assignment_expression>

<assignment_operator>					::= ":=" | "+=" | "-=" | "*=" | "/=" | "%="

<left_hand_expression>					::= <call_expression>
											| <member_expression>

<member_expression>						::= <function_or_primary_expression>
											| <member_expression_part>
											| <class_expression>

<class_expression>						::= "new" <member_expression> <arguments>

<function_or_primary_expression>		::= <function_expression>
											| <primary_expression>

<member_expression_part>				::= "[" <expression> "]"
											| "." <identifier>

<call_expression>						::= <member_expression> <arguments> <call_expression_part>

<call_expression_part>					::= <arguments>
											| "[" <arguments> "]"
											| "." <identifier>

<arguments>								::= "(" <argument_list_opt> ")"

<argument_list_opt>						::= "" | <argument_list>

<argument_list>							::= <assignment_expression>
											| <assignment_expression> "," <argument_list>

<conditional_expression>				::= <logical_or_expression>
											| <logical_or_expression> "?" <assignment_expression> ":" <assignment_expression>

<logical_or_expression>					::= <logical_and_expression>
											| <logical_or_expression> "||" <logical_and_expression>

<logical_and_expression>				::= <bitwise_or_expression>
											| <logical_and_expression> "&&" <bitwise_or_expression>

<bitwise_or_expression>					::= <bitwise_xor_expression>
											| <bitwise_or_expression> "|" <bitwise_xor_expression>

<bitwise_xor_expression>				::= <bitwise_and_expression>
											| <bitwise_xor_expression> "^" <bitwise_and_expression>

<bitwise_and_expression>				::= <equality_expression>
											| <bitwise_and_expression> "&" <equality_expression>

<equality_expression>					::= <relational_expression>
											| <equality_expression> "==" <relational_expression>
											| <equality_expression> "!=" <relational_expression>

<relational_expression>					::= <shift_expression>
											| <relational_expression> "<" <shift_expression>
											| <relational_expression> ">" <shift_expression>
											| <relational_expression> "<=" <shift_expression>
											| <relational_expression> ">=" <shift_expression>
											| <relational_expression> "in" <shift_expression>

<shift_expression>						::= <additive_expression>
											| <shift_expression> "<<" <additive_expression>
											| <shift_expression> ">>" <additive_expression>

<additive_expression>					::= <multiplicative_expression>
											| <additive_expression> "+" <multiplicative_expression>
											| <additive_expression> "-" <multiplicative_expression>

<multiplicative_expression>				::= <unary_expression>
											| <multiplicative_expression> "*" <unary_expression>
											| <multiplicative_expression> "/" <unary_expression>
											| <multiplicative_expression> "%" <unary_expression>

<unary_expression>						::= <postfix_expression>
											| "*" <postfix_expression>


<postfix_expression>					::= <left_hand_expression>
											| <left_hand_expression> "++"
											| <left_hand_expression> "--"

<variable_statement>					::= "let" <declaration_list> ";"

<declaration_list>						::= <declaration>
											| <declaration> "," <declaration_list>

<declaration>							::= <identifier> <type_annotation_opt> <initializer_opt>

<initializer_opt>						::= "" | "=" <assignment_expression>

<if_statement>							::= "if" "(" <expression> ")" <statement>
											| "if" "(" <expression> ")" <statement> "else" <statement>

<iteration_statement>					::= "while" "(" <expression> ")" <statement>
											| "for" "(" <variable_statement> ";" <expression>? ";" <expression>? ")" <statement>

<continue_statement>					::= "continue" ";"

<break_statement>						::= "break" ";"

<return_statement>						::= "return" <expression>? ";"

<export_statement>						::= "export" <htable_literal>

<struct_declaration>					::= "struct" <identifier> "{" <struct_body> "}"

<struct_body>							::= <field_declaration> <struct_body>
											| <member_declaration> <struct_body>

<field_declaration>						::= <declaration>

<member_declaration>					::= <identifier> "(" <parameter_list_opt> ")" <type_annotation_opt> <function_body>
*/

htable_o *eval(string_o *prog)
{
	SMART array_o *tokens = lexer(prog);
	SMART ast_node_o *ast = orca_parse2(tokens);

	print(ast);

	// SMART void *c = new_array(0);
	// type_check(ast, c);

	// print(array_join(c, ""));

	htable_o *result = NULL;
	// result = orca_evaluate(ast);

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

	return eval(str);
}