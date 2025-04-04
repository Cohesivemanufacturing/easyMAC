#pragma once

#include <iostream>
#include "atomic_actions_defs.h"
#include "machine_vector.h"

/* atomic actions 
 * the atomic actions are a stored in FIFO circular queue with methods:
 * add_atomic_action: adds an atomic action "after"
 * delete_atomic_actiion
 * and reference pointers: 
 * action_in and action_out
 */

typedef struct Atomic_action
{
	int    action;
	int    variable;
	int    lineno;
	union
	{	
		// machine vector actions 
		int    sequence;
		axis   position;
		offset origin_offset;
		float  feed;
		float  speed;
		int    tool;
		offset tool_offset;
		float  tool_radius;
		int    coolant;
		int    motion_mode;
		int    plane;
		int    distance_mode;
		int    feed_mode;
		int    units;
		int    radius_comp;
		int    length_comp;
		int    coordinate_system;
		int    retraction_mode;  
		int    path_mode;

		// other actions
		char*  filename;           // deprecated
		const char*  error_message;
		double seconds;
		int    mode;
		float  value;
	};
	struct Atomic_action *next;
} Atomic_action ;


/**************************************************************
Atomic actions prototypes
***************************************************************/
Atomic_action *add_action(Atomic_action *node);
Atomic_action *delete_action(Atomic_action *node);