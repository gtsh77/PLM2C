#include <stdio.h>
#ifdef IBMPC
#include <stdlib.h>
#endif
#include "misc.h"
#include "defs.h"
#include "cvt.h"
#include "struct.h"
#include "cvt_id.h"
#include "tokens.h"

extern	char	*text_buffer, *text_ptr;
extern	int	line_count;
extern	char	*out_string;

/*
 *	Parse a procedure parameter list.
 *	Return head of linked list of parameters.
 */
get_param_list(param_head)
PARAM_LIST	**param_head;
{
	PARAM_LIST	*list_ptr, *param_ptr;
	int		token_class;
	TOKEN		sep_token;

	*param_head = NULL;
	list_ptr = NULL;

	do {
		get_param_ptr(&param_ptr);
		if (*param_head == NULL)
			*param_head = param_ptr;
		if (list_ptr)
			list_ptr->next_param = param_ptr;
		list_ptr = param_ptr;
		token_class = get_token(&param_ptr->param);
		if (token_class != IDENTIFIER) {
			parse_error("Identifier expected");
			free_param_list(*param_head);
		}
			/* Get ',' or ')' */
		token_class = get_token(&sep_token);
	} while (token_class == COMMA);

	if (token_class != RIGHT_PAREN) {
		parse_error("')' expected");
		free_param_list(*param_head);
	}
}

/*
 *	Parse parameter list.
 *	Parse DECLARE statements until all parameters in param_list
 *	have been found.  Split declare statements into those used in
 *	param_list and those not.  Return pointers to head of both
 *	DECL_MEMBER lists.
 */
parse_param_list(param_list, decl_list, extra_decl_list)
PARAM_LIST	*param_list;
DECL		**decl_list, **extra_decl_list;
{
	DECL		*extra_decl, *extra_decl_ptr;
	DECL		*list, *list_ptr;
	DECL_MEMBER	*extra_list_ptr;
	DECL_MEMBER	*el_ptr, *last_el_ptr, *next_el_ptr;
	DECL_MEMBER	*extra_el_ptr;
	int		token_class;
	TOKEN		*decl_token;
	DECL_ID		*var_ptr, *last_var_ptr, *next_var_ptr;
	DECL_ID		*extra_last_var_ptr;
	PARAM_LIST	*param_ptr, *last_param;

	*decl_list = NULL;
	*extra_decl_list = NULL;
		/* Pointer to next DECL_MEMBER in decl_ptr */
	list_ptr = NULL;
		/* Pointer to next DECL in extra_decl_list */
	extra_decl_ptr = NULL;

	while (param_list) {
		    /* Get declaration */
	    get_token_ptr(&decl_token);
	    token_class = get_token(decl_token);
	    if ((token_class != RESERVED) ||
			(decl_token->token_type != DECLARE)) {
			parse_error("DECLARE expected");
			free((char *) decl_token);
			return;
	    }
		    /* Get declaration list */
	    get_decl_ptr(&list);
	    get_decl_list(list);
	    list->decl_token = decl_token;

		    /* Points to start of extra declaration list */
	    extra_list_ptr = NULL;

		    /* Pointer to previous el_ptr */
	    last_el_ptr = NULL;

		    /* Check each element of the DECLARE statement */
	    el_ptr = list->decl_list;
	    while (el_ptr) {

			/* Point to next member */
		next_el_ptr = el_ptr->next_member;

			/* Pointer to next DECL_MEMBER in extra_decl_ptr */
		extra_el_ptr = NULL;

			/* Points to last variable in variable list */
		last_var_ptr = NULL;
			/* Contains not found variables in name_list */
		extra_last_var_ptr = NULL;

			/* Check each variable in name list */
		for (var_ptr = el_ptr->name_list; var_ptr; ) {
			/* Point to following var_ptr */
		    next_var_ptr = var_ptr->next_var;

				/* Is this variable in param list? */
		    last_param = NULL;
		    for (param_ptr = param_list; param_ptr;
				param_ptr = param_ptr->next_param) {
			if (!strcmp(param_ptr->param.token_name,
			    var_ptr->name->token_name))
			    break;
			else
			    last_param = param_ptr;
		    }

		    if (param_ptr) {

				/* Variable found */
				/* Remove from parameter list */
			if (last_param)
			    last_param->next_param = param_ptr->next_param;
			else
			    param_list = param_ptr->next_param;

			free((char *) param_ptr);
		        last_var_ptr = var_ptr;
		    } else {
/*
 *	Variable not found - Add to extra variable list
 */
			if (extra_el_ptr == NULL) {
/*
 *	Create new element and copy DECLARE info
 */
			    get_element_ptr(&extra_el_ptr);
			    element_copy(el_ptr, extra_el_ptr);
			    extra_last_var_ptr = NULL;
/*
 *	Link new extra element into extra_list_ptr
 */
			    if (extra_list_ptr) {
				extra_list_ptr->next_member = extra_el_ptr;
			    } else {
/*
 *	Create new extra declaration
 */
				get_decl_ptr(&extra_decl);

					/* Point to DECLARE token */
				extra_decl->decl_token =
					list->decl_token;
				extra_decl->decl_list = extra_el_ptr;
/*
 *	Link new extra declaration into extra_decl_list
 */
				if (extra_decl_ptr)
				    extra_decl_ptr->next_decl = extra_decl;
				else
				    *extra_decl_list = extra_decl;

				extra_decl_ptr = extra_decl;
			    }
			    extra_list_ptr = extra_el_ptr;
			}

				/* Add var_ptr to extra list */
			if (extra_last_var_ptr)
			    extra_last_var_ptr->next_var = var_ptr;
			else
			    extra_list_ptr->name_list = var_ptr;

			extra_last_var_ptr = var_ptr;

				/* Remove from DECLARE list */
			if (last_var_ptr)
			    last_var_ptr->next_var = next_var_ptr;
			else
			    el_ptr->name_list = next_var_ptr;

			var_ptr->next_var = NULL;
		    }

		    var_ptr = next_var_ptr;
		}

/*
 *	Check for empty name list
 */
		if (el_ptr->name_list == NULL) {
/*
 *	Empty name list - unlink and discard element from declaration list
 */
		    if (last_el_ptr)
			last_el_ptr->next_member = next_el_ptr;
		    else
			list->decl_list = next_el_ptr;

		    el_ptr->next_member = NULL;
		    free((char *) el_ptr);
		} else
		    last_el_ptr = el_ptr;

		el_ptr = next_el_ptr;
	    }

		    /* Save found items in decl_list */
	    if (list->decl_list->name_list) {
		if (*decl_list)
		    list_ptr->next_decl = list;
		else
		    *decl_list = list;
		list_ptr = list;
	    } else
		free((char *) list);

	}
}

