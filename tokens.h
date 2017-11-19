/**************************
 *	Token classes
 *************************/
#define END_OF_FILE	0
#define RESERVED	1
#define IDENTIFIER	2
#define NUMERIC		3
#define OPERATOR	4
#define STRING		5
#define LABEL		6
#define END_OF_LINE	7
#define COMMA		8
#define PERIOD		9
#define LEFT_PAREN	10
#define RIGHT_PAREN	11
#define SUBSCRIPT	12
#define MODULE		13
#define ERROR		19


/**************************
 *	Token types
 *************************/
/*
 *	Operators
 */
#define PLUS		20	/* +	*/
#define MINUS		21	/* -	*/
#define TIMES		22	/* *	*/
#define DIVIDE		23	/* /	*/
#define NOT_EQUAL	24	/* <>	*/
#define LESS_EQUAL	25	/* <=	*/
#define GREATER_EQUAL	26	/* >=	*/
#define LESS		27	/* <	*/
#define GREATER		28	/* >	*/
#define EQUAL		29	/* =	*/
#define EQUATE		30	/* :=	*/
#define COLON		31	/* :	*/
#define AT_OP		32	/* @	*/

/*
 *	Reserved word values
 */
	/* Statements */
#define DECLARE		40
#define DO		41
#define END		42
#define IF		43
#define THEN		44
#define ELSE		45
#define GOTO		46
#define GO		47
#define CALL		48
#define RETURN		49
#define DISABLE		50
#define ENABLE		51
#define OUTPUT		52
#define OUTWORD		53
#define OUTHWORD	54

	/* Operators */
#define AND		60
#define OR		61
#define XOR		62
#define NOT		63
#define MOD		64

	/* DO options */
#define CASE		70
#define WHILE		71
#define TO		72
#define BY		73

	/* DECLARE types */
#define BYTE		80
#define WORD		81
#define DWORD		82
#define INTEGER		83
#define REAL		84
#define ADDRESS		85
#define SELECTOR	86
#define POINTER		87
#define STRUCTURE	88

	/* DECLARE options */
#define BASED		90
#define LITERALLY	91
#define DATA		92
#define EXTERNAL	93
#define INITIAL		94
#define PUBLIC		95
#define AT		96

	/* Misc reserved words */
#define PROCEDURE	101
#define REENTRANT	102
#define INTERRUPT	103

	/* Control Directives */
#define C_CODE		200
#define C_NOCODE	201
#define C_COND		202
#define C_NOCOND	203
#define C_DEBUG		204
#define C_NODEBUG	205
#define C_EJECT		206
#define C_IF		207
#define C_ELSEIF	208
#define C_ELSE		209
#define C_ENDIF		210
#define C_INCLUDE	211
#define C_INTERFACE	212
#define C_LEFTMARGIN	213
#define C_LIST		214
#define C_NOLIST	215
#define C_OBJECT	216
#define C_NOOBJECT	217
#define C_OPTIMIZE	218
#define C_OVERFLOW	219
#define C_NOOVERFLOW	220
#define C_PAGELENGTH	221
#define C_PAGEWIDTH	222
#define C_PAGING	223
#define C_NOPAGING	224
#define C_PRINT		225
#define C_NOPRINT	226
#define C_RAM		227
#define C_ROM		228
#define C_SAVE		229
#define C_RESTORE	230
#define C_SET		231
#define C_RESET		232
#define C_SMALL		233
#define C_COMPACT	234
#define C_MEDIUM	235
#define C_LARGE		236
#define C_SUBTITLE	237
#define C_SYMBOLS	238
#define C_NOSYMBOLS	239
#define C_TITLE		240
#define C_TYPE		241
#define C_NOTYPE	242
#define C_XREF		243
#define C_NOXREF	244
#define C_INTVECTOR	245
#define C_NOINTVECTOR	246
#define C_MOD86		247
#define C_MOD186	248
#define C_WORD16	249
#define C_WORD32	250

