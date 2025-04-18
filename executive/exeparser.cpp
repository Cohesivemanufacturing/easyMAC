/***************************************************
* easyMac Intepreter 1.0						  *
* Variables and function definitions              *
*                                                 *
* Jorge Correa & Placid Ferreira                  *
* 2016                                            *
*                                                 *
***************************************************/

#include "exeparser.h"

/**************************************************************
Lex and Yacc function definitions
***************************************************************/

void exeerror(char *s)
{
	// pointer to the head and tail actions
	Atomic_action **head = Exeparser::getActionIn();
	Atomic_action **tail = Exeparser::getActionOut();

	//format message
	static std::string message(s);

	// create  atomic action
	(*head)->action = INTERPRETER_ERROR;
	(*head)->error_message = message.c_str(); 
	(*head)->lineno = exelineno;
	(*head)->variable = ERR_EXEERROR;
	(*head) = add_action((*tail));
}

/**************************************************************
Static functions outside the class
***************************************************************/
static inline void putfile(char* filename);

static void putfile(char* filename) {
	FILE *file = fopen(filename, "r");
	int c;

	if (file) {
		while ((c = getc(file)) != EOF)
			putchar(c);
		fclose(file);
	}
}

/**************************************************************
Exeparser object
***************************************************************/
// initialize exeparser static members
Exeparser	    *Exeparser::exeparser = NULL;          
H_variables     *Exeparser::h_variables = NULL;
Modal_vector    *Exeparser::modal_vector = NULL;
Machine_vector  *Exeparser::machine_vector = NULL;
Atomic_action  **Exeparser::action_in = NULL;
Atomic_action  **Exeparser::action_out = NULL;

// (action_in) = points to pointer to the current action
// *(action_in) = points to current action
// **(action_in) = current action


// Executive-parser public services

Exeparser* Exeparser::initExeparser(H_variables *variables, Modal_vector *modal, Machine_vector *machine, 
	Atomic_action **head, Atomic_action **tail)
{
	exeparser = new Exeparser();
	
	// bind executive structures 
	h_variables    = variables;
	modal_vector   = modal;
	machine_vector = machine;
	action_in      = head;
	action_out     = tail;

	return exeparser;
}

Exeparser::~Exeparser(void)
{
	// free executive structures 
	h_variables    = NULL;
	modal_vector   = NULL;
	machine_vector = NULL;
	action_in      = NULL;
	action_out     = NULL;
}

void Exeparser::loadProgramFileAction(char* filename)
{
	static char dst[LONG_STRING_SIZE];
	strcpy_s(dst, LONG_STRING_SIZE, filename);

	(*action_in)->action = LOAD_PROGRAM_FILE;
	(*action_in)->filename = dst;
	(*action_in) = add_action((*action_in));
}

void Exeparser::loadConfigFileAction(char* filename)
{
	static char dst[LONG_STRING_SIZE];
	strcpy_s(dst, LONG_STRING_SIZE, filename);

	(*action_in)->action = LOAD_CONFIG_FILE;
	(*action_in)->filename = dst;
	(*action_in) = add_action((*action_in));
}

void Exeparser::setActionsLogAction(char* stat)
{
	if (strncmp(stat, "enable", 5) == 0)
	{
		(*action_in)->action = SET_ACTIONS_LOG;
		(*action_in)->mode = ACTION_LOG_ENABLE;
		(*action_in) = add_action((*action_in));
	}

	else if (strncmp(stat, "disable", 6) == 0)
	{
		(*action_in)->action = SET_ACTIONS_LOG;
		(*action_in)->mode = ACTION_LOG_DISABLE;
		(*action_in) = add_action((*action_in));
	}

	else
	{
		error(ERR_PARAMETR);
	}
}

