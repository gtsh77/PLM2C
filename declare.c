#include "misc.h"
#include "defs.h"
#include "cvt.h"
#include "struct.h"
#include "tokens.h"

extern	char	*text_ptr;
extern	char	*out_string;

/*
 *	Routines to process DECLARE statements.
 */

/*
 *	Skip to closing right parenthesis
 */
find_right_paren()
{
	TOKEN	token;
	int	token_class;
	int	paren_count;

	paren_count = 1;
	do {
		token_class = get_token(&token);
		if (token_class == LEFT_PAREN)
			paren_count++;
		else
		if (token_class == RIGHT_PAREN)
			paren_count--;
	} while (paren_count);
}

/*
 *	Copy an element from source to destination
 */
element_copy(src, dest)
DECL_MEMBER	*src, *dest;
{
		/* Don't copy name list */
	dest->name_list = NULL;
		/* Don't copy link */
	dest->next_member = NULL;
	dest->literal = src->literal;
	dest->array_bound = src->array_bound;
	dest->type = src->type;
	dest->attributes = src->attributes;
	dest->initialization = src->initialization;
	dest->at_ptr = src->at_ptr;
	dest->init_ptr = src->init_ptr;
	if (src->struct_list)
		element_copy(src->struct_list, dest->struct_list);
}

/*
 *	Generate a linked list of variables of the form:
 *		<id> [BASED <id>[.<id>]] or
 *		( <id> [BASED <id>[.<id>]] [ ,<id> [BASED <id>[.<id>]] ] ... )
 *	Return token following variable list.
 */
get_var_list(list_ptr, sep_token)
DECL_ID	**list_ptr;
TOKEN	*sep_token;
{
	DECL_ID	*var_ptr, *last_var;
	TOKEN	*token;
	int	token_class;
	BOOLEAN	multi_list;
	char	*par_white_start, *par_white_end;

	*list_ptr = NULL;
		/* Get first token */
	get_token_ptr(&token);
	token_class = get_token(token);

		/* Determine if <var> or list of ( <var> [,<var>] ... ) */
	if (token_class == LEFT_PAREN) {
			/* List of ( <var> [,<var>] ... ) */
		multi_list = TRUE;

			/* Use white space before '(' for first identifier */
		par_white_start = token->white_space_start;
		par_white_end = token->white_space_end;

			/* Get first identifier */
		token_class = get_token(token);
		token->white_space_start = par_white_start;
		token->white_space_end = par_white_end;
	} else
			/* <var> */
		multi_list = FALSE;

		/* Process identifier list */
	last_var = NULL;
	while (1) {
		if (token_class != IDENTIFIER) {
			parse_error("Identifier expected");
			free_var_list(*list_ptr);
			free((char *) token);
			*list_ptr = NULL;
			return ERROR;
		}

			/* Get a variable structure */
		get_var_ptr(&var_ptr);

		if (*list_ptr == NULL)
				/* Point to first variable */
			*list_ptr = var_ptr;

		if (last_var)
			last_var->next_var = var_ptr;
		last_var = var_ptr;

			/* Save variable name */
		var_ptr->name = token;

			/* Check for BASED */
		token_class = get_token(sep_token);

		if ((token_class == RESERVED) &&
			(sep_token->token_type == BASED)) {
				/* BASED <id>[ .<id> ] */
				/* Get based name */
			get_token_ptr(&token);
			token_class = get_token(token);
			if (token_class != IDENTIFIER) {
				parse_error("Identifier expected");
				free_var_list(*list_ptr);
				free((char *) token);
				*list_ptr = NULL;
				return ERROR;
			}
			token_class = parse_simple_variable(token, sep_token);

#ifdef USE_DEFINES
				/* Swap variable name with based name */
			var_ptr->based_name = var_ptr->name;
			var_ptr->name = token;
#else
			var_ptr->based_name = token;
#endif
		}

		if (!multi_list)
			return token_class;

		if (token_class != COMMA)
			break;

			/* Get next variable */
		get_token_ptr(&token);
		token_class = get_token(token);
	}

	if (token_class == RIGHT_PAREN) {
			/* Get next token */
		token_class = get_token(sep_token);
		return token_class;
	} else {
		parse_error("')' expected");
		free_var_list(*list_ptr);
		*list_ptr = NULL;
		return ERROR;
	}
}