/*
 *	Parse until desired token type appears
 */
parse_till(type, token)
int	type;
TOKEN	*token;
{
	while (get_token(token) != type)
		out_token(token);
}

/*
 *	Parse until END statement
 */
parse_till_end(token)
TOKEN	*token;
{
	int	token_class;

	while (1) {
		token_class = get_token(token);

		if (token_class == END_OF_FILE) {
			parse_error("Premature end-of-file");
			exit(1);
		}

		if ((token_class == RESERVED) && (token->token_type == END))
			return;

		parse_statement(token);
	}
}

/*
 *	Parse through END statement
 */
parse_to_end()
{
	TOKEN	token;

	parse_till_end(&token);
	parse_statement(&token);
}

/*
 *	Check for end of line (';')
 */
check_eol()
{
	TOKEN	token;

	if (get_token(&token) != END_OF_LINE)
		parse_error("';' expected");
	else
		out_token(&token);
}

/*
 *	Parse simple variable and return variable token and following token.
 *	Passed with initial identifier in token.
 *	Returns with next_token terminating variable.
 *	Handles [ .<identifier> ] ...
 */
parse_simple_variable(token, next_token)
TOKEN	*token, *next_token;
{
	int	token_class;

	while (1) {
		token_class = get_token(next_token);
		if (token_class != PERIOD)
			return token_class;

			/* Process .<identifier> */
		token_class = get_token(next_token);
		if (token_class != IDENTIFIER) {
			parse_error("Illegal identifier");
			return ERROR;
		}
			/* Add .<identifier> to original token name */
		(void) strcat(token->token_name, ".");
		(void) strcat(token->token_name, next_token->token_name);
			/* Parse for additional member */
		return parse_simple_variable(token, next_token);
	}
}

/*
 *	Parse variable identifier.  If pointer, output (*ident) else
 *	If variable has BASED attribute, output (*based_name).
 *	Otherwise, output ident.
 */
out_ident(ident, decl, decl_id)
TOKEN		*ident;
DECL_MEMBER	*decl;
DECL_ID		*decl_id;
{
	if (decl->at_ptr || decl_id->is_ext_at) {
		out_white_space(ident);
		out_str("(*");
		out_token_name(ident);
		out_char(')');
	} else

	if (decl_id->based_name) {
		out_white_space(ident);
		out_str("(**");
		out_token_name(decl_id->name);
		out_char(')');
	} else
		out_token(ident);
}

/*
 *	Parse variable name or structure element and output appropriate tokens.
 *	Passed with identifier in token, and declaration for token in decl.
 *	Returns with token terminating variable.
 *	Handles <member> { [ ( <expression> ) ] [ .<identifier> ] }
 */