void Exeparser::setProgramAction(char* mode)
{
	// program pause. waits in the thread until program continues
	if (strncmp(mode, "pause", 4) == 0)
	{
		(*action_in)->action = SET_PROGRAM;
		(*action_in)->mode = PROGRAM_PAUSE;
		(*action_in) = add_action((*action_in));
	}
	// program stop. closes parse program thread
	else if (strncmp(mode, "stop", 3) == 0)
	{
		(*action_in)->action = SET_PROGRAM;
		(*action_in)->mode = PROGRAM_STOP;
		(*action_in) = add_action((*action_in));
	}
	// program continue. continues program thread execution
	else if (strncmp(mode, "continue", 8) == 0)
	{
		(*action_in)->action = SET_PROGRAM;
		(*action_in)->mode = PROGRAM_CONTINUE;
		(*action_in) = add_action((*action_in));
	}
	
	else
	{
		//error(ERR_PARAMETR);
	}
}

void Exeparser::setFeedModeAction(char* mode)
{
	if (strncmp(mode, "inverse_time", 11) == 0)
	{
		// store modal 
		store_modal(modal_vector, G93);

		// generate atomic action
		(*action_in)->action = SET_FEED_MODE;
		(*action_in)->feed_mode = INVERSE_TIME;
		(*action_in) = add_action((*action_in));
	}

	else if (strncmp(mode, "units_per_min", 12) == 0)
	{
		// store modal 
		store_modal(modal_vector, G94);

		// generate atomic action
		(*action_in)->action = SET_FEED_MODE;
		(*action_in)->feed_mode = UNITS_PER_MIN;
		(*action_in) = add_action((*action_in));
	}

	else if (strncmp(mode, "units_per_rev", 12) == 0)
	{
		// store modal 
		store_modal(modal_vector, G95);

		// generate atomic action
		(*action_in)->action = SET_FEED_MODE;
		(*action_in)->feed_mode = UNITS_PER_REV;
		(*action_in) = add_action((*action_in));
	}

	else
	{
		error(ERR_PARAMETR);
	}
}

void Exeparser::setPlaneAction(char* plane)
{
	if (strncmp(plane, "xy", 1) == 0)
	{
		// store modal 
		store_modal(modal_vector, G17);

		// generate atomic action
		(*action_in)->action = SET_PLANE;
		(*action_in)->plane = XY;
		(*action_in) = add_action((*action_in));
	}

	else if (strncmp(plane, "zx", 1) == 0)
	{
		// store modal 
		store_modal(modal_vector, G18);

		// generate atomic action
		(*action_in)->action = SET_PLANE;
		(*action_in)->plane = ZX;
		(*action_in) = add_action((*action_in));
	}

	else if (strncmp(plane, "yz", 1) == 0)
	{
		// store modal 
		store_modal(modal_vector, G19);

		// generate atomic action
		(*action_in)->action = SET_PLANE;
		(*action_in)->plane = YZ;
		(*action_in) = add_action((*action_in));
	}

	else {
		error(ERR_PARAMETR);
	}
}

void Exeparser::setLengthUnitsAction(char* units)
{
	if (strncmp(units, "in", 2) == 0)
	{
		// store modal 
		store_modal(modal_vector, G20);

		// generate atomic action
		(*action_in)->action = SET_LENGTH_UNITS;
		(*action_in)->units = IN;
		(*action_in) = add_action((*action_in));
	}

	else if (strncmp(units, "mm", 2) == 0)
	{
		// store modal 
		store_modal(modal_vector, G21);

		// generate atomic action
		(*action_in)->action = SET_LENGTH_UNITS;
		(*action_in)->units = MM;
		(*action_in) = add_action((*action_in));
	}

	else
	{
		error(ERR_PARAMETR);
	}

}

void Exeparser::setRadiusCompensationAction(char *mode)
{
	if (strncmp(mode, "disable", 6) == 0)
	{
		// store modal 
		store_modal(modal_vector, G40);

		// generate atomic action
		(*action_in)->action = SET_RADIUS_COMP;
		(*action_in)->radius_comp = RCOMP_DISABLE;
		(*action_in) = add_action((*action_in));
	}

	else if (strncmp(mode, "tool_left", 8) == 0)
	{
		// store modal 
		store_modal(modal_vector, G41);

		// generate atomic action
		(*action_in)->action = SET_RADIUS_COMP;
		(*action_in)->radius_comp = TOOL_LEFT;
		(*action_in) = add_action((*action_in));
	}

	else if (strncmp(mode, "tool_right", 9) == 0)
	{
		// store modal 
		store_modal(modal_vector, G42);

		// generate atomic action
		(*action_in)->action = SET_RADIUS_COMP;
		(*action_in)->radius_comp = TOOL_RIGHT;
		(*action_in) = add_action((*action_in));
	}

	else
	{
		error(ERR_PARAMETR);
	}
}

