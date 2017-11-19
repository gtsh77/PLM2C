/*
 *	PL/M Cast function equivalents
 */
CVT_ID cast_functions[] = {
	"float",	TYPE_REAL,
	"real",		TYPE_REAL,
	"fix",		TYPE_INTEGER,
	"int",		TYPE_INTEGER,
	"signed",	TYPE_INTEGER,
	"integer",	TYPE_INTEGER,
	"unsign",	TYPE_WORD,
	"word",		TYPE_WORD,
	"byte",		TYPE_BYTE,
	"dword",	TYPE_DWORD,
	"pointer",	TYPE_POINTER,
	"",		""
};

/*
 *	PL/M function equivalents
 */
CVT_ID cvt_functions[] = {
	"size",			"sizeof",
	"abs",			"fabs",
	"iabs",			"abs",
	"input",		"inportb",
	"inword",		"inport",
	"setinterrupt",		"signal",
	"initrealmathunit",	"_fpreset",
	"",		""
};

/*
 *	PL/M identifier equivalents
 */
CVT_ID cvt_identifiers[] = {
	"getrealerror",		"_status87()",
	"",			""
};