parse_member(token, decl, decl_id)
TOKEN		*token;
DECL_MEMBER	*decl;
DECL_ID		*decl_id;
{
	int		token_class;
	TOKEN		member;
	DECL_MEMBER	*var_decl;
	DECL_ID		*var_decl_id;

		/* Check for literal */
	if (decl->literal) {
			/* Yes - output case converted literal */
		out_white_space(token);
		out_cvt_name(token);
			/* Return next token */
		token_class = get_token(token);
		return token_class;
	}

	token_copy(token, &member);

	token_class = get_token(token);

		/* Check for array subscript */
	if (decl->array_bound) {
	    out_ident(&member, decl, decl_id);

	    if (token_class == LEFT_PAREN) {
			/* Convert to open square bracket */
		token->token_name[0] = '[';
		out_token(token);

			/* Parse expression to right parenthesis */
		token_class = parse_expression(token);
		if (token_class != RIGHT_PAREN) {
			parse_error("')' expected");
			return ERROR;
		}

			/* Convert to close square bracket */
		token->token_name[0] = ']';
		out_token(token);

		token_class = get_token(token);
	    }
	}

		/* Check for .<identifier> */
	if ((decl->type->token_type == STRUCTURE) && (token_class == PERIOD)) {

		if (decl->array_bound)
				/* Already printed identifier */
			out_token(token);
		else {
			if (decl->at_ptr || decl_id->based_name) {
/*
 * --- Note: Does not handle BASED AT variables!
 */
					/* Print 'member->' */
				out_token(&member);
				out_str("->");
			} else {
					/* Print 'member.' */
				out_ident(&member, decl, decl_id);
				out_token(token);
			}
		}

		token_class = get_token(token);
		if (token_class != IDENTIFIER) {
			parse_error("Illegal structure member");
			return ERROR;
		}

			/* Find variable in list */
		if (!find_list_symbol(token, decl->struct_list,
					&var_decl, &var_decl_id)) {
			parse_error("Undefined structure member");
			return ERROR;
		}

			/* Parse this member now */
		token_class = parse_member(token, var_decl, var_decl_id);
	} else
	    if (decl->array_bound == NULL)
		out_ident(&member, decl, decl_id);

	return token_class;
}

/*
 *	Parse variable and output appropriate tokens.
 *	Passed with initial identifier in token.
 *	Returns with token terminating variable.
 *	Handles { [ ( <expression> ) ] [ .<identifier> ] } ...
 */
parse_variable(token, var_decl, var_decl_id)
TOKEN		*token;
DECL_MEMBER	**var_decl;
DECL_ID		**var_decl_id;
{
	if (!find_symbol(token, var_decl, var_decl_id)) {
		parse_error("Undefined variable");
		return ERROR;
	}

	return parse_member(token, *var_decl, *var_decl_id);
}

/*
 *	See if token is in cvt_list.
 *	If found, return pointer to conversion string.
 */
check_cvt_id(token, cvt_id, cvt_string)
TOKEN	*token;
CVT_ID	*cvt_id;
char	**cvt_string;
{
		/* Check each string in cvt_id */
	while (*(cvt_id->id_name)) {
		if (!strcmp(token->token_name, cvt_id->id_name)) {
				/* Found match - return new string */
			*cvt_string = cvt_id->new_id;
			return TRUE;
		}

		cvt_id++;
	}

	return FALSE;
}

/*
 *	Parse function call
 */
