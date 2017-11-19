#include <stdio.h>
#include <string.h>
#include "misc.h"
#include "defs.h"
#include "cvt.h"
#include "struct.h"
#include "tokens.h"
#include "tkn_ext.h"

BOOLEAN		parsing_literal;
TOKEN		literal_token, eof_token;
char		*lit_text_ptr;

extern	char	*text_buffer, *text_ptr;
extern	int	line_count;
extern	char	*line_ptr;
extern	char	current_file_name[];

/*
 *	get_token() -	Fetch a token from the buffer and return type,
 *			pointer and associated white space.
 */
get_token(token)
TOKEN	*token;
{
    RESERVED_WORD	*word_ptr;
    RESERVED_OPERATOR	*op_ptr;
    char		token_ch, last_token;
    char		*token_name_ptr;
    char		*op_name;
    BOOLEAN		got_fraction;
    BOOLEAN		cvt_case;
    char		id[MAX_TOKEN_LENGTH], *id_ptr;
    DECL_MEMBER		*decl_ptr;
    DECL_ID		*decl_id;
    int			token_class;
    char		*cvt_ptr;
    TOKEN		*token_ptr;

	/* Point to start of white space (if any) */
    token->white_space_start = text_ptr;
    token->white_space_end = text_ptr;

	/* Get first character */
    token_ch = *text_ptr++;

	/* Check for white space */
    while ((token_ch == SPACE) || (token_ch == TAB) || (token_ch == CR) ||
	   (token_ch == LF) || (token_ch == '$') ||
	  ((token_ch == '/') && (*text_ptr == '*'))) {

      if (token_ch == '$') {
			/* Check for a control directive */
		if ((text_ptr - 1 == text_buffer) ||
				(*(text_ptr - 2) == '\n')) {
			out_pre_white(token);
			parse_control();

				/* Reset start of white space */
			token->white_space_start = text_ptr;
			token->white_space_end = text_ptr;
		} else {
			parse_error("Illegal character");
			return ERROR;
		}
      } else {

	*(token->white_space_end++) = token_ch;

	if (token_ch == LF) {
			/* Increment input line count */
		line_count++;
			/* Point to start of line */
		line_ptr = text_ptr;
	} else

	if (token_ch == '/') {
			/* Comment - search to end */
			/* Add '*' of comment */
		token_ch = *(token->white_space_end++) = *text_ptr++;

		do {
			last_token = token_ch;
			token_ch = *(token->white_space_end++) = *text_ptr++;
			if (token_ch == LF) {
					/* Increment input line count */
				line_count++;
					/* Point to start of line */
				line_ptr = text_ptr;
			}
		} while ((token_ch != '/') || (last_token != '*'));
	}
      }

      token_ch = *text_ptr++;
    }


	/* Point to start of current token */
    token->token_start = text_ptr - 1;
	/* Point to start of converted token */
    token_name_ptr = token->token_name;

    if (is_a_char(token_ch)) {
		/* Process identifier */
#ifdef CONVERT_CASE
			/* Convert identifiers starting with an   */
			/* upper-case character to opposite case. */
		cvt_case = is_a_uc_char(token_ch);
#else
		cvt_case = FALSE;
#endif
	while (TRUE) {
		if (is_a_char(token_ch)) {
		    if (cvt_case) {
			if (is_a_uc_char(token_ch))
					/* Convert to lower-case character */
				*token_name_ptr++ = token_ch + ' ';
			else

					/* Convert to upper-case character */
				*token_name_ptr++ = token_ch - ' ';
		    } else
			*token_name_ptr++ = token_ch;
		} else

		if (is_a_digit(token_ch))
			*token_name_ptr++ = token_ch;
		else

		if (token_ch == '_')
			*token_name_ptr++ = token_ch;
		else

		if (token_ch == '$')
#ifdef CONVERT_DOLLAR
			*token_name_ptr++ = CONVERT_DOLLAR;
#else
			;
#endif
		else
			break;

		token_ch = *text_ptr++;
	}


		/* Mark end of token */
	text_ptr--;
	token->token_length = text_ptr - token->token_start;
	*token_name_ptr = '\0';

		/* Get a copy of identifier */
	(void) strcpy(id, token->token_name);
		/* If lower-case, convert to upper case for comparison */
	if (is_a_lc_char(*id)) {
		for (id_ptr = id; *id_ptr; id_ptr++)
			if (is_a_lc_char(*id_ptr))
				*id_ptr -= ' ';
	}

		/* Check for reserved word */
	for (word_ptr = &reserved_words[0]; word_ptr->token != END_OF_FILE;
			word_ptr++)
	{
		if (!strcmp(word_ptr->name, id)) {

			token->token_type = word_ptr->token;

				/* Check for reserved operator */
	    		switch (token->token_type) {

	    		case AND :
				op_name = AND_OP;
				break;

	    		case OR :
				op_name = OR_OP;
				break;

	    		case NOT :
				op_name = NOT_OP;
				break;

	    		case XOR :
				op_name = "^";
				break;

	    		case MOD :
				op_name = "%";
				break;

	    		case PLUS :
				parse_error("Cannot convert PLUS operator");
				token->token_class = token->token_type = ERROR;
				return ERROR;

	    		case MINUS :
				parse_error("Cannot convert MINUS operator");
				token->token_class = token->token_type = ERROR;
				return ERROR;

	    		default :
					/* Must not be an operator! */
				token->token_class = RESERVED;
				return RESERVED;
	    		}

				/* Switch to appropriate operator */
			(void) strcpy(token->token_name, op_name);
			token->token_class = OPERATOR;
			return OPERATOR;
		}
	}

		/* Not a reserved word - must be an identifier */
	token->token_class = token->token_type = IDENTIFIER;

		/* Check for a literal */
	if (!parsing_literal && find_symbol(token, &decl_ptr, &decl_id) &&
				(decl_ptr->type->token_type == LITERALLY)) {
#ifdef CONVERT_CASE
			/* Convert case of literal */
		for (cvt_ptr = token->token_name; *cvt_ptr;
				cvt_ptr++) {
			if (is_a_uc_char(*cvt_ptr))
				*cvt_ptr += 32;
			else
			if (is_a_lc_char(*cvt_ptr))
				*cvt_ptr -= 32;
		}
#endif
#ifdef PARSE_LITERALS
			/* Yes - Has literal been parsed? */
		if (decl_ptr->literal_token) {
				/* Yes - return parsed literal token */
				/* with token_name set to literal name */
			token_ptr = decl_ptr->literal_token;
			token->token_class = token_ptr->token_class;
			token->token_type = token_ptr->token_type;
			return token->token_class;
		}
#endif
			/* Is literal a single token? */
		lit_text_ptr = text_ptr;
		text_ptr = decl_ptr->literal;
		token_class = get_token(&literal_token);
		if (get_token(&eof_token) == END_OF_FILE) {
				/* Yes - return single token with */
				/* token_name set to literal name */
			token->token_class = token_class;
			token->token_type = literal_token.token_type;
			text_ptr = lit_text_ptr;
			parsing_literal = FALSE;
			return token->token_class;
		}

			/* No - parse complex literal and replace */
			/* Use of literal declaration */
		parsing_literal = TRUE;
		text_ptr = lit_text_ptr;
		parse_warning("Literal expanded");
		text_ptr = decl_ptr->literal;
		return get_token(token);
	}

	return IDENTIFIER;
    } else

    if (is_a_digit(token_ch)) {
		/* Process number */
		/* Flag not a floating point number */
	got_fraction = FALSE;

	while (TRUE) {
		if (is_a_digit(token_ch))
			*token_name_ptr++ = token_ch;
		else

		if (token_ch == '.') {
			got_fraction = TRUE;
			*token_name_ptr++ = token_ch;
		} else

		if ((token_ch == 'E') && got_fraction) {
				/* Process exponent */
			*token_name_ptr++ = token_ch;
				/* Signed exponent? */
			if ((*text_ptr != '+') && (*text_ptr != '-')) {
					/* No - default to + exponent */
				*token_name_ptr++ = '+';
			} else {
					/* Yes - add sign */
				token_ch = *text_ptr++;
				*token_name_ptr++ = token_ch;
			}
		} else

			/* Assume it's a hex char or constant designator */
		if (is_a_char(token_ch))
			*token_name_ptr++ = token_ch;
		else

		if (token_ch != '$')
			break;

		token_ch = *text_ptr++;
	}

		/* Point to last character in constant */
	token_name_ptr--;
	token_ch = *token_name_ptr;

	if (got_fraction) {
			/* Floating point - add suffix */
		*++token_name_ptr = 'F';
			/* Mark end of token */
		*++token_name_ptr = '\0';
	} else

	if (token_ch == 'B') {
		parse_error("Binary constant");
		token->token_class = token->token_type = ERROR;
		return ERROR;
	} else

	if ((token_ch == 'O') || (token_ch == 'Q')) {
			/* Octal constant */
			/* Mark end of token */
		*token_name_ptr++ = '\0';
			/* Move constant up 1 character */
		while (token_name_ptr != token->token_name) {
			*token_name_ptr = *(token_name_ptr - 1);
			token_name_ptr--;
		}

			/* Make a C octal constant */
		*token_name_ptr = '0';
	} else

	if (token_ch == 'H') {
			/* Hex constant */
			/* Mark end of token */
		*token_name_ptr++ = '\0';
		token_name_ptr++;
			/* Move constant up 2 characters */
		while (token_name_ptr != (token->token_name + 1)) {
			*token_name_ptr = *(token_name_ptr - 2);
			token_name_ptr--;
		}

			/* Make a C hex constant */
		*token_name_ptr-- = 'x';
		*token_name_ptr = '0';
	} else

	if (token_ch == 'D')
			/* Decimal constant - ignore 'D' */
		*token_name_ptr = '\0';
	else
			/* Regular constant */
		*++token_name_ptr = '\0';

		/* Mark end of token */
	text_ptr--;
	token->token_length = text_ptr - token->token_start;

	token->token_class = token->token_type = NUMERIC;
	return NUMERIC;
    } else {

		/* Check for operator */
	for (op_ptr = &reserved_operators[0]; op_ptr->name != END_OF_FILE;
				op_ptr++) {
		token->token_length = strlen(op_ptr->operator);
		if (!strncmp(text_ptr - 1, op_ptr->operator,
			token->token_length)) {
				/* Found operator */
				/* Save converted type */
			(void) strcpy(token->token_name, op_ptr->cvt_operator);
			token->token_type = op_ptr->name;
				/* Point past operator */
			text_ptr += token->token_length - 1;
				
			token->token_class = OPERATOR;
			return OPERATOR;
		}
	}

		/* Assume single character token */
	*token_name_ptr++ = token_ch;
	*token_name_ptr = '\0';
		/* Mark end of token so far */
	token->token_length = 1;


	switch (token_ch) {

	case ';' :
		token->token_class = token->token_type = END_OF_LINE;
		return END_OF_LINE;

	case ':' :
		token->token_class = token->token_type = LABEL;
		return LABEL;

	case ',' :
		token->token_class = token->token_type = COMMA;
		return COMMA;

	case '.' :
		token->token_class = token->token_type = PERIOD;
		return PERIOD;

	case '(' :
		token->token_class = token->token_type = LEFT_PAREN;
		return LEFT_PAREN;

	case ')' :
		token->token_class = token->token_type = RIGHT_PAREN;
		return RIGHT_PAREN;

	case '\'' :
			/* String constant */
		token_name_ptr--;
		while (1) {
			if (*text_ptr == '\'') {
				if ((*(text_ptr + 1) == '\''))
					text_ptr++;
				else
					break;
			}
			*token_name_ptr++ = *text_ptr++;
		}

		text_ptr++;
		*token_name_ptr++ = '\0';
		token->token_length = strlen(token->token_name);

		token->token_class = token->token_type = STRING;
		return STRING;

	case 0:
		if (parsing_literal) {
				/* Done parsing literal -  */
				/* Switch back to text_ptr */
			parsing_literal = FALSE;
			text_ptr = lit_text_ptr;
			return get_token(token);
		}
		token->token_class = token->token_type = END_OF_FILE;
		return END_OF_FILE;

	default:
		parse_error("Illegal character");
			/* Eat the evidence */
		token->token_name[0] = '\0';
		token->token_class = token->token_type = ERROR;
		return ERROR;
	}
    }
}

/*
 *	Copy source token to destination token
 */
token_copy(src, dest)
TOKEN	*src, *dest;
{
	dest->token_class = src->token_class;
	dest->token_type = src->token_type;
	(void) strcpy(dest->token_name, src->token_name);
	dest->token_start = src->token_start;
	dest->token_length = src->token_length;
	dest->white_space_start = src->white_space_start;
	dest->white_space_end = src->white_space_end;
}

