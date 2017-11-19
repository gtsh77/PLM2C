#include "misc.h"
#include "defs.h"
#include "cvt.h"
#include "struct.h"
#include "tokens.h"

extern	char	*text_ptr;
extern	int	at_decl_count;
extern	char	at_decl_list[MAX_AT_DECLS][MAX_TOKEN_LENGTH];

/*
 *	Output *<name> if use_parens == NULL, else (*<name>).
 */
out_pointer(name, use_parens)
TOKEN	*name;
BOOLEAN	use_parens;
{
		/* Use parentheses? */
	if (use_parens) {
			/* Yes - make it (*name) */
		out_str("(*");
		out_token_name(name);
		out_char(')');
	} else {
			/* No - make it *name */
		out_char('*');
		out_token_name(name);
	}
}

/*
 *	Output array bound (if any)
 */
out_bound(bound)
TOKEN	*bound;
{
	if (bound) {
		out_char('[');
		out_token(bound);
		out_char(']');
	}
}

/*
 *	Output a declaration type.
 */
out_decl_type(decl_ptr)
DECL_MEMBER	*decl_ptr;
{
	if (decl_ptr->type->token_type != STRUCTURE) {
		out_type(decl_ptr->type->token_type);
	} else {
		out_struct(decl_ptr->struct_list);
	}
}

/*
 *	Output structure contents.
 */
out_struct(el_ptr)
DECL_MEMBER	*el_ptr;
{
	DECL_ID		*var_ptr;

	out_str("struct {");

	while (el_ptr) {
			/* Use initial white space before type */
		var_ptr = el_ptr->name_list;
		if (var_ptr)
			out_must_white(var_ptr->name);

		out_decl_type(el_ptr);
		out_char(' ');

		while (var_ptr) {
			out_token_name(var_ptr->name);
			out_bound(el_ptr->array_bound);
			var_ptr = var_ptr->next_var;
			if (var_ptr) {
				out_char(',');
				out_must_white(var_ptr->name);
			}
		}
		if ((el_ptr = el_ptr->next_member) != NULL)
			out_char(';');
	}
	out_char('}');
}

/*
 *	Output C declaration list member.
 */
