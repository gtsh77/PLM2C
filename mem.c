#ifdef IBMPC
#include <alloc.h>
#endif
#include "misc.h"
#include "defs.h"
#include "cvt.h"
#include "struct.h"

/*
 *	Memory allocation and deallocation routines.
 */

/*
 *	Allocate memory
 */
char *get_mem(size)
unsigned int	size;
{
	char	*malloc_ptr;
	void	*malloc();

	if ((malloc_ptr = (char *)malloc(size)) == NULL) {
		parse_error("Out of memory");
		exit(1);
	}
	return malloc_ptr;
}

/*
 *	Generate a new context.
 */
get_context_ptr(context)
CONTEXT	**context;
{
	*context = (CONTEXT *) get_mem(sizeof(CONTEXT));
	(*context)->decl_head = NULL;
	(*context)->next_context = NULL;
}

/*
 *	Malloc memory for a TOKEN.
 */
get_token_ptr(token)
TOKEN	**token;
{
	*token = (TOKEN *) get_mem(sizeof(TOKEN));
}

/*
 *	Malloc memory for a DECL_ID.
 */
get_var_ptr(var)
DECL_ID	**var;
{
	*var = (DECL_ID *) get_mem(sizeof(DECL_ID));
	(*var)->name = NULL;
	(*var)->based_name = NULL;
	(*var)->next_var = NULL;
	(*var)->is_ext_at = FALSE;
}

/*
 *	Free a linked list of variables.
 */
free_var_list(list_ptr)
DECL_ID	*list_ptr;
{
	DECL_ID	*next_ptr;

	while (list_ptr) {
		if (list_ptr->name)
			free( (char *) list_ptr->name);
		if (list_ptr->based_name)
			free( (char *) list_ptr->based_name);
		next_ptr = list_ptr->next_var;
		free((char *) list_ptr);
		list_ptr = next_ptr;
	}
}

/*
 *	Malloc space for a DECL_MEMBER structure and return pointer.
 */
get_element_ptr(element)
DECL_MEMBER	**element;
{
	DECL_MEMBER	*el_ptr;

		/* Malloc space for element */
	el_ptr = (DECL_MEMBER *) get_mem(sizeof(DECL_MEMBER));

		/* Init pointers */
	el_ptr->name_list = NULL;
	el_ptr->literal = NULL;
#ifdef PARSE_LITERALS
	el_ptr->literal_token = NULL;
#endif
	el_ptr->array_bound = NULL;
	el_ptr->type = NULL;
	el_ptr->struct_list = NULL;
	el_ptr->at_ptr = NULL;
	el_ptr->init_ptr = NULL;
	el_ptr->next_member = NULL;

	el_ptr->attributes = NONE;
	el_ptr->initialization = NONE;

	*element = el_ptr;
}

/*
 *	Free a DECL_MEMBER list.
 */
free_decl_list(element)
DECL_MEMBER	*element;
{
	DECL_MEMBER	*el_ptr;

	while (element) {
		if (element->name_list)
			free_var_list(element->name_list);
		if (element->literal)
			free((char *) element->literal);
		if (element->array_bound)
			free((char *) element->array_bound);
		if (element->type)
			free((char *) element->type);
		if (element->struct_list)
			free_decl_list(element->struct_list);
		if (element->at_ptr)
			free(element->at_ptr);

		el_ptr = element->next_member;
		free((char *) element);
		element = el_ptr;
	}
}

/*
 *	Malloc space for a procedure parameter
 */
get_param_ptr(param)
PARAM_LIST	**param;
{
	*param = (PARAM_LIST *) get_mem(sizeof(PARAM_LIST));
	(*param)->next_param = NULL;
}

/*
 *	Free parameter list
 */
free_param_list(param_list)
PARAM_LIST	*param_list;
{
	PARAM_LIST	*param_ptr;

	while (param_list) {
		param_ptr = param_list->next_param;
		free((char *) param_list);
		param_list = param_ptr;
	}
}

/*
 *	Malloc space for a DECLARE statement
 */
get_decl_ptr(decl)
DECL	**decl;
{
	*decl = (DECL *) get_mem(sizeof(DECL));
	(*decl)->decl_list = NULL;
	(*decl)->next_decl = NULL;
}

/*
 *	Free DECL list
 */
free_decl(decl_list)
DECL	*decl_list;
{
	DECL	*decl_ptr;

	while (decl_list) {
		decl_ptr = decl_list->next_decl;
#ifdef FREE_DECL_TOKEN
		if (decl_list->decl_token)
			free((char *) decl_list->decl_token);
#endif
		if (decl_list->decl_list)
			free_decl_list(decl_list->decl_list);
		free((char *) decl_list);
		decl_list = decl_ptr;
	}
}


