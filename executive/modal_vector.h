#pragma once

#include "modal_vector_defs.h"
#include "modal_vector_types.h"   // prep words enumerations

#include "block_vector.h"
#include "queue.h"

# ifndef EMPTYCODE
# define EMPTYCODE     -1 
# endif

typedef struct
{
	queue modal0;               // non-modals

	int   modal1  = G00;        // rapid motion
	int   modal2  = G17;	    // XY plane
	int   modal3  = G90;        // absolute motion
	int   modal4  = EMPTYCODE;  // 
	int   modal5  = G94;        // feed in units per minute mode
	int   modal6  = G21;        // mm 
	int   modal7  = G40;        // cancel cutter radius compensation
	int   modal8  = G49;        // cancel tool length offset
	int   modal9  = EMPTYCODE;  //
	int   modal10 = G98;        // retract to the position that axis was canned cycles
	int   modal11 = EMPTYCODE;  // 
	int   modal12 = G54;        // machine coordinate system 1
	int   modal13 = G61;        // exact path mode

} Modal_vector;

/**************************************************************
modal vector function prototypes
***************************************************************/

/* setters */
void update_modal_vector(Modal_vector *modal, Block_vector *block);
int store_modal(Modal_vector *modal, int val);

/* getters */
// char* serialize_modal_vector(void);
int find_group(int val);
int ismodal(Modal_vector*modal, int val);