void Exeparser::setLengthCompensationAction(char *mode)
{
	if (strncmp(mode, "enable", 5) == 0)
	{
		// store modal 
		store_modal(modal_vector, G43);

		// generate atomic action
		(*action_in)->action = SET_LENGTH_COMP;
		(*action_in)->length_comp = LCOMP_ENABLE;
		(*action_in) = add_action((*action_in));
	}

	else if (strncmp(mode, "disable", 5) == 0)
	{
		// store modal 
		store_modal(modal_vector, G49);

		// generate atomic action
		(*action_in)->action = SET_LENGTH_COMP;
		(*action_in)->length_comp = LCOMP_DISABLE;
		(*action_in) = add_action((*action_in));
	}

	else
	{
		error(ERR_PARAMETR);
	}
}

void Exeparser::setRetractModeAction(char *mode)
{
	if (strncmp(mode, "origin", 5) == 0)
	{
		// store modal 
		store_modal(modal_vector, G98);

		// generate atomic action
		(*action_in)->action = SET_RETRACTION_MODE;
		(*action_in)->retraction_mode = ORIGIN;
		(*action_in) = add_action((*action_in));
	}

	else if (strncmp(mode, "specified", 9) == 0)
	{
		// store modal 
		store_modal(modal_vector, G99);

		// generate atomic action
		(*action_in)->action = SET_RETRACTION_MODE;
		(*action_in)->retraction_mode = SPECIFIED;
		(*action_in) = add_action((*action_in));
	}

	else {
		error(ERR_PARAMETR);
	}
}

void Exeparser::setCoordinateSystemAction(int code)
{
	if (code >= G54 && code <= G59)
	{
		// store modal 
		store_modal(modal_vector, code);
		
		// generate atomic action
		(*action_in)->action = SET_COORD_SYSTEM;
		(*action_in)->coordinate_system = code;
		(*action_in) = add_action((*action_in));
	}
	else
	{
		error(ERR_PARAMETR);
	}

}

void Exeparser::setPathModeAction(char* mode)
{
	if (strncmp(mode, "exact", 4) == 0) {
		// store modal 
		store_modal(modal_vector, G61);

		// generate atomic action
		(*action_in)->action = SET_PATH_MODE;
		(*action_in)->path_mode = EXACT;
		(*action_in) = add_action((*action_in));
	}

	else if (strncmp(mode, "stop", 3) == 0) {
		// store modal 
		//store_modal(modal_vector, G61.4);

		// generate atomic action
		(*action_in)->action = SET_PATH_MODE;
		(*action_in)->path_mode = STOP;
		(*action_in) = add_action((*action_in));
	}

	else if (strncmp(mode, "blending", 7) == 0) {
		// store modal 
		store_modal(modal_vector, G64);

		// generate atomic action
		(*action_in)->action = SET_PATH_MODE;
		(*action_in)->path_mode = BLENDING;
		(*action_in) = add_action((*action_in));
	}

	else
	{
		error(ERR_PARAMETR);
	}

}

void Exeparser::setDistanceModeAction(char* mode)
{
	if (strncmp(mode, "abs", 2) == 0)
	{
		// store modal 
		store_modal(modal_vector, G90);

		// generate atomic action
		(*action_in)->action = SET_DISTANCE_MODE;
		(*action_in)->distance_mode = ABS;
		(*action_in) = add_action((*action_in));
	}

	else if (strncmp(mode, "rel", 2) == 0)
	{
		// store modal 
		store_modal(modal_vector, G91);

		// generate atomic action
		(*action_in)->action = SET_DISTANCE_MODE;
		(*action_in)->distance_mode = REL;
		(*action_in) = add_action((*action_in));
	}

	else {
		error(ERR_PARAMETR);
	}

}

