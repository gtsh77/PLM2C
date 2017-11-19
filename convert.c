#include <stdio.h>
#ifdef IBMPC
#include <stdlib.h>
#endif
#include "misc.h"
#include "defs.h"
#include "cvt.h"
#include "struct.h"
#include "tokens.h"

BOOLEAN		syntax_error;

extern	char	*text_buffer, *text_ptr;
extern	int	line_count;

/*
 *	Determine statement type and call appropriate parse routine.
 *	Return statement class or, if a reserved word, reserved word token.
 */
parse_statement(first_token)
TOKEN	*first_token;
{
	int	token_type;

		/* Flush standard output and standard error */
	(void) fflush(stdout);
	(void) fflush(stderr);

		/* Flag no error yet */
	syntax_error = FALSE;

	switch (first_token->token_class) {

	case RESERVED:
		token_type = first_token->token_type;

		switch (token_type) {

		case DECLARE :
			parse_declare(first_token);
			break;

		case DO :
			parse_do(first_token);
			break;

		case IF :
			parse_if(first_token);
			break;

		case THEN :
			parse_then();
			break;

		case ELSE :
			parse_else(first_token);
			break;

		case GOTO :
			parse_goto(first_token);
			break;

		case GO :
			parse_go(first_token);
			break;

		case CALL :
			parse_call(first_token);
			break;

		case RETURN :
			parse_return(first_token);
			break;

		case END :
			parse_end(first_token);
			break;

		case DISABLE :
			parse_int_ctl(first_token);
			break;

		case ENABLE :
			parse_int_ctl(first_token);
			break;

		case OUTPUT :
			parse_output(first_token);
			break;

		case OUTWORD :
			parse_outword(first_token);
			break;

		case OUTHWORD :
			parse_outhword(first_token);
			break;

		default :
			parse_error("Illegal reserved word");
			return ERROR;
		}

		return token_type;

	case IDENTIFIER:
		parse_identifier(first_token);
		break;

	case LABEL:
		parse_label();
		break;

	case END_OF_LINE:
		parse_eol(first_token);
		break;

	case END_OF_FILE:
		out_white_space(first_token);
		return END_OF_FILE;

	default:
		parse_error("Illegal statement");
		return ERROR;

	}

	return first_token->token_class;
}

parse_new_statement()
{
	TOKEN		first_token;

		/* Get first token on line */
	(void) get_token(&first_token);

	return parse_statement(&first_token);
}

parse_file()
{
	while (parse_new_statement() != END_OF_FILE) ;
}

