/*
 *	Format of a token returned by get_token().
 */
typedef struct TOKEN {
		/* Class of token (see below) */
	int	token_class;
		/* Type of token (see below) */
	int	token_type;
		/* Converted token name (when applicable) */
	char	token_name[MAX_TOKEN_LENGTH];
		/* Pointer to start of token in text_buffer */
	char	*token_start;
		/* Number of characters token_start points to */
	int	token_length;
		/* Pointer to start of white space in text_buffer */
	char	*white_space_start;
		/* Pointer to char after end of white space in text_buffer */
	char	*white_space_end;
#ifdef LINKED_TOKENS
		/* Pointer for use in linked list */
	struct	TOKEN	*next_token;
#endif
} TOKEN;

/*
 *	Format of a procedure parameter list
 */
typedef struct PARAM_LIST {
		/* Parameter name */
	TOKEN	param;
		/* Pointer for use in linked list */
	struct PARAM_LIST *next_param;
} PARAM_LIST;

/*
 *	Format of a variable in a DECLARE statement.
 */
typedef struct DECL_ID {
		/* Variable name */
	TOKEN	*name;
		/* BASED identifier token */
	TOKEN	*based_name;
		/* If declared AT in another module */
	BOOLEAN	is_ext_at;
		/* Pointer for use in linked list */
	struct DECL_ID *next_var;
} DECL_ID;

/*
 *	Format of an element in a DECLARE statement.
 */
typedef struct DECL_MEMBER {
		/* Linked list of identifiers of designated type */
	DECL_ID	*name_list;
		/* LITERALLY string */
	char	*literal;
#ifdef PARSE_LITERALS
		/* Parsed LITERAL token */
	TOKEN	*literal_token;
#endif
		/* Array bound token */
	TOKEN	*array_bound;
		/* Type of variable (INTEGER, WORD, LABEL, LITERALLY, etc.) */
	TOKEN	*type;
		/* Attributes (NONE, EXTERNAL or PUBLIC) */
	int	attributes;
		/* Initialization attribute (NONE, INITIAL or DATA) */
		/* If PROCEDURE, DATA if has parameters */
	int	initialization;
		/* Pointer to linked list of structure elements */
	struct DECL_MEMBER *struct_list;
		/* Pointer to parsed AT expression */
	char	*at_ptr;
		/* Pointer in text_buffer to start of INITIAL/DATA values */
	char	*init_ptr;
		/* Pointer for use in linked list */
	struct DECL_MEMBER *next_member;
} DECL_MEMBER;

/*
 *	Format of a DECLARE statement.
 */
typedef struct DECL {
		/* DECLARE token */
	TOKEN		*decl_token;
		/* Linked list of DECL_MEMBERs */
	DECL_MEMBER	*decl_list;
		/* Pointer for use in linked list */
	struct	DECL	*next_decl;
} DECL;

/*
 *	Format of a context element
 */
typedef struct CONTEXT {
		/* Type of context (MODULE, PROCEDURE or DO) */
	int	context_type;
		/* Name of module or procedure */
	TOKEN	*context_name;
		/* Pointer to linked list of declaration members */
	DECL_MEMBER	*decl_head;
		/* Pointer for use in linked list */
	struct CONTEXT	*next_context;
} CONTEXT;


/*
 *	Format of a PL/M identifier equivalent
 */
typedef struct {
	char	*id_name, *new_id;
} CVT_ID;


/*
 *	Format of a PL/M reserved word
 */
typedef struct {
	char	*name;
	int	token;
} RESERVED_WORD;

/*
 *	Format of a PL/M reserved operator
 */
typedef struct {
	char	*operator;
	char	*cvt_operator;
	int	name;
} RESERVED_OPERATOR;