void Exeparser::setMotionModeAction(char* mode)
{
	if (strncmp(mode, "rapid", 4) == 0)
	{
		// store modal 
		store_modal(modal_vector, G00);

		// generate atomic action
		(*action_in)->action = SET_MOTION_MODE;
		(*action_in)->motion_mode = RAPID;
		(*action_in) = add_action((*action_in));
	}

	else if (strncmp(mode, "linear", 5) == 0)
	{
		// store modal 
		store_modal(modal_vector, G01);

		// generate atomic action
		(*action_in)->action = SET_MOTION_MODE;
		(*action_in)->motion_mode = LINEAR;
		(*action_in) = add_action((*action_in));
	}

	else if (strncmp(mode, "cw", 1) == 0)
	{
		// store modal 
		store_modal(modal_vector, G02);

		// generate atomic action
		(*action_in)->action = SET_MOTION_MODE;
		(*action_in)->motion_mode = CW;
		(*action_in) = add_action((*action_in));
	}

	else if (strncmp(mode, "ccw", 2) == 0)
	{
		// store modal 
		store_modal(modal_vector, G03);

		// generate atomic action
		(*action_in)->action = SET_MOTION_MODE;
		(*action_in)->motion_mode = CCW;
		(*action_in) = add_action((*action_in));
	}

	else
	{
		error(ERR_PARAMETR);
	}

}

void Exeparser::setHvariableAction(int var, float value)
{
	(*action_in)->action = SET_H_VARIABLE;
	(*action_in)->variable = var;
	(*action_in)->value = value;

	(*action_in) = add_action((*action_in));
}

void Exeparser::setBLUinAction(float val)
{
	(*action_in)->action = SET_BLU_IN;
	(*action_in)->value = val;
	(*action_in) = add_action((*action_in));
}

void Exeparser::setBLUmmAction(float val)
{
	(*action_in)->action = SET_BLU_MM;
	(*action_in)->value = val;
	(*action_in) = add_action((*action_in));
}

void Exeparser::setArcTolAction(int val)
{
	(*action_in)->action = SET_ARC_TOL;
	(*action_in)->value = val;
	(*action_in) = add_action((*action_in));

}

void Exeparser::setInterpoStatusAction(char* stat)
{
	if (strncmp(stat, "enable", 5) == 0)
	{
		(*action_in)->action = SET_INTERPO_STAT;
		(*action_in)->mode = INTERPOLATOR_ENABLE;
		(*action_in) = add_action((*action_in));
	}

	else if (strncmp(stat, "disable", 6) == 0)
	{
		(*action_in)->action = SET_INTERPO_STAT;
		(*action_in)->mode = INTERPOLATOR_DISABLE;
		(*action_in) = add_action((*action_in));
	}

	else
	{
		error(ERR_PARAMETR);
	}

}

void Exeparser::setInterpoPortAction(int port)
{
	(*action_in)->action = SET_INTERPO_PORT;
	(*action_in)->value = port;
	(*action_in) = add_action((*action_in));
}

void Exeparser::setInterpoBaudAction(int baud)
{
	(*action_in)->action = SET_INTERPO_BAUD;
	(*action_in)->value = baud;
	(*action_in) = add_action((*action_in));
}

void Exeparser::setInterpoMaxFrqAction(int val) 
{
	(*action_in)->action = SET_INTERPO_MAXFRQ;
	(*action_in)->value = val;
	(*action_in) = add_action((*action_in));
}

void Exeparser::setInterpoLoggingAction(char* mode)
{
	if (strncmp(mode, "stop", 6) == 0)
	{
		(*action_in)->action = SET_INTERPO_LOGGING;
		(*action_in)->mode = STOP_LOGGING;
		(*action_in) = add_action((*action_in));
	}
	else if (strncmp(mode, "start", 5) == 0)
	{
		(*action_in)->action = SET_INTERPO_LOGGING;
		(*action_in)->mode = START_LOGGING;
		(*action_in) = add_action((*action_in));

	}

	else
	{
		error(ERR_PARAMETR);
	}
}

