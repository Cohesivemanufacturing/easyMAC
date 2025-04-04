#pragma once

/**************************************************************
Machine state vector
***************************************************************/

// motion mode
# define RAPID					0
# define LINEAR					1
# define CW						2 
# define CCW					3
# define DWELL					4

// plane
# define XY						0
# define ZX						1
# define YZ						2 

// distance mode
# define ABS					0
# define REL					1

// feed mode
# define INVERSE_TIME			0
# define UNITS_PER_MIN			1
# define UNITS_PER_REV			2

// length units
# define IN						0
# define MM						1

// tool radius compensation 
# define RCOMP_DISABLE			0
# define TOOL_LEFT				2
# define TOOL_RIGHT				3

// tool length compensation 
# define LCOMP_DISABLE			0
# define LCOMP_ENABLE			1
        
// canned cycle
# define ORIGIN					0
# define SPECIFIED				1

// path mode
# define EXACT					0	 
# define STOP					1
# define BLENDING				2

// coolant
# define COOLANT_DISABLE		0
# define COOLANT_ENABLE			1
