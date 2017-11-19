#include <stdio.h>
#ifdef IBMPC
#include <stdlib.h>
#endif
#include "misc.h"
#include "defs.h"
#include "cvt.h"
#include "struct.h"
#include "tokens.h"

char	*out_string;
char	last_out_ch;

char	*str_shifts[] = { "0", "8", "16", "24" };

extern	char	*text_buffer, *text_ptr;
extern	int	line_count;
extern	int	file_depth;
extern	FILE	*ofd;

extern	BOOLEAN	parsing_literal;
extern	TOKEN	literal_token;

/*
 *	Output data of specified length.
 *	If out_string is not NULL, append string to out_string.
 *	Otherwise write string to stdout.
 */
out_data(string, length)
char	*string;
int	length;
{
	if (length) {
		if (out_string)
			(void) strncat(out_string, string, length);
		else
		if (file_depth == 1)
#ifdef DEBUG
			(void) fwrite(string, length, 1, stdout);
#else
			(void) fwrite(string, length, 1, ofd);
#endif
		else
			return;

			/* Save last character output */
		last_out_ch = *(string + length - 1);
	}
}

/*
 *	Print white space
 */
out_white_space(token)
TOKEN	*token;
{
	int	length;

		/* Compute length of white space */
	length = token->white_space_end - token->white_space_start;

	if (length)
		out_data(token->white_space_start, length);
}

/*
 *	Print white space, if any.  If start of white space string is not
 *	white, prefix with a space.
 */
out_must_white(token)
TOKEN	*token;
{
	if (!is_white(*(token->white_space_start)))
		out_char(' ');
	out_white_space(token);
}

/*
 *	Print all white space up first new-line (if any).
 *	Move white_space_start to point past first new-line.
 */
out_pre_line(token)
TOKEN	*token;
{
	while ((token->white_space_start < token->white_space_end) &&
		(*token->white_space_start != '\n')) {
		out_char(*token->white_space_start);
		token->white_space_start++;
	}
}

/*
 *	Print all white space up to but not including last new-line.
 *	Move white_space_start to point to last new-line.
 */
out_pre_white(token)
TOKEN	*token;
{
	char	*ptr;
	int	length;

	for (ptr = token->white_space_end;
		(ptr > token->white_space_start) && (*(ptr - 1) != '\n') ; )
			ptr--;

	if (ptr == token->white_space_start)
		return;

		/* Compute length of white space */
	length = ptr - token->white_space_start - 1;

	if (length)
		out_data(token->white_space_start, length);

	token->white_space_start = ptr - 1;
}

/*
 *	Output token name
 */
out_token_name(token)
TOKEN	*token;
{
	if (is_a_type(token))
		out_type(token->token_type);
	else
		out_data(token->token_name, strlen(token->token_name));
}

/*
 *	Output white space and token name
 */
out_token(token)
TOKEN	*token;
{
	out_white_space(token);
	out_token_name(token);
}

/*
 *	Output guaranteed white space and token name
 */
out_must_token(token)
TOKEN	*token;
{
	out_must_white(token);
	out_token_name(token);
}

/*
 *	Output case converted token name
 */
out_cvt_name(token)
TOKEN	*token;
{
	char	*ptr;

	for (ptr = token->token_name; *ptr; ptr++) {
		if (is_a_lc_char(*ptr))
			out_char(*ptr - 32);
		else
		if (is_a_uc_char(*ptr))
			out_char(*ptr + 32);
		else
			out_char(*ptr);
	}
}

/*
 *	Output string
 */
out_str(string)
char	*string;
{
	out_data(string, strlen(string));
}

/*
 *	Output character
 */
out_char(ch)
char	ch;
{
	out_data(&ch, 1);
}

/*
 *	Output new-line if not at start of line
 */
out_to_start()
{
	if (last_out_ch != LF)
		out_char(LF);
}

/*
 *	Output type
 */
out_type(type)
int	type;
{
	switch (type) {

	case BYTE :
#ifdef CONVERT_TYPES
		out_str(TYPE_BYTE);
#else
		out_str("BYTE");
#endif
		break;

	case WORD :
#ifdef CONVERT_TYPES
		out_str(TYPE_WORD);
#else
		out_str("WORD");
#endif
		break;

	case DWORD :
#ifdef CONVERT_TYPES
		out_str(TYPE_DWORD);
#else
		out_str("DWORD");
#endif
		break;

	case INTEGER :
#ifdef CONVERT_TYPES
		out_str(TYPE_INTEGER);
#else
		out_str("INTEGER");
#endif
		break;

	case REAL :
#ifdef CONVERT_TYPES
		out_str(TYPE_REAL);
#else
		out_str("REAL");
#endif
		break;

	case POINTER :
		out_str(TYPE_POINTER);
		break;

	default :
		parse_error("Unknown type");
	}
}

/*
 *	Initialize variables for I/O.
 */
out_init()
{
	out_string = NULL;
	last_out_ch = '\0';
	parsing_literal = FALSE;
}

/*
 *	Output string constant of form 'WXYZ' in form:
 *		'W' << 24 | 'X' << 16 | 'Y' << 8 | Z
 *	where len specifies the number of bytes in the string to output.
 */
out_str_const(str_ptr, len)
char	*str_ptr;
int	len;
{
	while (len-- && *str_ptr) {
		out_char('\'');
		if (*str_ptr == '\'')
			out_char('\\');
		out_char(*str_ptr++);
		out_char('\'');

		if (len) {
			out_str(" << ");
			out_str(str_shifts[len]);
			if (*str_ptr)
				out_str(" | ");
		}
	}
}

/*
 *	Convert NUMERIC constant to octal constant
 */
cvt_octal(token, octal_string)
TOKEN	*token;
char	octal_string[];
{
	int	octal;
	char	ch, *ptr;

	octal = 0;
	octal_string[0] = '\\';
	octal_string[4] = '\0';

	ch = *(token->token_start + token->token_length - 1);

		/* Determine base of numeric */
	if (ch == 'H') {
			/* Hex */
		for (ptr = token->token_name + 2; *ptr; ptr++) {
			octal *= 16;
			if ((*ptr >= '0') && (*ptr <= '9'))
				octal += *ptr - '0';
			else
			if ((*ptr >= 'A') && (*ptr <= 'Z'))
				octal += *ptr - 'A' + 10;
			else
			if ((*ptr >= 'a') && (*ptr <= 'z'))
				octal += *ptr - 'a' + 10;
			else {
				parse_error("Illegal hex character");
				return;
			}
		}
	} else

	if ((ch == 'O') || (ch == 'Q')) {
			/* Octal constant */
		for (ptr = token->token_name + 1; *ptr; ptr++) {
			octal *= 8;
			if ((*ptr >= '0') && (*ptr <= '7'))
				octal += *ptr - '0';
			else {
				parse_error("Illegal decimal character");
				return;
			}
		}
	} else {

			/* Decimal constant */
		for (ptr = token->token_name + 1; *ptr; ptr++) {
			octal *= 10;
			if ((*ptr >= '0') && (*ptr <= '9'))
				octal += *ptr - '0';
			else {
				parse_error("Illegal decimal character");
				return;
			}
		}
	}


		/* Generate octal constant */
	octal_string[1] = ((octal >> 6) & 3) + '0';
	octal_string[2] = ((octal >> 3) & 7) + '0';
	octal_string[3] = (octal & 7) + '0';
}