void Exeparser::setInterpoBurstAction(char* mode)
{
	if (strncmp(mode, "stop", 6) == 0)
	{
		(*action_in)->action = SET_INTERPO_LOGGING;
		(*action_in)->mode = STOP_BURST;
		(*action_in) = add_action((*action_in));
	}
	else if (strncmp(mode, "start", 4) == 0)
	{
		(*action_in)->action = SET_INTERPO_LOGGING;
		(*action_in)->mode = START_BURST;
		(*action_in) = add_action((*action_in));
	}
	else if (strncmp(mode, "receive", 4) == 0)
	{
		(*action_in)->action = SET_INTERPO_LOGGING;
		(*action_in)->mode = RECEIVE_BURST;
		(*action_in) = add_action((*action_in));
	}
	else
	{
		error(ERR_PARAMETR);
	}
}

void Exeparser::setVirtualmacStatAction(char *stat)
{
	if (strncmp(stat, "enable", 5) == 0)
	{
		(*action_in)->action = SET_VIRTMAC_STAT;
		(*action_in)->mode = VIRTMAC_ENABLE;
		(*action_in) = add_action((*action_in));
	}

	else if (strncmp(stat, "disable", 6) == 0)
	{
		(*action_in)->action = SET_VIRTMAC_STAT;
		(*action_in)->mode = VIRTMAC_DISABLE;
		(*action_in) = add_action((*action_in));
	}

	else
	{
		error(ERR_PARAMETR);
	}

}

void Exeparser::setCloudNCStatAction(char *stat) 
{
	if (strncmp(stat, "enable", 5) == 0)
	{
		(*action_in)->action = SET_CLOUDNC_STAT;
		(*action_in)->mode = CLOUDNC_ENABLE;
		(*action_in) = add_action((*action_in));
	}

	else if (strncmp(stat, "disable", 6) == 0)
	{
		(*action_in)->action = SET_CLOUDNC_STAT;
		(*action_in)->mode = CLOUDNC_DISABLE;
		(*action_in) = add_action((*action_in));
	}

	else
	{
		error(ERR_PARAMETR);
	}
}

void Exeparser::setExecutiveStatAction(char *stat) 
{
	if (strncmp(stat, "disable", 5) == 0)
	{
		(*action_in)->action = SET_EXECUTIVE_STAT;
		(*action_in)->mode = EXECUTIVE_DISABLE;
		(*action_in) = add_action((*action_in));
	}
	else
	{
		error(ERR_PARAMETR);
	}
}

// get commands

void Exeparser::getStateVector(void)
{
	(*action_in)->action = PRINT_ACTION;
	(*action_in)->mode = PRINT_MACHINE_VECTOR;
	(*action_in) = add_action((*action_in));
}

void Exeparser::getAtomicActions(void)
{
	(*action_in)->action = PRINT_ACTION;
	(*action_in)->mode = PRINT_ATOMIC_ACTIONS;
	(*action_in) = add_action((*action_in));
}

void Exeparser::help(void) {
	putfile("./res/executive/config/help.txt");
}

void Exeparser::help_start(void) {
	putfile("./res/executive/config/help_start.txt");
}

void Exeparser::help_h(void) {
	putfile("./res/executive/config/help_h.txt");
}

void Exeparser::help_g(void) {
	putfile("./res/executive/config/help_g.txt");
}

void Exeparser::help_cloudnc(void) {
	putfile("./res/executive/config/help_cloudnc.txt");
}


// Executive parser private Services

void Exeparser::error(int num)
{
	// allocate memory for the message 
	std::string header = "Error: Exeparser " + std::to_string(num) + ": ";
	std::string body;

	// body 
	switch (num) {
	case ERR_PARAMETR:                 
		body = "Unidentified parameter: " + std::string(&exetext[0]);
		break;
	}

	// concatenate message
	static std::string message = header + body;

	// create error handler atomic action
	(*action_in)->action = EXEPARSER_ERROR;
	(*action_in)->error_message = message.c_str(); // 
	(*action_in)->lineno = exelineno;
	(*action_in)->variable = num;
	(*action_in) = add_action((*action_in));
}