parse_function(token)
TOKEN	*token;
{
	int		token_class;
	BOOLEAN		left_shift, right_shift;
	char		*new_func;
	DECL_MEMBER	*decl_ptr;
	DECL_ID		*decl_id;

		/* Function call - check for SHL or SHR */
	out_white_space(token);
	left_shift = !strcmp(token->token_name, "shl") ||
		     !strcmp(token->token_name, "SHL");
	right_shift = !strcmp(token->token_name, "shr") ||
		      !strcmp(token->token_name, "SHR");
	if (left_shift || right_shift) {
			/* SHL(expr, expr) or SHR(expr, expr) */
			/* Check for '(' */
		token_class = get_token(token);
		if (token_class != LEFT_PAREN) {
			parse_error("'(' expected");
			return ERROR;
		}
		out_token(token);

			/* Output first expression */
		out_char('(');
		token_class = parse_expression(token);
		if (token_class != COMMA) {
			parse_error("',' expected");
			return ERROR;
		}

		out_str(left_shift ? ") << (" : ") >> (");

			/* Output second expression */
		token_class = parse_expression(token);
		if (token_class != RIGHT_PAREN) {
			parse_error("Missing ')'");
			return ERROR;
		}
		out_char(')');
		out_token(token);
	} else {

			/* Check for a type cast function */
		if (check_cvt_id(token, &cast_functions[0], &new_func)) {
				/* Convert to a cast */
			out_char('(');
			out_str(new_func);
			out_str(") ");
		} else

			/* Check for a function conversion */
		if (check_cvt_id(token, &cvt_functions[0], &new_func)) {
				/* Convert to desired function */
			out_str(new_func);
		} else {

				/* Output function name */
			out_token_name(token);

				/* Check for parameter list */
			if (find_symbol(token, &decl_ptr, &decl_id)) {
			    if (decl_ptr->type->token_type != PROCEDURE) {
				parse_error("Illegal function call");
				return ERROR;
			    }
			    if (decl_ptr->initialization != DATA) {
					/* No param list */
				token_class = get_token(token);
				return token_class;
			    }
			}
		}

			/* Check for parameter list */
		token_class = get_token(token);
		if (token_class != LEFT_PAREN) {
			parse_warning("Parameter list expected");
			return token_class;
		}
		out_token(token);

			/* Parse to closing right paren */
		do {
			token_class = parse_expression(token);
			out_token(token);
		} while (token_class == COMMA);

		if (token_class != RIGHT_PAREN) {
			parse_error("Missing ')'");
			return ERROR;
		}
	}
		/* Return token following function */
	token_class = get_token(token);
	return token_class;
}

/*
 *	Parse expression and output appropriate tokens.
 *	Return token at end of expression.
 */
parse_expression(token)
TOKEN	*token;
{
    int		token_class;
    int		i, last_class, temp_class;
    DECL_MEMBER	*id_type;
    DECL_ID	*id_id;
    char	*new_id;
    char	string_const[MAX_TOKEN_LENGTH], octal_const[5];

    last_class = OPERATOR;

    token_class = get_token(token);

    while (1) {

	switch (token_class) {

	case LEFT_PAREN :
		if (last_class != OPERATOR) {
			parse_error("Missing operator");
			return ERROR;
		}

			/* Sub-expression */
		out_token(token);
			/* Parse to closing right paren */
		token_class = parse_expression(token);
		if (token_class != RIGHT_PAREN) {
			parse_error("Missing ')'");
			return ERROR;
		}

		out_token(token);
		break;

	case RIGHT_PAREN :
		return token_class;

	case OPERATOR :
		out_white_space(token);
		if (token->token_type == EQUAL)
				/* Make it a '==' */
			out_str("==");
		else

			/* Check for address operator '@' or '.' */
		if ((token->token_type == AT_OP) ||
				(token->token_type == PERIOD)) {
		    token_class = get_token(token);
		    if (token_class == IDENTIFIER) {
				/* Make it a '&' */
			out_char('&');

				/* See if it's a function reference */
			if (find_symbol(token, &id_type, &id_id) &&
			    (id_type->type->token_type != PROCEDURE)) {
				    /* Variable - parse it */
			    temp_class = parse_member(token, id_type, id_id);
			} else {

				    /* Function call - Check for */
				    /* a function conversion */
			    if (check_cvt_id(token, &cvt_functions[0], &new_id))
					    /* Convert to desired function */
				    out_str(new_id);
			    else
				    	    /* Function call - output name */
			    	    out_token_name(token);

			    temp_class = get_token(token);
			}
		    } else

		    if (token_class == LEFT_PAREN) {
				/* Constant list - convert to string */
			out_char('"');
			string_const[0] = '\0';

			do {
				token_class = get_token(token);
				if (token_class == STRING)
					(void) strcat(string_const, token->token_name);
				else
				if (token_class == NUMERIC) {
					cvt_octal(token, octal_const);
					(void) strcat(string_const, octal_const);
				} else {
					parse_error("Illegal constant");
					return ERROR;
				}

				token_class = get_token(token);
			} while (token_class == COMMA);

			if (token_class != RIGHT_PAREN) {
				parse_error("')' expected");
				return ERROR;
			}

			i = strlen(string_const);
			if ((i >= 4) &&
				(!strcmp(string_const + i - 4, "\\000")))
					/* Discard trailing null */
				string_const[i - 4] = '\0';
			out_str(string_const);
			out_char('"');
		    } else {
			parse_error("Illegal operator");
			return ERROR;
		    }
		} else

			out_token_name(token);
		break;

	case IDENTIFIER :
			/* Check for identifier conversion */
		if (check_cvt_id(token, &cvt_identifiers[0], &new_id)) {
			out_white_space(token);
			out_str(new_id);
			temp_class = get_token(token);
		} else

			/* See if variable in context */
		if (find_symbol(token, &id_type, &id_id) &&
			(id_type->type->token_type != PROCEDURE)) {
				/* Variable - parse it */
			temp_class = parse_member(token, id_type, id_id);
		} else

				/* Function call - parse it */
			temp_class = parse_function(token);
		break;

	case NUMERIC :
		out_token(token);
		break;

	case STRING :
		out_white_space(token);
			/* Convert to a numeric constant */
		if (token->token_length > 4) {
			parse_error("Illegal string constant");
			return ERROR;
		}

		if (token->token_length > 1)
			out_char('(');

		out_str_const(token->token_name, token->token_length);

		if (token->token_length > 1)
			out_char(')');
		break;

	default :
		/* Must not be part of an expression! */
		return token_class;
	}

	last_class = token_class;

	token_class = (last_class == IDENTIFIER) ?
			temp_class : get_token(token);
    }
}

