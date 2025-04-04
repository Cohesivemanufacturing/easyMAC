#pragma once

/**************************************************************
Atomic Actions
***************************************************************/
# define SET_HISTORY_LOG        1
# define SET_ACTIONS_LOG        2
# define LOAD_PROGRAM_FILE      3
# define LOAD_CONFIG_FILE       5
# define SET_PROGRAM            10
# define SET_SEQ_NUMBER         20
# define SET_FEED_MODE          25
# define SET_FEED_RATE          30
# define SET_SPINDLE_SPEED      31
# define SET_PLANE              35
# define SET_LENGTH_UNITS       40
# define SET_RADIUS_COMP        45
# define SET_LENGTH_COMP        50 
# define SET_RETRACTION_MODE    51
# define SET_COORD_SYSTEM       55 
# define SET_PATH_MODE          56
# define SET_DISTANCE_MODE      60
# define SET_ORIGIN_OFFSET      65
# define SET_MOTION_MODE        70
# define STRAIGHT_FEED          75
# define ARC_FEED               80
# define SET_H_VARIABLE			85
# define SET_BLU_IN             90
# define SET_BLU_MM             95
# define SET_ARC_TOL            100
# define SET_INTERPO_STAT       105
# define SET_INTERPO_PORT       110
# define SET_INTERPO_BAUD       115
# define SET_INTERPO_MAXFRQ     116
# define SET_INTERPO_LOGGING	120
# define SET_INTERPO_BURST	    121
# define SET_VIRTMAC_STAT       125
# define SET_CLOUDNC_STAT       126
# define PRINT_ACTION		    130
# define INTERPRETER_ERROR      135
# define EXEPARSER_ERROR        140
# define SET_EXECUTIVE_STAT     145

// set history log
#define HISTORY_LOG_DISABLE     0
#define HISTORY_LOG_ENABLE      1

// set actions log
#define ACTION_LOG_DISABLE      0
#define ACTION_LOG_ENABLE       1

// set program
#define PROGRAM_PAUSE		    1
#define PROGRAM_STOP		    2
#define PROGRAM_CONTINUE	    3

// set interpolator status
# define INTERPOLATOR_DISABLE   0  
# define INTERPOLATOR_ENABLE    1

// set interpolator logging
# define STOP_LOGGING			0
# define START_LOGGING			1

// set interpolator burst
# define STOP_BURST				0
# define START_BURST		    1
# define RECEIVE_BURST          2

// set virtual mac
# define VIRTMAC_DISABLE		0  
# define VIRTMAC_ENABLE			1

// set cloudNC
# define CLOUDNC_DISABLE		0  
# define CLOUDNC_ENABLE			1

// print action
# define PRINT_ATOMIC_ACTIONS   0
# define PRINT_ACTIONS_HISTORY  1
# define PRINT_MACHINE_VECTOR   2
# define PRINT_BLOCK_VECTOR     3 
# define PRINT_MODAL_VECTOR     4 
# define PRINT_H_VARIABLE       5 

// set executive status
#define EXECUTIVE_DISABLE       0
#define EXECUTIVE_ENABLE        1