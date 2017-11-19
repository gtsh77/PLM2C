#include "misc.h"
#include "defs.h"
#include "cvt.h"
#include "struct.h"

/*
 *	Pointer to the current context
 */
CONTEXT		*context_head;
/*
 *	Pointer to all popped contexts
 */
CONTEXT		*old_context;

/*
 *	Search DECL_MEMBER list for symbol and if found, return TRUE
 *	and pointer to DECL_ID for that symbol.
 */
find_member_symbol(symbol, decl_ptr, decl_id)
TOKEN		*symbol;
DECL_MEMBER	*decl_ptr;
DECL_ID		**decl_id;
{
	DECL_ID	*var_ptr;

	for (var_ptr = decl_ptr->name_list; var_ptr;
			var_ptr = var_ptr->next_var) {
		if (!strcmp(var_ptr->name->token_name, symbol->token_name)) {
			*decl_id = var_ptr;
			return TRUE;
		}
	}
	*decl_id = NULL;
	return FALSE;
}

/*
 *	Search DECL_MEMBER list for symbol.
 *	If found, return pointer to DECL_MEMBER containing that symbol
 *	in decl_found, and return TRUE.
 *	If not found, return null pointer in decl_found, and return FALSE.
 */
find_list_symbol(symbol, decl_ptr, decl_found, decl_id)
TOKEN		*symbol;
DECL_MEMBER	*decl_ptr, **decl_found;
DECL_ID		**decl_id;
{
	for (*decl_found = decl_ptr; *decl_found;
			*decl_found = (*decl_found)->next_member) {
		if (find_member_symbol(symbol, *decl_found, decl_id))
			return TRUE;
	}
	return FALSE;
}

/*
 *	Search context for symbol.
 *	If found, return pointer to DECL_MEMBER containing that symbol
 *	in decl_found, return DECL_ID for that symbol in decl_id, and
 *	return TRUE.
 *	If not found, return null pointers in decl_found and decl_id,
 *	and return FALSE.
 */
find_symbol(symbol, decl_found, decl_id)
TOKEN		*symbol;
DECL_MEMBER	**decl_found;
DECL_ID		**decl_id;
{
	CONTEXT	*context_ptr;

	for (context_ptr = context_head; context_ptr;
			context_ptr = context_ptr->next_context) {
		if (find_list_symbol(symbol, context_ptr->decl_head,
				decl_found, decl_id))
			return TRUE;
	}
	return FALSE;
}

/*
 *	Add a declaration to current context
 */
add_to_context(decl)
DECL_MEMBER	*decl;
{
	DECL_MEMBER	*decl_ptr;

		/* Find end of declaration list */
	for (decl_ptr = decl; decl_ptr->next_member; )
		decl_ptr = decl_ptr->next_member;

		/* Add current declarations to tail of new list */
	decl_ptr->next_member = context_head->decl_head;
	context_head->decl_head = decl;
}

/*
 *	Add a DECL list to context and NULL the list pointer
 */
add_decl_to_context(decl)
DECL	*decl;
{
	DECL	*decl_ptr;

		/* Find end of declaration list */
	for (decl_ptr = decl; decl_ptr; decl_ptr = decl_ptr->next_decl) {
		if (decl_ptr->decl_list)
			add_to_context(decl_ptr->decl_list);
		decl_ptr->decl_list = NULL;
	}
}

/*
 *	Push a new context of specified type and name
 */
new_context(type, name)
int	type;
TOKEN	*name;
{
	CONTEXT	*new_context;

	get_context_ptr(&new_context);
	new_context->context_type = type;
	if (name) {
		get_token_ptr(&new_context->context_name);
		token_copy(name, new_context->context_name);
	} else
		new_context->context_name = NULL;
	new_context->next_context = context_head;
	context_head = new_context;
}

/*
 *	Pop current context and place on old context
 */
pop_context()
{
	CONTEXT	*popped_context;

	popped_context = context_head;
	context_head = popped_context->next_context;
	popped_context->next_context = old_context;
	old_context = popped_context;
}

/*
 *	Initializes context pointers
 */
init_context()
{
	context_head = NULL;
	old_context = NULL;
}

