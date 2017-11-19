#include "misc.h"
#include "defs.h"
#include "cvt.h"
#include "struct.h"
#include "tokens.h"
#include "tkn_ext.h"

extern	char	*text_buffer;
extern	char	*text_ptr;

/*
 *	Parse a control directive.
 *	Handles:		Abbreviation:
 *		$INCLUDE	$IC
 *		$SET
 *		$RESET
 *		$IF
 *		$ELSE
 *		$ELSEIF
 *		$ENDIF
 */
parse_control()
{
	TOKEN		ctl_token, token;
	int		token_class;
	RESERVED_WORD	*word_ptr;
	char		include_file[128], *include_ptr;

	token_class = get_token(&ctl_token);
	if (token_class != IDENTIFIER) {
		control_error("Invalid directive");
		return;
	}

	for (word_ptr = &control_directives[0]; word_ptr->token != END_OF_FILE;
			word_ptr++) {
	    if ((strlen(word_ptr->name) == ctl_token.token_length) &&
		!strncmp(word_ptr->name, ctl_token.token_start,
				ctl_token.token_length)) {
		switch (word_ptr->token) {

		case C_INCLUDE :
			token_class = get_token(&token);
			if (token_class != LEFT_PAREN) {
				control_error("'(' expected");
				return;
			}
				/* Copy and send file name (up to ')') */
			include_ptr = include_file;
			while (*text_ptr != ')') {
				if ((*text_ptr >= 'A') && (*text_ptr <= 'Z'))
						/* Convert to lower case */
					*include_ptr++ = *text_ptr++ + ' ';
				else
					*include_ptr++ = *text_ptr++;
			}
			*include_ptr++ = '\0';

				/* Skip ')' */
			text_ptr++;

				/* Parse include file */
			cvt_file(include_file);

				/* Convert .plm to .c */
			if (strcmp(include_ptr - 5, "plm")) {
				(void) strcpy(include_ptr - 5, ".c");
				include_ptr -= 2;
			}

			out_to_start();
			out_str("#include");
			out_must_white(&token);
			out_char('"');
			out_str(include_file);

			out_char('"');
			return;

		default :
			control_error("Non-supported directive");
			return;
		}
	    }
	}

	control_error("Invalid directive");
}

