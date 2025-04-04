#include "atomic_actions.h"

/**************************************************************
Atomic actions definitions
***************************************************************/

Atomic_action *add_action(Atomic_action *node)
{
	// create a new node p
	Atomic_action *p;
	p = new Atomic_action;
	*p = Atomic_action();

	p->next = NULL;

	// inform "node" that its next node is "p" 
	node->next = p;

	return p;

}

Atomic_action *delete_action(Atomic_action *node)
{
	// pointers to the next nodes of "node"
	Atomic_action *next = node->next;

	// delete node and returns a pointer to the previous node
	delete[] node;

	return next;
}
