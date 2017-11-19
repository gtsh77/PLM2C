#include <stdio.h>
#include <string.h>
#include "misc.h"
#include "defs.h"
#include "cvt.h"
#include "struct.h"
#include "tokens.h"
#include "tkn_ext.h"

extern	BOOLEAN	syntax_error;

extern	char	*text_ptr;
extern	int	line_count;
extern	char	*line_ptr;
extern	char	current_file_name[];

/*
 *	parse_mesg() -	Print given message, message type, and current
 *			line number.  Skip to error_eol.
 */
parse_mesg(error_string, error_type, error_eol)
char	*error_string, *error_type;
char	error_eol;
{
	char	*err_ptr;
	int	i, offset;
	TOKEN	token;

	syntax_error = TRUE;

	offset = text_ptr - line_ptr - 1;

		/* Find end of line */
	for (err_ptr = line_ptr; (*err_ptr != '\0') &&
				 (*err_ptr != LF); err_ptr++) ;

	if (*error_string) {
		(void) fprintf(stderr, "\n%s - Parse %s: %s.\nOccurred at line %d near:\n",
			current_file_name, error_type, error_string, line_count);

			/* Print offending line */
		(void) fwrite(line_ptr, err_ptr - line_ptr + 1, 1, stderr);

		for (i = 0; i < offset; i++)
			if (line_ptr[i] < ' ')
				(void) fputc(line_ptr[i], stderr);
			else
				(void) fputc(' ', stderr);
		(void) fputc('^', stderr);

		if (*err_ptr == '\0')
			(void) fputc(LF, stderr);
	}

	if (*err_ptr != '\0')
		err_ptr++;

		/* Skip to end-of-line */
	if (error_eol == '\0')
		return;
	else

	if (error_eol == LF) {
		text_ptr = err_ptr;
		line_ptr = err_ptr;
		line_count++;
	} else {

		if (*(text_ptr - 1) != ';') {
			do {
				i = get_token(&token);
			} while ((i != END_OF_FILE) && (i != END_OF_LINE));
		}

			/* Point at end of line */
		text_ptr--;
	}
}

/*
 *	parse_error() -	Print given error message and current line number.
 *			Called when an unrecognised or unprocessable token
 *			appears.
 */
parse_error(error_string)
char	*error_string;
{
	if (syntax_error)
			/* Already had an error on this line */
		return;

	parse_mesg(error_string, "error", END_OF_LINE);
}

/*
 *	Do a parse_error(), but move to END_OF_LINE, not ';'
 */
control_error(error_string)
char	*error_string;
{
#ifdef IGNORE_CONTROL_ERRORS
	parse_mesg("", "", LF);
#else
	parse_mesg(error_string, "error", LF);
#endif
}

/*
 *	parse_warning - Generate a warning message
 */
parse_warning(warning_string)
char	*warning_string;
{
	parse_mesg(warning_string, "warning", '\0');
}

