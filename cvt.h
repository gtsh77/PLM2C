/*
 *	Parse LITERALLY declared strings
 */
#define PARSE_LITERALS

/*
 *	Ignore Invalid control errors
 */
#define	IGNORE_CONTROL_ERRORS

/*
 *	Convert lower case to upper and upper to lower in identifiers
 */
#define	CONVERT_CASE

/*
 *	If CONVERT_TYPES defined, use the following type conversions.
 */
#define CONVERT_TYPES

/*
 *	Type conversions
 */
#define TYPE_BYTE	"BYTE"
#define TYPE_WORD	"WORD"
#define TYPE_DWORD	"DWORD"
#define TYPE_INTEGER	"short"
#define TYPE_REAL	"float"

/*
 *	For initialized DATA, use this prefix.
 *	Probably should be "const" or "static".
 */
#define TYPE_DATA	"const"

/*
 *	Default POINTER type.
 */
#define TYPE_POINTER	"void"

/*
 *	Sizes of data types
 */
#define SIZE_BYTE	1
#define SIZE_WORD	2
#define SIZE_DWORD	4

/*
 *	Conversion operators
 */
#define AND_OP	"&&"
#define OR_OP	"||"
#define NOT_OP	"!"
/*
#define AND_OP	"&"
#define OR_OP	"|"
#define NOT_OP	"~"
*/
/*
#define AND_OP	"AND"
#define OR_OP	"OR"
#define NOT_OP	"NOT"
*/

/*
 *	Function call equivalent to OUTPUT(port) = expr
 *	Becomes: FUNC_OUTPUT(port, expr)
 */
#define FUNC_OUTPUT	"outportb"

/*
 *	Function call equivalent to OUTWORD(port) = expr
 *	Becomes: FUNC_OUTWORD(port, expr)
 */
#define FUNC_OUTWORD	"outport"

/*
 *	Function call equivalent to OUTHWORD(port) = expr
 *	Becomes: FUNC_OUTHWORD(port, expr)
 */
#define FUNC_OUTHWORD	"outporth"