/*
 *	DO statement
 *	Handles DO;, DO CASE, DO WHILE, and iterative DO
 */
parse_do(first_token)
TOKEN	*first_token;
{
	TOKEN		token;
	int		token_class;
	int		case_line;
	char		case_statement[MAX_TOKEN_LENGTH];
	char		case_output[MAX_CASE_STATEMENT_SIZE];
	char		var_string[MAX_TOKEN_LENGTH];
	char		*temp_out_string, *temp_out_string1;
	DECL_MEMBER	*var_decl;
	DECL_ID		*var_decl_id;

		/* Create new context */
	new_context(DO, (TOKEN *) NULL);

	out_white_space(first_token);

		/* Determine what kind of DO statement */
	token_class = get_token(&token);

	switch (token_class) {

	case END_OF_LINE :
			/* DO; */
		out_white_space(&token);
		out_char('{');			/* } for dumb vi */
		parse_to_end();
		break;

	case IDENTIFIER :
			/* DO counter = start TO limit BY step */
		out_str("for");
		out_must_white(&token);
		out_char('(');

			/* Put full variable in var_string */
		var_string[0] = '\0';
		temp_out_string = out_string;
		out_string = var_string;
		token_class = parse_variable(&token, &var_decl, &var_decl_id);
		out_string = temp_out_string;

			/* Check for '=' */
		if ((token_class != OPERATOR) ||
			(token.token_type != EQUAL)) {
			parse_error("Missing '='");
			pop_context();
			return;
		}
			/* Send <ident> '=' <expr> */
		out_str(var_string);
		out_token(&token);
		token_class = parse_expression(&token);
		if ((token_class != RESERVED) ||
			(token.token_type != TO)) {
			parse_error("Missing TO");
			pop_context();
			return;
		}

			/* Send <ident> <= <limit> */
		out_str("; ");
		out_str(var_string);
		out_str(" <=");
		token_class = parse_expression(&token);
		out_str("; ");

			/* Parse increment */
		if ((token_class == RESERVED) &&
			(token.token_type == BY)) {

				/* Send <ident> += <step> */
			out_str(var_string);
			out_str(" +=");
			token_class = parse_expression(&token);
		} else {
				/* Send <ident>++ */
			out_str(var_string);
			out_str("++");
		}

		out_str(") {");		/* } for dumb vi */
		out_white_space(&token);

		if (token_class != END_OF_LINE) {
			parse_error("BY or ';' expected");
			pop_context();
			return;
		}

		parse_to_end();
		break;

	case RESERVED :
		switch (token.token_type) {

		case CASE :
				/* DO CASE <expr>; */
			out_str("switch (");
			if (parse_expression(&token) != END_OF_LINE) {
				parse_error("';' expected");
				pop_context();
				return;
			}
			out_white_space(&token);
			out_str(") {");		/* } for dumb vi */

			case_line = 0;
			while (1) {
					/* Place case statement in out_string */
				temp_out_string1 = out_string;
				case_output[0] = '\0';
				out_string = case_output;
	
				(void) sprintf(case_statement, "case %d :",
					case_line++);
				token_class = parse_new_statement();
				if (token_class == END_OF_FILE) {
					parse_error("Premature end-of-file");
					exit(1);
				}
				if (token_class == END) {
					out_string = temp_out_string1;
					out_str(case_output);
					break;
				}
				out_string = temp_out_string1;
				out_white_space(first_token);
				out_str(case_statement);
				out_str(case_output);
				out_white_space(first_token);
				out_str("break;\n");
			}
			break;

		case WHILE :
				/* DO WHILE <expr>; */
			out_str("while (");
			if (parse_expression(&token) != END_OF_LINE) {
				parse_error("';' expected");
				pop_context();
				return;
			}
			out_white_space(&token);
			out_str(") {");		/* } for dumb vi */

			parse_to_end();
			break;

		default:
			parse_error("Illegal DO clause");
			pop_context();
			return;
		}
		break;
	}

		/* End of context */
	pop_context();
}