out_decl_member(decl_list, decl_token)
DECL_MEMBER	*decl_list;
TOKEN		*decl_token;
{
	int	i;
	TOKEN	token, tmp_token;
	int	token_class;
	int	name_count;
	char	count_str[8];
	DECL_ID	*var_ptr;
	char	*tmp_white_start, *tmp_white_end;
	char	*tmp_text_ptr;
	BOOLEAN	typedefed, is_at;
	int	string_len, string_size;
	char	*string_ptr;

		/* Output characters up to CR */
	out_pre_white(decl_token);

	if (decl_list->type->token_type == LABEL)
			/* Ignore label declarations */
		return;

	var_ptr = decl_list->name_list;

	if (decl_list->type->token_type == LITERALLY) {
			/* Make sure we're at start of new line */
		out_pre_white(var_ptr->name);
		out_to_start();

			/* Convert to a #define */
		out_str("#define ");
		out_cvt_name(var_ptr->name);
		out_char(' ');
		out_str(decl_list->literal);
/*
		out_str("\n");
*/
		return;
	}

	var_ptr->name->white_space_start = decl_token->white_space_start;
	var_ptr->name->white_space_end = decl_token->white_space_end;

		/* Swap white space between type and first identifier */
		/* and eat any new_lines prior to first identifier */
	tmp_white_start = decl_list->type->white_space_start;
	tmp_white_end = decl_list->type->white_space_end;

	while ((tmp_white_start < tmp_white_end) && (*tmp_white_start < ' '))
		tmp_white_start++;

	decl_list->type->white_space_start = var_ptr->name->white_space_start;
	var_ptr->name->white_space_start = tmp_white_start;
	decl_list->type->white_space_end = var_ptr->name->white_space_end;
	var_ptr->name->white_space_end = tmp_white_end;

	out_white_space(decl_list->type);

	if (decl_list->attributes == EXTERNAL) {
		out_str("extern ");

			/* Check if declared AT in another module */
		for (i = 0; i < at_decl_count; i++)
			if (!strcmp(var_ptr->name->token_name, at_decl_list[i]))
					/* Yes - flag as so */
				var_ptr->is_ext_at = TRUE;
	} else

	if (decl_list->initialization == DATA) {
		out_str(TYPE_DATA);
		out_char(' ');
	}


	is_at = (decl_list->at_ptr != NULL) || var_ptr->is_ext_at;

		/* Determine if a structure with an AT attribute */
	typedefed = (decl_list->type->token_type == STRUCTURE) && is_at;

		/* Output type */
		/* Is this a structure with an AT attribute? */
	if (typedefed) {
			/* Yes - output typedefed structure */
		out_str("typedef ");
		out_struct(decl_list->struct_list);
		out_must_white(var_ptr->name);
#ifdef USE_DEFINES
		out_char('_');
#endif
		out_cvt_name(var_ptr->name);
		if (decl_list->array_bound)
			out_bound(decl_list->array_bound);
		out_str(";\n");
		out_white_space(decl_token);
#ifdef USE_DEFINES
		out_char('_');
#endif
		out_cvt_name(var_ptr->name);
	} else
		out_decl_type(decl_list);

		/* Walk through name list */
	name_count = 0;
	while (var_ptr) {
		if (is_at) {
				/* AT (<expression>) -
				   OK... don't panic...
				   we can handle this
				*/
/*
 * Output:
 *	<type> *<ident> = (<type> *) <AT expr> + name_count
 *
 * NOTE: BASED variables are not dealt with.
 */
			out_must_white(var_ptr->name);
				/* Is this an array? */
			if ((decl_list->array_bound) && !typedefed)
					/* Yes - output ( *<ident> ) */
				out_char('(');
			out_char('*');
#ifdef USE_DEFINES
				/* Output case converted name */
			out_cvt_name(var_ptr->name);
#else
			out_token_name(var_ptr->name);
#endif
			if ((decl_list->array_bound) && !typedefed) {
				out_char(')');
					/* Output array bound (if any) */
				out_bound(decl_list->array_bound);
			}

			if (decl_list->attributes != EXTERNAL) {
			    out_str(" = (");
				/* Is this a structure? */
			    if (decl_list->type->token_type == STRUCTURE) {
					/* Yes - output structure name prefix */
#ifdef USE_DEFINES
				out_char('_');
#endif
				out_cvt_name(decl_list->name_list->name);
			    } else
				out_decl_type(decl_list);
			    out_str(" *) ");

			    out_str(decl_list->at_ptr);
			    if (name_count) {
				(void) sprintf(count_str, " + %d", name_count);
				out_str(count_str);
			    }
			}
		} else {
				/* Not an AT expression (whew!) */
			out_must_white(var_ptr->name);

				/* Is variable based? */
			if (var_ptr->based_name) {
					/* Yes - Output **name = */
					/*	(type **) &based_name */
				if (decl_list->array_bound) {
						/* Use (**name)[] */
					out_str("(**");
					out_token_name(var_ptr->name);
					out_str(")[]");
				} else {
					out_str("**");
					out_token_name(var_ptr->name);
				}

				out_str(" = (");
				out_decl_type(decl_list);
				out_str(" **) &");
				out_token_name(var_ptr->based_name);
			} else

			if (decl_list->type->token_type == POINTER) {
					/* Yes - if based on an array */
					/* output (*name) else output *name */
				out_pointer(var_ptr->name,
					(BOOLEAN) decl_list->array_bound);
			} else {
					/* Output variable name */
				out_token_name(var_ptr->name);

					/* Output array bound (if any) */
				out_bound(decl_list->array_bound);
			}
		}

			/* Get next name */
		if ((var_ptr = var_ptr->next_var) != NULL) {
			out_char(',');
			name_count++;
		}
	}

		/* Check for INITIAL or DATA initializers */
	if (decl_list->initialization != NONE) {
		out_str(" = ");
			/* Point back to initializer string */
		tmp_text_ptr = text_ptr;
		text_ptr = decl_list->init_ptr;
		if (decl_list->array_bound) {
			out_char('{');
				/* Array - determine if just a single string */
			switch (decl_list->type->token_type) {

			case BYTE :
				string_size = SIZE_BYTE;
				break;

			case WORD :
				string_size = SIZE_WORD;
				break;

			case DWORD :
				string_size = SIZE_DWORD;
				break;

			case STRUCTURE :
/*
 *  Oh, SH-T!!  fake it!
 */
				string_size = SIZE_BYTE;
				break;

			default :
				string_size = 0;
			}

			if (string_size && (get_token(&token) == STRING) &&
				(get_token(&tmp_token) == RIGHT_PAREN)) {
					/* Single string - break up into */
					/* Pieces of sizeof(<type>) size */
				string_ptr = token.token_name;
				string_len = token.token_length;
				while (string_len) {
					out_str_const(string_ptr, string_size);
					if (string_size > string_len)
						string_size = string_len;
					string_ptr += string_size;
					if (string_len -= string_size)
						out_char(',');
				}
			} else {
					/* Point back to init string */
				text_ptr = decl_list->init_ptr;
				do {
					token_class = parse_expression(&token);
					if (token_class == COMMA)
						out_token(&token);
				} while (token_class == COMMA);
			}

			out_char('}');
				/* Point past init string */
			text_ptr = token.token_start + token.token_length + 2;
			token_class = get_token(&token);
		} else {
			token_class = parse_expression(&token);
		}
		if (token_class != RIGHT_PAREN)
			parse_error("')' expected");
		text_ptr = tmp_text_ptr;
	}

	out_char(';');

#ifdef USE_DEFINES
		/* Walk through name list and check for BASED variables */
	var_ptr = decl_list->name_list;
	while (var_ptr) {
			/* See if variable is BASED */
		if (var_ptr->based_name) {
				/* Make sure we're at start of new line */
			out_to_start();
			out_str("#define");
			out_must_token(var_ptr->based_name);
			out_white_space(var_ptr->name);
			out_str("(*");
			out_token_name(var_ptr->name);
			out_str(")\n");
		}

			/* See if variable is AT */
		if (is_at) {
				/* Make sure we're at start of new line */
			out_to_start();
			out_str("#define");
			out_must_token(var_ptr->name);
			out_white_space(var_ptr->name);
			out_str("(*");
			out_cvt_name(var_ptr->name);
			out_str(")\n");
		}

		var_ptr = var_ptr->next_var;
	}
#endif
}

out_decl(decl)
DECL		*decl;
{
	DECL_MEMBER	*decl_list;

	while (decl) {
		for (decl_list = decl->decl_list; decl_list;
				decl_list = decl_list->next_member)
			out_decl_member(decl_list, decl->decl_token);
		decl = decl->next_decl;
	}
}
