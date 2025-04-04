#include "modal_vector.h"	

// map between preparatoy words and modal groups

static int prep_words[] = { G93, G94, G95, // set feed mode 
G04,            			        // set dwell
G17, G18, G19,  				    // set active plane
G20, G21,       			        // set length units
G40, G41, G42,  			        // cutter radius compensation
G43, G49, 					        // cutter length compensation
G54, G55, G56, G57, G58, G59,       // coordinate system selection
G61, G64, 							// set path control mode
G90, G91, 			    	        // set distance mode
G98, G99,                           // set retract mode
G28, G30, G10, G45, G46, G47,       // home, set offsets
G48, G92, G92_1, G92_2, G92_3,
G00, G01, G02, G03, G38_2, G80,     // motion
G81, G82, G83, G84, G85, G86,
G87, G88, G89, G53 };

static int modal_groups[] = { 5, 5, 5,
0,
2, 2, 2,
6, 6,
7, 7, 7,
8, 8,
12, 12, 12, 12, 12, 12,
13, 13,
3, 3,
10, 10,
0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0,
0, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1,
1, 1, 1, 0 };

/**************************************************************
modal vector function definitions
***************************************************************/

/* setters */
void update_modal_vector(Modal_vector *modal, Block_vector *block)
{
	// clear non-modals
	init_queue(&modal->modal0);

	int code = EMPTYCODE;

	//  G codes
	while (block->G.queue_out != block->G.queue_in)
	{
		delete_queue(&block->G, &code); // clear the block_vector G commands
		store_modal(modal, code);
	}

	//  M codes
	while (block->M.queue_out != block->M.queue_in)
	{
		delete_queue(&block->M, &code); // clear the block_vector M commands
		store_modal(modal, code);
	}
}

int store_modal(Modal_vector *modal, int val)
{
	int group = find_group(val);
	if (group != EMPTYCODE) {
		switch (group) {
		case 00:
			add_queue(&modal->modal0, val);
			return true;
			break;
		case 1:
			modal->modal1 = val;
			return true;
			break;
		case 2:
			modal->modal2 = val;
			return true;
			break;
		case 3:
			modal->modal3 = val;
			return true;
			break;
		case 4:
			modal->modal4 = val;
			return true;
			break;
		case 5:
			modal->modal5 = val;
			return true;
			break;
		case 6:
			modal->modal6 = val;
			return true;
			break;
		case 7:
			modal->modal7 = val;
			return true;
			break;
		case 8:
			modal->modal8 = val;
			return true;
			break;
		case 9:
			modal->modal9 = val;
			return true;
			break;
		case 10:
			modal->modal10 = val;
			return true;
			break;
		case 11:
			modal->modal11 = val;
			return true;
			break;
		case 12:
			modal->modal12 = val;
			return true;
			break;
		case 13:
			modal->modal13 = val;
			return true;
			break;
		default:
			break;
		}
	}
	return false;
}

/* getters */
int find_group(int val)
{
	/*checks preparatory word */
	for (int i = 0; i != NUM_GCODES; i++)
	{
		/*checks item members*/
		if (val == prep_words[i])
		{
			return modal_groups[i];
		}
	}
	return EMPTYCODE;
}

int ismodal(Modal_vector *modal, int val)
{
	int head = modal->modal0.queue_in;
	int tail = modal->modal0.queue_out;
	int size = modal->modal0.size;

	int group = find_group(val);

	switch (group) {
	case 0:
		while (tail != head)
		{
			int code = modal->modal0.queue[tail];
			if (val == code) {
				return true;
			}
			tail = (tail + 1) % size;
		}
		return false;
		break;
	case 1:
		return (modal->modal1 == val) ? true : false;
		break;
	case 2:
		return (modal->modal2 == val) ? true : false;
		break;
	case 3:
		return (modal->modal3 == val) ? true : false;
		break;
	case 4:
		return (modal->modal4 == val) ? true : false;
		break;
	case 5:
		return (modal->modal5 == val) ? true : false;
		break;
	case 6:
		return (modal->modal6 == val) ? true : false;
		break;
	case 7:
		return (modal->modal7 == val) ? true : false;
		break;
	case 8:
		return (modal->modal8 == val) ? true : false;
		break;
	case 9:
		return (modal->modal9 == val) ? true : false;
		break;
	case 10:
		return (modal->modal10 == val) ? true : false;
		break;
	case 11:
		return (modal->modal11 == val) ? true : false;
		break;
	case 12:
		return (modal->modal12 == val) ? true : false;
		break;
	case 13:
		return (modal->modal13 == val) ? true : false;
		break;
	default:
		return false;
	}
}