/*
 *	END statement
 *	Handles END [ <identifier> ] ;
 */
parse_end(first_token)
TOKEN	*first_token;
{
	TOKEN	token;
	int	token_class;

	out_white_space(first_token);		/* { for dumb vi */
	out_char('}');

		/* Check for END <procedure name>; */
	token_class = get_token(&token);
	if (token_class == IDENTIFIER) {
			/* END foo; where foo is a procedure */
		out_white_space(&token);
		out_str("/* ");
		out_token_name(&token);
		out_str(" */");
		token_class = get_token(&token);
	}

	if (token_class == END_OF_LINE)
		out_white_space(&token);
	else
		parse_error("';' expected");
}

/*
 *	IF statement
 */
parse_if(first_token)
TOKEN	*first_token;
{
	TOKEN	token;

	out_white_space(first_token);
	out_str("if (");

	if ((parse_expression(&token) != RESERVED) ||
		(token.token_type != THEN))
			parse_error("Missing THEN in IF statement");
	else {
		out_pre_line(&token);
		out_char(')');
		out_white_space(&token);
	}
}

/*
 *	THEN statement
 */
parse_then()
{
	parse_error("Illegal use of THEN");
}

/*
 *	ELSE statement
 */
parse_else(first_token)
TOKEN	*first_token;
{
	out_white_space(first_token);
	out_str("else");
}

/*
 *	GOTO statement
 */
parse_goto(first_token)
TOKEN	*first_token;
{
	TOKEN	token;

	out_white_space(first_token);
	out_str("goto");

	if (get_token(&token) != IDENTIFIER)
		parse_error("Illegal GOTO label");
	else {
		out_token(&token);
		check_eol();
	}
}

/*
 *	GO TO statement
 */
parse_go(first_token)
TOKEN	*first_token;
{
	TOKEN	token;

	if ((get_token(&token) != RESERVED) || (token.token_type != TO))
		parse_error("Illegal GO TO");
	else
		parse_goto(first_token);
}

/*
 *	CALL statement
 *	Handles CALL <procedure name> [ ( <parameter list> ) ] ;
 */
parse_call(first_token)
TOKEN	*first_token;
{
	TOKEN		token;
	int		token_class;
	DECL_MEMBER	*id_type;
	DECL_ID		*id_id;
	char		*new_func, *tmp_out_string;
	char		func_name[MAX_TOKEN_LENGTH];

		/* Get procedure name */
	token_class = get_token(&token);
	if (token_class != IDENTIFIER) {
		parse_error("Illegal procedure name");
		return;
	}

	out_white_space(first_token);

		/* Check for function conversion */
	if (check_cvt_id(&token, &cvt_functions[0], &new_func)) {
		out_str(new_func);
		token_class = get_token(&token);
	} else

	if (find_symbol(&token, &id_type, &id_id) &&
		(id_type->type->token_type != PROCEDURE)) {

			/* Skip white space */
		token.white_space_start = token.white_space_end;

			/* Check for call to pointer */
		func_name[0] = '\0';
		tmp_out_string = out_string;
		out_string = func_name;
		token_class = parse_variable(&token, &id_type, &id_id);
		out_string = tmp_out_string;

		if ((id_type->type->token_type == POINTER) ||
#ifdef OFFSET
			(id_type->type->token_type == OFFSET) ||
#endif
			(id_type->type->token_type == WORD)) {
				/* Yes - use pointer reference */
			out_str("(*");
			out_str(func_name);
			out_char(')');
		} else {
			parse_error("Illegal procedure reference");
			return;
		}
	} else {
		out_token_name(&token);
		token_class = get_token(&token);
	}

		/* Get parameter list (if any) */
	if (token_class == LEFT_PAREN) {
		out_token(&token);

		do {
			token_class = parse_expression(&token);
			out_token(&token);
		} while (token_class == COMMA);

		if (token_class == RIGHT_PAREN)
				/* Get end of line */
			check_eol();
		else
			parse_error("Illegal parameter list seperator");
	} else

	if (token_class == END_OF_LINE) {
			/* No parameter list */
		out_str("()");
		out_token(&token);
	} else
		parse_error("';' expected");
}

/*
 *	RETURN statement
 *	Handles RETURN [ <expression> ] ;
 */
parse_return(first_token)
TOKEN	*first_token;
{
	TOKEN	token;
	int	token_class;

	out_white_space(first_token);
	out_str("return");

	token_class = parse_expression(&token);
	if (token_class != END_OF_LINE)
		parse_error("';' expected");
	else
		out_token(&token);
}

/*
 *	Parse statement starting with an identifier.
 *	Possibilities include:
 *		Assignment
 *		Procedure statement
 */
