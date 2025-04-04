#pragma once

#include "machine_vector_defs.h"
#include "machine_vector_types.h"

typedef struct
{
	int    sequence;				// target sequence number in the program
	axis   target_position;		    // target position of the machine in the native coordinate system (G53)
	offset origin_offset;			// origin_offsets for coordinates systems G54-G59 as per G92 (same as H760, H761, H762)
	float  feed;					// feed in MM/min or  IN/min
	float  speed; 					// spindle speed in RPM
	int    tool;					// tool number
	offset tool_offset;				// current tool X, Y and Z offsets (same as H800- H860) 
	float  tool_radius;				// current tool raduis (same as H800- H860) 
	int    coolant;					// DISABLE or ENABLE
	int    motion_mode;				// linear, CW, CCW, RAPID, DWELL
	int    plane;					// XY, YZ or XZ
	int    distance_mode;			// ABS or REL distance mode
	int    feed_mode;				// inverse time, units per minute or units per revolution mode
	int    units;					// MM or IN
	int    radius_comp;				// DISABLE, LEFT_TOOL or RIGHT_TOOL
	int    length_comp;				// DISABLE, ENABLE 
	int    coordinate_system;		// G54 - G59
	int    retraction_mode;			// ORIGIN, SPECIFIED 
	int    path_mode;				// EXACT, STOP, BLENDING
} Machine_vector;
