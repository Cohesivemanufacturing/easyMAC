#pragma once

#include <iostream>

#include "queue.h"

# ifndef EMPTYCODE
# define EMPTYCODE     -1 
# endif

/* block vector */

typedef struct
{
	queue G;				  // queue for the preparatory words 
	queue M;				  // queue for the miscellaneous word
	int   N = EMPTYCODE;	  // sequece word
	float X = NAN;			  // dimiension words 
	float Y = NAN;
	float Z = NAN;
	float I = NAN;
	float J = NAN;
	float K = NAN;
	float R = NAN;             // tool radius compensation               
	float Q = NAN;             // delta increment along the Z-direction for drilling cycle
	int   L = EMPTYCODE;       // sets CS tables, tool offsets or number of repetitions in a drilling cycle
	int   P = EMPTYCODE;       // number of seconds to dwell in a drill cycle
	float F = NAN;;            // feed word
	float S = NAN;;			   // speed word
	int   T = EMPTYCODE;       // tool word  

} Block_vector;