parse_identifier(first_token)
TOKEN	*first_token;
{
	TOKEN		token, next_token;
	TOKEN		param_token, attrib_token, type_token;
	int		token_class, next_token_class;
	DECL		*decl_list, *extra_decl_list;
	PARAM_LIST	*param_list, *param_ptr;
	DECL_MEMBER	*decl_ptr;
	DECL_ID		*decl_id;
	BOOLEAN		extern_proc, got_type, interrupt_proc;
	char		*tmp_text_ptr;

		/* Check for label or procedure */
	tmp_text_ptr = text_ptr;
	token_class = get_token(&token);

	if (token_class == LABEL) {
			/* Determine if label or procedure definition */
		next_token_class = get_token(&next_token);
		if ((next_token_class == RESERVED) &&
			(next_token.token_type == PROCEDURE)) {
/*
 *	Procedure - Check for parameter list
 */
			param_list = NULL;
			token_class = get_token(&param_token);
			if (token_class == LEFT_PAREN) {
					/* Yes - get parameter list */
				get_param_list(&param_list);

					/* Get token after parameter list */
				token_class = get_token(&attrib_token);
			} else
					/* No param list - save as attribute */
				token_copy(&param_token, &attrib_token);

			out_white_space(first_token);
			extern_proc = FALSE;
			interrupt_proc = FALSE;

			got_type = (token_class == RESERVED) &&
				(attrib_token.token_type >= BYTE) &&
				(attrib_token.token_type <= SELECTOR);
			if (got_type) {
/*
 *	Process [ <type> ]
 */
				token_copy(&attrib_token, &type_token);
				token_class = get_token(&attrib_token);
			}

			while (token_class == RESERVED) {
			    if (attrib_token.token_type == INTERRUPT) {
/*
 *	Process [ <interrupt> ]
 */
				interrupt_proc = TRUE;
				token_class = get_token(&attrib_token);
				if (token_class == NUMERIC)
						/* Interrupt number */
					token_class = get_token(&attrib_token);
			    } else

/*
 *	Process [ EXTERNAL |  { [ PUBLIC ] [ REENTRANT ] } ]
 */
			    if (attrib_token.token_type == EXTERNAL) {
				out_str("extern");
				out_must_white(&attrib_token);
				extern_proc = TRUE;

				token_class = get_token(&attrib_token);
			    } else

			    if ((attrib_token.token_type == PUBLIC) ||
				    (attrib_token.token_type == REENTRANT)) {
				do {
				    if (attrib_token.token_type == PUBLIC) {
						/* Ignore for now */
				        token_class = get_token(&attrib_token);
				    } else

				    if (attrib_token.token_type == REENTRANT) {
						/* Ignore for now */
				        token_class = get_token(&attrib_token);
				    } else
					    break;
				} while (token_class == RESERVED);
			    } else
				break;
			}

			if (token_class != END_OF_LINE) {
				parse_error("';' expected");
				return;
			}

			if (interrupt_proc && !extern_proc)
				parse_warning("INTERRUPT procedure declared");

				/* Create declaration for procedure */
			get_element_ptr(&decl_ptr);
			get_var_ptr(&decl_ptr->name_list);
				/* Type = PROCEDURE */
			get_token_ptr(&decl_ptr->type);
			token_copy(&next_token, decl_ptr->type);
				/* Name = procedure name */
			get_token_ptr(&decl_ptr->name_list->name);
			token_copy(first_token, decl_ptr->name_list->name);
				/* Flag if parameter list */
			if (param_list)
				decl_ptr->initialization = DATA;
				/* Add it to context */
			add_to_context(decl_ptr);

			if (got_type) {
					/* Output procedure type */
				out_token_name(&type_token);
				out_must_white(&type_token);
			}

				/* Output procedure name */
			out_token_name(first_token);

			if (extern_proc) {
			    out_str("()");

			    if (param_list)
					/* Parse parameter declarations */
				parse_param_list(param_list, &decl_list,
					&extra_decl_list);

			    out_char(';');
				    /* Eat closing 'END [<proc name>];' */
			    token_class = get_token(&token);
			    if ((token_class != RESERVED) ||
				(token.token_type != END)) {
				parse_error("END expected");
				return;
			    }

			    out_white_space(&token);
			    token_class = get_token(&token);
			    if (token_class == IDENTIFIER) {
				token_class = get_token(&token);
			    }

			    if (token_class != END_OF_LINE) {
				parse_error("';' expected");
			    }

			    return;
			} else

			if (param_list) {
				out_token(&param_token);
					/* Output parameter list */
				param_ptr = param_list;
				while (param_ptr) {
					out_token(&param_ptr->param);
					param_ptr = param_ptr->next_param;
					if (param_ptr)
						out_char(',');
				}
				out_char(')');

				    /* Parse parameter declarations */
				parse_param_list(param_list, &decl_list,
					&extra_decl_list);

					/* Output declarations */
				if (decl_list) {
				    out_decl(decl_list);
					/* Add declarations to context */
				    add_decl_to_context(decl_list);
				}

				out_str("\n{");		/* } for dumb vi */

				if (extra_decl_list) {
				    out_decl(extra_decl_list);
					/* Add declarations to context */
				    add_decl_to_context(extra_decl_list);
				}

					/* Discard declarations */
				free_decl(decl_list);
				free_decl(extra_decl_list);
			} else
					/* No parameter list */
				out_str("()\n{");	/* } for dumb vi */

				/* Create new context */
			new_context(PROCEDURE, first_token);
				/* Parse statements to END */
			parse_to_end();
				/* Pop procedure context */
			pop_context();
		} else {
/*
 *	Label - add label name
 */
			out_token(first_token);
				/* Add colon */
			out_token(&token);

				/* Is this a defined label or a module? */
			if (find_symbol(first_token, &decl_ptr, &decl_id)) {
				if (decl_ptr->type->token_class == LABEL) {
						/* Label - new context */
					new_context(MODULE, first_token);
					parse_statement(&next_token);
					pop_context();
				} else {
					parse_error("Illegal label name");
					return;
				}
			} else
				parse_statement(&next_token);
		}
		return;
	}

		/* Assignment statement */
	text_ptr = tmp_text_ptr;
	token_copy(first_token, &token);
	token_class = parse_variable(&token, &decl_ptr, &decl_id);

		/* Check for multiple assignments */
	while (token_class == COMMA) {
			/* Print ' =' instead of ',' */
		out_str(" =");
		out_white_space(&token);
			/* Get identifier part of next assignment variable */
		token_class = get_token(&token);
		if (token_class != IDENTIFIER) {
			parse_error("Illegal assignment");
			return;
		}

			/* Parse remainder of variable (if any) */
		token_class = parse_variable(&token, &decl_ptr, &decl_id);
	}

	if (token_class == OPERATOR) {
		if (token.token_type != EQUAL) {
			parse_error("Illegal use of identifier");
			return;
		}

		out_token(&token);

			/* Check for POINTER assignment */
		if (decl_ptr->type->token_type == POINTER) {
				/* Yes - cast it */
			out_str(" (");
			out_str(TYPE_POINTER);
			out_str(" *) ");
		}

		if (parse_expression(&token) != END_OF_LINE)
			parse_error("';' expected");
		else
			out_token(&token);
		return;
	} else

	if (token_class != LABEL) {
		parse_error("Illegal use of identifier");
		return;
	}

}