/*
 *	Parse a structure declaration of the form:
 *	STRUCTURE ( <member> [ ,<member> ] ... )
 *		where:
 *	<member> ::= { <id> | ( <id> [ ,<id> ] ... ) } [ ( <numeric> ) ] <type>
 */
parse_structure(list_ptr)
DECL_MEMBER	**list_ptr;
{
	DECL_MEMBER	*struct_ptr, *last_struct;
	TOKEN		token;
	int		token_class;

	*list_ptr = NULL;

		/* Get left paren */
	token_class = get_token(&token);
	if (token_class != LEFT_PAREN) {
		parse_error("'(' expected");
		return;
	}

	last_struct = NULL;
	do {
			/* Get a DECL_MEMBER structure */
		get_element_ptr(&struct_ptr);

		if (*list_ptr == NULL)
				/* Point to first structure */
			*list_ptr = struct_ptr;

		if (last_struct)
			last_struct->next_member = struct_ptr;
		last_struct = struct_ptr;

			/* Get variable list */
		token_class = get_var_list(&struct_ptr->name_list, &token);

			/* Get type and optional array designator */
		get_token_ptr(&struct_ptr->type);
		token_class = parse_type(struct_ptr, &token);

			/* Get seperator */
		token_class = get_token(&token);
	} while (token_class == COMMA);

	if (token_class != RIGHT_PAREN) {
		parse_error("'(' expected");
		free_decl_list(*list_ptr);
		*list_ptr = NULL;
		return;
	}
}

/*
 *	Parse type and optional array designator.
 *	Passed initial token.
 *	Returns RESERVED if appropriate type found, else returns END_OF_LINE.
 */
parse_type(el_ptr, token)
DECL_MEMBER	*el_ptr;
TOKEN		*token;
{
	TOKEN	*temp_token;
	int	token_class;

	token_class = token->token_class;
	if (token_class == LEFT_PAREN) {
			/* Array specifier */
			/* Get numeric or '*' */
		get_token_ptr(&temp_token);
		token_class = get_token(temp_token);

		if ((token_class == NUMERIC) ||
			((token_class == OPERATOR) &&
				(temp_token->token_type == TIMES))) {
			if (token_class != NUMERIC)
				/* array(*) specified - ignore '*' */
				temp_token->token_name[0] = '\0';

				/* Save array bound token */
			el_ptr->array_bound = temp_token;
		} else {
			parse_error("Illegal array bound");
			free((char *) temp_token);
			return ERROR;
		}

			/* Get right paren */
		token_class = get_token(token);
		if (token_class != RIGHT_PAREN) {
			parse_error("')' expected");
			free((char *) temp_token);
			return ERROR;
		}

			/* Get type */
		token_class = get_token(token);
	}

	if ((token_class == RESERVED) && (token->token_type >= BYTE) &&
					 (token->token_type <= STRUCTURE)) {

			/* Save type token */
		token_copy(token, el_ptr->type);

		if (token->token_type == STRUCTURE) {
				/* Get structure list */
			parse_structure(&el_ptr->struct_list);
		}
		return token_class;
	} else {
		parse_error("Illegal type");
		return ERROR;
	}
}

/*
 *	Parse a DECLARE element.
 *	Return token terminating DECLARE element.
 */