/*
 *	Statement started with ':'
 */
parse_label()
{
	parse_error("Illegal label");
}

/*
 *	End of line (Null statement)
 */
parse_eol(first_token)
TOKEN	*first_token;
{
	out_white_space(first_token);
	out_char(';');
}

/*
 *	ENABLE or DISABLE statement
 */
parse_int_ctl(first_token)
TOKEN	*first_token;
{
	TOKEN	token;
	int	token_class;

	out_token(first_token);
	out_str("()");

	token_class = get_token(&token);
	if (token_class != END_OF_LINE) {
		parse_error("';' expected");
		return;
	}
	out_token(&token);
}

/*
 *	OUTPUT, OUTWORD or OUTHWORD statement of form:
 *		OUTPUT(port) = expr;
 */
parse_outport()
{
	TOKEN	token;
	int	token_class;

	if (get_token(&token) != LEFT_PAREN) {
		parse_error("'(' expected");
		return;
	}
	out_token(&token);

		/* Get port number */
	if (parse_expression(&token) != RIGHT_PAREN) {
		parse_error("'(' expected");
		return;
	}
	out_char(',');

	token_class = get_token(&token);
	if ((token_class != OPERATOR) || (token.token_type != EQUAL)) {
		parse_error("'=' expected");
		return;
	}

		/* Get expression */
	if (parse_expression(&token) != END_OF_LINE) {
		parse_error("'(' expected");
		return;
	}
	out_char(')');
	out_token(&token);
}

/*
 *	OUTPUT statement
 */
parse_output(first_token)
TOKEN	*first_token;
{
	out_white_space(first_token);
	out_str(FUNC_OUTPUT);
	parse_outport();
}

/*
 *	OUTWORD statement
 */
parse_outword(first_token)
TOKEN	*first_token;
{
	out_white_space(first_token);
	out_str(FUNC_OUTWORD);
	parse_outport();
}

/*
 *	OUTHWORD statement
 */
parse_outhword(first_token)
TOKEN	*first_token;
{
	out_white_space(first_token);
	out_str(FUNC_OUTHWORD);
	parse_outport();
}