get_element(element, token)
DECL_MEMBER	**element;
TOKEN		*token;
{
	DECL_MEMBER	*el_ptr;
	TOKEN		temp_token, eof_token;
	int		token_class;
	char		*tmp_text_ptr;
	char		*tmp_out_string;

	char		*get_mem();

	get_element_ptr(element);

		/* Point to element */
	el_ptr = *element;

		/* Get name list */
	token_class = get_var_list(&el_ptr->name_list, token);

		/* Malloc space for type */
	get_token_ptr(&el_ptr->type);

	if (token_class == RESERVED)
	    switch (token->token_type) {

	    case LABEL :
			/* LABEL declaration */
		token_copy(token, el_ptr->type);

			/* Check for PUBLIC or EXTERNAL */
		token_class = get_token(token);
		if ((token_class == RESERVED) &&
			((token->token_type == PUBLIC) ||
			 (token->token_type == EXTERNAL)))
				/* Indeed, who cares? */
			token_class = get_token(token);
		return token_class;

	    case LITERALLY :
		token_copy(token, el_ptr->type);

			/* Check for 'string' */
		if (get_token(token) != STRING) {
			parse_error("String expected");
			free_decl_list(el_ptr);
			return ERROR;
		}

		el_ptr->literal = get_mem(MAX_LITERAL_SIZE);
#ifdef PARSE_LITERALS
			/* Parse literal string if only one token in string */
		tmp_text_ptr = text_ptr;
		text_ptr = token->token_name;

			/* Parse token in string */
		if (get_token(&temp_token) == END_OF_FILE)
			el_ptr->literal[0] = '\0';
		else
		if (get_token(&eof_token) == END_OF_FILE) {
				/* Single token literal */
			(void) strcpy(el_ptr->literal, temp_token.token_name);
				/* Save parsed token */
			get_token_ptr(&el_ptr->literal_token);
			token_copy(&temp_token, el_ptr->literal_token);
		} else
			(void) strcpy(el_ptr->literal, token->token_name);

		text_ptr = tmp_text_ptr;
#else
			/* Put string in literal */
		(void) strcpy(el_ptr->literal, token->token_name);
#endif

			/* Return following token */
		token_class = get_token(token);
		return token_class;
	}

	if (parse_type(el_ptr, token) != RESERVED) {
			/* Error occurred */
		free_decl_list(el_ptr);
		return END_OF_LINE;
	}

		/* Process attribute information (if any) */
		/* Check for EXTERNAL [ DATA ] */
	token_class = get_token(token);
	if (token_class != RESERVED)
		return token_class;

	if (token->token_type == EXTERNAL) {
		el_ptr->attributes = EXTERNAL;

			/* Check for DATA attribute */
		token_class = get_token(token);
		if (token_class == RESERVED) {
			if (token->token_type == DATA) {
/*
 *	Ignore attribute
 *				el_ptr->initialization = DATA;
 */
				token_class = get_token(token);
			} else {
				parse_error("Illegal attribute");
				free_decl_list(el_ptr);
				return ERROR;
			}
		}

		return token_class;
	} else

		/* Check for PUBLIC */
	if (token->token_type == PUBLIC) {
		el_ptr->attributes = PUBLIC;
		token_class = get_token(token);
	}

	if (token_class != RESERVED)
		return token_class;

		/* Check for AT ( <expr> ) */
	if (token->token_type == AT) {
			/* Check for '(' */
		token_class = get_token(token);

		if (token_class != LEFT_PAREN) {
				parse_error("'(' expected");
				free_decl_list(el_ptr);
				return ERROR;
		}

			/* Generate a string for the AT expression */
		el_ptr->at_ptr = get_mem(MAX_AT_EXPR_SIZE);
		el_ptr->at_ptr[0] = '\0';
		tmp_out_string = out_string;
		out_string = el_ptr->at_ptr;

			/* Parse the expression into at_ptr */
		token_class = parse_expression(token);
		if (token_class != RIGHT_PAREN) {
			parse_error("')' expected");
			free_decl_list(el_ptr);
			return ERROR;
		}
		out_string = tmp_out_string;
		token_class = get_token(token);
	}

	if (token_class != RESERVED)
		return token_class;

		/* Check for INITIAL or DATA ( <expr> ) */
	if ((token->token_type == INITIAL) ||
		(token->token_type == DATA)) {

		el_ptr->initialization = token->token_type;

			/* Check for '(' */
		token_class = get_token(token);

		if (token_class != LEFT_PAREN) {
				parse_error("'(' expected");
				free_decl_list(el_ptr);
				return ERROR;
		}

		el_ptr->init_ptr = text_ptr;
			/* Skip to ')' */
		find_right_paren();
		token_class = get_token(token);
	}
	return token_class;
}

/*
 *	Parse a DECLARE list.
 *	Passed a pointer to a DECL, returns with DECL filled.
 */
get_decl_list(decl)
DECL	*decl;
{
	DECL_MEMBER	*el_ptr, *decl_ptr;
	TOKEN		token;
	int		token_class;

	decl->decl_list = NULL;
	decl->next_decl = NULL;
	decl_ptr = NULL;

	do {
			/* Get a declaration element */
		token_class = get_element(&el_ptr, &token);

		if (decl->decl_list == NULL)
			decl->decl_list = el_ptr;

			/* Link previous element */
		if (decl_ptr)
			decl_ptr->next_member = el_ptr;
		decl_ptr = el_ptr;
	} while (token_class == COMMA);
}

/*
 *	Parse a DECLARE statement.
 */
parse_declare(first_token)
TOKEN	*first_token;
{
	DECL		decl;

	decl.decl_token = first_token;
	get_decl_list(&decl);
	out_decl(&decl);
	add_to_context(decl.decl_list);
}
