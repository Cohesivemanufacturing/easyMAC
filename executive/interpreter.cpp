/***************************************************
* easyMac Intepreter 1.0						  *
* Variables and function definitions              *
*                                                 *
* Jorge Correa & Placid Ferreira                  *
* 2016                                            *
*                                                 *
***************************************************/

#include "interpreter.h"

/**************************************************************
Lex and Yacc function definitions
***************************************************************/

void yyerror(char *s)
{
	// pointer to the head and tail actions
	Atomic_action **head = Interpreter::getActionIn();
	Atomic_action **tail = Interpreter::getActionOut();

	//format message
	static std::string message(s);

	// create  atomic action
	(*head)->action = INTERPRETER_ERROR;
	(*head)->error_message = message.c_str();
	(*head)->lineno = yylineno;
	(*head)->variable = ERR_YYERROR;
	(*head) = add_action((*tail));
}

/**************************************************************
Static functions outside the class
***************************************************************/


/**************************************************************
Interpreter object
***************************************************************/

// initialize Interpreter static members 
Interpreter	   *Interpreter::interpreter = NULL;   
H_variables    *Interpreter::h_variables = NULL;          
Modal_vector   *Interpreter::modal_vector = NULL;        
Block_vector   *Interpreter::block_vector = NULL;         
Machine_vector *Interpreter::machine_vector = NULL;       
Atomic_action  **Interpreter::action_in = NULL;            
Atomic_action  **Interpreter::action_out = NULL;

// initialize Interpreter state variables
volatile bool    Interpreter::m_checksEnabled  = true;


// Interpreter public services

Interpreter* Interpreter::initInterpreter(H_variables *variables, Block_vector *block, Modal_vector *modal, 
	Machine_vector *machine, Atomic_action **head, Atomic_action **tail)
{	
	interpreter = new Interpreter();
	
	// bind interpreter structures 
	h_variables    = variables;
	block_vector   = block;
	modal_vector   = modal;
	machine_vector = machine;
	action_in      = head;
	action_out     = tail;

	return interpreter;
}

Interpreter::~Interpreter(void)
{
	// free interpreter structures 
	h_variables = NULL;
	block_vector = NULL;
	modal_vector = NULL;
	machine_vector = NULL;
	action_in  = NULL;
	action_out = NULL;
}

void Interpreter::processBlock(void)
{
	// check the block
	if (m_checksEnabled)  // managed by the executive 
		checkBlockVector();

	// update modal vector // derecated (should happend at the executive level)
	update_modal_vector(modal_vector,block_vector);  // ModalVector object

	// compute atomic actions  
	generateActions();

	// empty block vector
	resetBlockVector();
}

// Interpreter private Services

void Interpreter::generateActions(void)
{
	/*
	* The atomic actions must excecute according to the order of excution in Table 8
	* of document "The NIST RS274/NGC Interpreter-Version 3" .
	* This function could have been programed as a switch-case statement with the added
	* difficulty of setting the feed rate, speed and tool in between of the exceution of
	* the prep. words
	*/

	// change sequence number
	if (block_vector->N != EMPTYCODE)
	{
		/*build atomic action*/
		(*action_in)->action = SET_SEQ_NUMBER;
		(*action_in)->sequence = block_vector->N;
		(*action_in) = add_action((*action_in));
	}

	// set inverse time feed rate mode
	if (ismodal(modal_vector, G93))
	{
		if (machine_vector->feed_mode != INVERSE_TIME)
		{
			/* build atomic action */
			(*action_in)->action = SET_FEED_MODE;
			(*action_in)->feed_mode = INVERSE_TIME;
			(*action_in) = add_action((*action_in));
		}
	}

	// set units per minute feed rate mode
	if (ismodal(modal_vector, G94))
	{
		if (machine_vector->feed_mode != UNITS_PER_MIN)
		{
			/* build atomic action */
			(*action_in)->action = SET_FEED_MODE;
			(*action_in)->feed_mode = UNITS_PER_MIN;
			(*action_in) = add_action((*action_in));
		}
	}

	// set units per revolution feed rate mode
	if (ismodal(modal_vector, G95))
	{
		if (machine_vector->feed_mode != UNITS_PER_REV)
		{
			/* build atomic action */
			(*action_in)->action = SET_FEED_MODE;
			(*action_in)->feed_mode = UNITS_PER_REV;
			(*action_in) = add_action((*action_in));
		}
	}

	// set feed rate
	if (!isnan(block_vector->F))
	{
		/*build atomic action*/
		(*action_in)->action = SET_FEED_RATE;
		(*action_in)->feed = block_vector->F;
		(*action_in) = add_action((*action_in));
	}
	
	// set spindle speed
	if (!isnan(block_vector->S))
	{
		/*build atomic action*/
		(*action_in)->action = SET_SPINDLE_SPEED;
		(*action_in)->speed = block_vector->S;
		(*action_in) = add_action((*action_in));
	}

	// set dwell (G04)

	// set XY plane
	if (ismodal(modal_vector, G17))
	{
		if (machine_vector->plane != XY)
		{
			/* build atomic action */
			(*action_in)->action = SET_PLANE;
			(*action_in)->plane = XY;
			(*action_in) = add_action((*action_in));
		}
	}

	// set ZX plane
	if (ismodal(modal_vector, G18))
	{
		if (machine_vector->plane != ZX)
		{
			/* build atomic action */
			(*action_in)->action = SET_PLANE;
			(*action_in)->plane = ZX;
			(*action_in) = add_action((*action_in));
		}
	}

	// set YZ plane
	if (ismodal(modal_vector, G19))
	{
		if (machine_vector->plane != YZ)
		{
			/* build atomic action */
			(*action_in)->action = SET_PLANE;
			(*action_in)->plane = YZ;
			(*action_in) = add_action((*action_in));
		}
	}

	// set IN length units 
	if (ismodal(modal_vector, G20))
	{
		if (machine_vector->units != IN)
		{
			/* build atomic action */
			(*action_in)->action = SET_LENGTH_UNITS;
			(*action_in)->units = IN;
			(*action_in) = add_action((*action_in));
		}
	}

	// set MM length units 
	if (ismodal(modal_vector, G21))
	{
		if (machine_vector->units != MM)
		{
			/* build atomic action */
			(*action_in)->action = SET_LENGTH_UNITS;
			(*action_in)->units = MM;
			(*action_in) = add_action((*action_in));
		}
	}

	// set cutter radius compensation DISABLE
	if (ismodal(modal_vector, G40))
	{
		if (machine_vector->radius_comp != RCOMP_DISABLE)
		{
			/* build atomic action */
			(*action_in)->action = SET_RADIUS_COMP;
			(*action_in)->radius_comp = RCOMP_DISABLE;
			(*action_in) = add_action((*action_in));
		}
	}

	// set cutter radius compensation TOOL_LEFT
	if (ismodal(modal_vector, G41))
	{
		if (machine_vector->radius_comp != TOOL_LEFT)
		{
			/* build atomic action */
			(*action_in)->action = SET_RADIUS_COMP;
			(*action_in)->radius_comp = TOOL_LEFT;
			(*action_in) = add_action((*action_in));
		}
	}

	// set cutter radius compensation TOOL_LEFT
	if (ismodal(modal_vector, G42))
	{
		if (machine_vector->radius_comp != TOOL_RIGHT)
		{
			/* build atomic action */
			(*action_in)->action = SET_RADIUS_COMP;
			(*action_in)->radius_comp = TOOL_RIGHT;
			(*action_in) = add_action((*action_in));
		}
	}

	// set cutter length compensation ENABLE 
	if (ismodal(modal_vector, G43))
	{
		if (machine_vector->radius_comp != LCOMP_ENABLE)
		{
			/* build atomic action */
			(*action_in)->action = SET_LENGTH_COMP;
			(*action_in)->length_comp = LCOMP_ENABLE;
			(*action_in) = add_action((*action_in));
		}
	}

	// set cutter length compensation DISABLE 
	if (ismodal(modal_vector, G49))
	{
		if (machine_vector->radius_comp != LCOMP_DISABLE)
		{
			/* build atomic action */
			(*action_in)->action = SET_LENGTH_COMP;
			(*action_in)->length_comp = LCOMP_DISABLE;
			(*action_in) = add_action((*action_in));
		}
	}

	// set coordinate system G54
	if (ismodal(modal_vector, G54))
	{
		if (machine_vector->coordinate_system != G54)
		{
			/* build atomic action */
			(*action_in)->action = SET_COORD_SYSTEM;
			(*action_in)->coordinate_system = G54;
			(*action_in) = add_action((*action_in));
		}
	}

	// set ABS coordinates
	if (ismodal(modal_vector, G90))
	{
		if (machine_vector->distance_mode != ABS)
		{
			/* build atomic action */
			(*action_in)->action = SET_DISTANCE_MODE;
			(*action_in)->distance_mode = ABS;
			(*action_in) = add_action((*action_in));
		}
	}

	// set REL coordinates
	if (ismodal(modal_vector, G91))
	{
		if (machine_vector->distance_mode != REL)
		{
			/* build atomic action */
			(*action_in)->action = SET_DISTANCE_MODE;
			(*action_in)->distance_mode = REL;
			(*action_in) = add_action((*action_in));
		}
	}

	// set origin offsets
	if (ismodal(modal_vector, G92))
	{
		/* build atomic action */
		(*action_in)->action = SET_ORIGIN_OFFSET;
		(*action_in) = setOriginOffset((*action_in));
		(*action_in) = add_action((*action_in)); // always incremental values
	}

	// rapid motion
	if (ismodal(modal_vector, G00))
	{
		/* set motion mode to rapid */
		if (machine_vector->motion_mode != RAPID)
		{
			(*action_in)->action = SET_MOTION_MODE;
			(*action_in)->motion_mode = RAPID;
			(*action_in) = add_action((*action_in));
		}

		/* move the axes */
		if (newDimensions())
		{
			(*action_in)->action = STRAIGHT_FEED;
			(*action_in) = setIncrements((*action_in));
			(*action_in) = add_action((*action_in)); // always incremental values
		}
	}

	// linear motion
	else if (ismodal(modal_vector, G01))
	{
		/* set motion mode to linear */
		if (machine_vector->motion_mode != LINEAR)
		{
			(*action_in)->action = SET_MOTION_MODE;
			(*action_in)->motion_mode = LINEAR;
			(*action_in) = add_action((*action_in));
		}

		/* move the axes */
		if (newDimensions())
		{
			(*action_in)->action = STRAIGHT_FEED;
			(*action_in) = setIncrements((*action_in));
			(*action_in) = add_action((*action_in)); // always incremental values
		}
	}

	// clockwise motion
	else if (ismodal(modal_vector, G02))
	{
		/* set motion mode to linear */
		if (machine_vector->motion_mode != CW)
		{
			(*action_in)->action = SET_MOTION_MODE;
			(*action_in)->motion_mode = CW;
			(*action_in) = add_action((*action_in));
		}

		/* move the axes */
		if (newDimensions())
		{
			generateArcs();
		}
	}

	// counter-clockwise motion
	else if (ismodal(modal_vector, G03))
	{
		/* set motion mode to linear */
		if ((*action_in)->motion_mode != CCW)
		{
			(*action_in)->action = SET_MOTION_MODE;
			(*action_in)->motion_mode = CCW;
			(*action_in) = add_action((*action_in));
		}

		/* move the axes */
		if (newDimensions())
		{
			generateArcs();
		}
	}
}

Atomic_action *Interpreter::setOriginOffset(Atomic_action *node)
{
	// check the machine state vector to find the current coordynate system
	struct offset origin;
	origin = offset();

	switch (machine_vector->coordinate_system) {
	case G54:
		origin.X = h_variables->H[G54_ORIGIN_X];
		origin.Y = h_variables->H[G54_ORIGIN_Y];
		origin.Z = h_variables->H[G54_ORIGIN_Z];
		break;
	case G55:
		origin.X = h_variables->H[G55_ORIGIN_X];
		origin.Y = h_variables->H[G55_ORIGIN_Y];
		origin.Z = h_variables->H[G55_ORIGIN_Z];
		break;
	case G56:
		origin.X = h_variables->H[G56_ORIGIN_X];
		origin.Y = h_variables->H[G56_ORIGIN_Y];
		origin.Z = h_variables->H[G56_ORIGIN_Z];
		break;
	case G57:
		origin.X = h_variables->H[G57_ORIGIN_X];
		origin.Y = h_variables->H[G57_ORIGIN_Y];
		origin.Z = h_variables->H[G57_ORIGIN_Z];
		break;
	case G58:
		origin.X = h_variables->H[G58_ORIGIN_X];
		origin.Y = h_variables->H[G58_ORIGIN_Y];
		origin.Z = h_variables->H[G58_ORIGIN_Z];
		break;
	case G59:
		origin.X = h_variables->H[G59_ORIGIN_X];
		origin.Y = h_variables->H[G59_ORIGIN_Y];
		origin.Z = h_variables->H[G59_ORIGIN_Z];
		break;
	}
	return calculateOffsets(node, origin);

}

Atomic_action *Interpreter::setIncrements(Atomic_action *node)
{
	if (isnan(block_vector->X)) {
		node->position.X = 0;
	}
	else {
		node->position.X = block_vector->X - (ismodal(modal_vector, G90) ? getTargetPosition().X : 0);
	}

	if (isnan(block_vector->Y)) {
		node->position.Y = 0;
	}
	else {
		node->position.Y = block_vector->Y - (ismodal(modal_vector, G90) ? getTargetPosition().Y : 0);
	}

	if (isnan(block_vector->Z)) {
		node->position.Z = 0;
	}
	else {
		node->position.Z = block_vector->Z - (ismodal(modal_vector, G90) ? getTargetPosition().Z : 0);
	}

	if (isnan(block_vector->I)) {
		node->position.I = machine_vector->target_position.I;
	}
	else {
		node->position.I = block_vector->I;
	}

	if (isnan(block_vector->J)) {
		node->position.J = machine_vector->target_position.J;
	}
	else {
		node->position.J = block_vector->J;
	}

	if (isnan(block_vector->K)) {
		node->position.K = machine_vector->target_position.K;
	}
	else {
		node->position.K = block_vector->K;
	}

	return node;
}

Atomic_action *Interpreter::calculateOffsets(Atomic_action *node, struct offset origin)
{
	/*
	* X = Xmachine - Xorigin - Xoffset or,
	* Xoffset = Xmachine - Xorigin - X
	*
	* where:
	* X		   : current cordinate of the machine in X
	* Xmachine : component of the machine's native coordinate system origin (G53) in X
	* Xorigin  : component of the current coordinate system in X; e.g G54
	*/

	if (isnan(block_vector->X)) {
		node->origin_offset.X = machine_vector->target_position.X - origin.X;
	}
	else {
		node->origin_offset.X = machine_vector->target_position.X - origin.X - block_vector->X;
	}

	if (isnan(block_vector->Y)) {
		node->origin_offset.Y = machine_vector->target_position.Y - origin.Y;
	}
	else {
		node->origin_offset.Y = machine_vector->target_position.Y - origin.Y - block_vector->Y;
	}

	if (isnan(block_vector->Z)) {
		node->origin_offset.Z = machine_vector->target_position.Z - origin.Z;
	}
	else {
		node->origin_offset.Z = machine_vector->target_position.Z - origin.Z - block_vector->Z;
	}

	return node;
}

void Interpreter::getIncrements(axis *increment)
{

	if (isnan(block_vector->X)) {
		increment->X = 0;
	}
	else {
		increment->X = block_vector->X - (ismodal(modal_vector, G90) ? getTargetPosition().X : 0);
	}

	if (isnan(block_vector->Y)) {
		increment->Y = 0;
	}
	else {
		increment->Y = block_vector->Y - (ismodal(modal_vector, G90) ? getTargetPosition().Y : 0);
	}

	if (isnan(block_vector->Z)) {
		increment->Z = 0;
	}
	else {
		increment->Z = block_vector->Z - (ismodal(modal_vector, G90) ? getTargetPosition().Z : 0);
	}

	if (isnan(block_vector->I)) {
		increment->I = machine_vector->target_position.I;
	}
	else {
		increment->I = block_vector->I;
	}

	if (isnan(block_vector->J)) {
		increment->J = machine_vector->target_position.J;
	}
	else {
		increment->J = block_vector->J;
	}

	if (isnan(block_vector->K)) {
		increment->K = machine_vector->target_position.K;
	}
	else {
		increment->K = block_vector->K;
	}
}

axis Interpreter::getTargetPosition(void)
{
	/*
	* X = Xmachine - Xorigin - Xoffset or,
	* Xoffset = Xmachine - Xorigin - X
	*/

	struct axis result;

	float Xorigin = 0;
	float Yorigin = 0;
	float Zorigin = 0;

	float Xoffset = h_variables->H[ORGIN_OFFSET_X];
	float Yoffset = h_variables->H[ORGIN_OFFSET_Y];
	float Zoffset = h_variables->H[ORGIN_OFFSET_Z];

	switch (machine_vector->coordinate_system)
	{
	case G54:
		Xorigin = h_variables->H[G54_ORIGIN_X];
		Yorigin = h_variables->H[G54_ORIGIN_Y];
		Zorigin = h_variables->H[G54_ORIGIN_Z];
		break;
	case G55:
		Xorigin = h_variables->H[G55_ORIGIN_X];
		Yorigin = h_variables->H[G55_ORIGIN_Y];
		Zorigin = h_variables->H[G55_ORIGIN_Z];
		break;
	case G56:
		Xorigin = h_variables->H[G56_ORIGIN_X];
		Yorigin = h_variables->H[G56_ORIGIN_Y];
		Zorigin = h_variables->H[G56_ORIGIN_Z];
		break;
	case G57:
		Xorigin = h_variables->H[G57_ORIGIN_X];
		Yorigin = h_variables->H[G57_ORIGIN_Y];
		Zorigin = h_variables->H[G57_ORIGIN_Z];
		break;
	case G58:
		Xorigin = h_variables->H[G58_ORIGIN_X];
		Yorigin = h_variables->H[G58_ORIGIN_Y];
		Zorigin = h_variables->H[G58_ORIGIN_Z];
		break;
	case G59:
		Xorigin = h_variables->H[G59_ORIGIN_X];
		Yorigin = h_variables->H[G59_ORIGIN_Y];
		Zorigin = h_variables->H[G59_ORIGIN_Z];
		break;
	}

	result.X = machine_vector->target_position.X - Xorigin - Xoffset;
	result.Y = machine_vector->target_position.Y - Yorigin - Yoffset;
	result.Z = machine_vector->target_position.Z - Zorigin - Zoffset;
	result.I = machine_vector->target_position.I;
	result.J = machine_vector->target_position.J;
	result.K = machine_vector->target_position.K;
	return result;
}

void Interpreter::generateArcs(void)
{
	// variable initialization	
	axis arc     = axis();  // incremental vectors for the full arc
	axis arc_xy  = axis();  // incremental vectors for the full equivalent xy arc
	axis arci    = axis();  // incremental vectors for the partial arcs
	axis arci_xy = axis();  // incremental vectors for the partial quivalent xy arc

	// calculates the targets of the incremental arc
	getIncrements(&arc);

	// convert the arc targets to XY-arc equivalent targets
	arc_to_arcxy(&arc, &arc_xy);

	// my initial and final quadrants
	int q0 = initialQuadrant(arc_xy.I,
		arc_xy.J);

	int qf = finalQuadrant(arc_xy.I - arc_xy.X,
		arc_xy.J - arc_xy.Y);

	//number of arcs moves to go from q0 to qf
	int n = numberOfQuadrants(q0,
							  qf,
							  arc_xy.I,
							  arc_xy.J,
							  arc_xy.I - arc_xy.X,
							  arc_xy.J - arc_xy.Y);

	//if it is a single quadrant motion
	if (n == 1)
	{
		// generate the atomic action
		(*action_in)->action = ARC_FEED;
		(*action_in)->position = arc;
		(*action_in) = add_action((*action_in)); // always incremental values

		if (!checkArcFeed((*action_in)))
		{
			// report to error handler
			error(ERR_ARCDIMEN);
		}
		return;
	}

	// gotta work harder..
	else
	{
		// radius of the arc 
		float R = sqrt(pow(arc_xy.I, 2) + pow(arc_xy.J, 2));

		// get state array
		mat4 state_array = getStateArray(R);

		// intermediate blocks are generated here
		for (int i = 1; i <= n; i++)
		{
			/*
			* this lines and the lines in function calc_n can be writen in terms of modulo operator.
			* if the pointers to the quadrants start from 0 to 3. for example qo = (qo + 1 ) % 4,
			* if q0 = -1 -> qo = 3. Also if q0 = 4 -> qo = 0.
			*/
			if (q0 < 1) q0 = 4;
			if (q0 > 4) q0 = 1;

			// last block is generated here.
			if (i == 1)
			{
				// first intermediate block is generated here. 
				arci_xy.I = arc_xy.I;
				arci_xy.J = arc_xy.J;
				arci_xy.X = arc_xy.I - state_array[q0 - 1][2] + state_array[q0 - 1][0];
				arci_xy.Y = arc_xy.J - state_array[q0 - 1][3] + state_array[q0 - 1][1];
			}
			// last block is generated here
			else if (i == n)
			{
				arci_xy.I = state_array[q0 - 1][2];
				arci_xy.J = state_array[q0 - 1][3];
				arci_xy.X = arci_xy.I + arc_xy.X - arc_xy.I;
				arci_xy.Y = arci_xy.J + arc_xy.Y - arc_xy.J;
			}
			// the other blocks are generated here
			else
			{
				arci_xy.I = state_array[q0 - 1][2];
				arci_xy.J = state_array[q0 - 1][3];
				arci_xy.X = state_array[q0 - 1][0];
				arci_xy.Y = state_array[q0 - 1][1];
			}

			// generates the atomic action
			arcxy_to_arc(&arci_xy, &arci);
			(*action_in)->action = ARC_FEED;
			(*action_in)->position = arci;
			(*action_in) = add_action((*action_in)); // always incremental values

			if (!checkArcFeed((*action_in)))
			{
				// report to error handler
				error(ERR_ARCDIMEN);
			}

			// updates quadrant
			if (ismodal(modal_vector, G02)) q0--;
			if (ismodal(modal_vector, G03)) q0++;
		}
	}
}

int Interpreter::initialQuadrant(float I, float J)
{
	/**
	whereAmI, whereIgo
	uses the value of I and J to find the starting quadrant "qo" w.r.t the center of rotation. The boundaries of the quadrants
	change depending on the orientation of the motion and wheater we are looking for the starting or final quadrant. For example,
	for the starting point, the first quadrant in clockwise motion corresponds to the sub-region I <= 0 && J < 0. This means that
	the positive Y-axis is included in this region while the X-axis is left out.  On the countrary, for the final point, the first
	quadrant in clockwise motion correspond to the sub-region Xf <= 0 && Yf < 0. This means that the positive Y-axis is left out
	while the positive X-axis is included in the region. In this manner, if a circular clockwise motion starts at I = 0 and finishes
	at J = 0, q0 = 1 and qf = 1 and the total amount of moves count = quad0 - quadf + 1 = 1;
	*/

	// the starting quadrant depends on the orientation of the motion
	int q = 0;

	if (ismodal(modal_vector, G02))
	{
		if (I <= 0 && J < 0) {        // first quadrant
			q = 1;
		}
		else if (I > 0 && J <= 0) { // second quadrant
			q = 2;
		}
		else if (I >= 0 && J > 0) { // third quadrant
			q = 3;
		}
		else {//(I < 0 && J >= 0)   // forth quadrant                     
			q = 4;
		}
	}

	if (ismodal(modal_vector, G03))
	{
		if (I < 0 && J <= 0) {        // first quadrant
			q = 1;
		}
		else if (I >= 0 && J < 0) {   // second quadrant
			q = 2;
		}
		else if (I>0 && J >= 0) {   // third quadrant
			q = 3;
		}
		else {//(I<=0 && J > 0)     // forth quadrant
			q = 4;
		}
	}

	return q;
}

int Interpreter::finalQuadrant(float Xf, float Yf)
{
	// the starting quadrant depends on the orientation of the motion
	int q = 0;

	if (ismodal(modal_vector, G02))
	{
		if (Xf < 0 && Yf <= 0) {         // first quadrant
			q = 1;
		}
		else if (Xf >= 0 && Yf < 0) {  // first quadrant
			q = 2;
		}
		else if (Xf > 0 && Yf >= 0) {  // first quadrant
			q = 3;
		}
		else {//(Xf <= 0 && Yf > 0)    // forth quadrant  
			q = 4;
		}
	}

	if (ismodal(modal_vector, G03))
	{
		if (Xf <= 0 && Yf < 0) {         // first quadrant
			q = 1;
		}
		else if (Xf > 0 && Yf <= 0) {  // second quadrant
			q = 2;
		}
		else if (Xf >= 0 && Yf > 0) {   // third quadrant
			q = 3;
		}
		else {//(I<=0 && J > 0)        // forth quadrant
			q = 4;
		}
	}
	return q;
}

int Interpreter::numberOfQuadrants(int quad0, int quadf, float xo, float yo, float xf, float yf)
{
	/*
	calc_n
	This function calculates the number of blocks needed to be genearated when going from the init. quadrant qo to the
	final quadrant qf. In the case of qo = qf, the program assigns a number of blocks equal to one or five depending on
	wheather the final poitn (xf,yf) is in front or behind the initial point (I, J).
	*/

	int count = 0;
	//spatial case: points are in the same quadrant
	if (quad0 == quadf) {
		if (classifyPoints(xo, yo, xf, yf)) {
			return(count = 1);
		}
		else {
			return(count = 5);
		}
	}
	else {
		// in general..
		if (ismodal(modal_vector, G02))
		{
			count = quad0 - quadf + 1;  // or if q0 = 0, 1, 2, 3 and qf = 0, 1, 2, 3 then count = (q0 - qf) % 4 +1
			if (count <= 0) {
				count = count + 4;
			}
		}

		if (ismodal(modal_vector, G03))
		{
			count = quadf - quad0 + 1;
			if (count <= 0) {
				count = count + 4;
			}

		}
		return (count);
	}
}

bool Interpreter::classifyPoints(float xo, float yo, float xf, float yf)
{
	/* classifyPnts
	* This funtion takes the coordinates of the initial and final points w.r.t the center of rotation and figures out if
	* the final point in ahead of before the initial point depending on the orientation of the motion. the classification is
	* done by computing the sign of the cross product between the vector from the I.C.R to the final point and the vector from the I.C.R to
	* initial point.
	*/

	bool pntInFront = false;
	float classifier = xf*yo - yf*xo;

	if (ismodal(modal_vector, G02))
	{
		if (classifier > 0) {
			pntInFront = true;
		}
	}
	if (ismodal(modal_vector, G03))
	{
		if (classifier < 0) {
			pntInFront = true;
		}
	}
	return pntInFront;
}

mat4 Interpreter::getStateArray(float R)
{
	std::array<std::array<float, 4>, 4>  result;

	if (ismodal(modal_vector, G02))
	{
		result =
		{ {
			{ { R,  -R,   0, -R } },
			{ { R,   R,   R,  0 } },
			{ { -R,  R,   0,  R } },
			{ { -R, -R,  -R,  0 } }
			} };
	}
	else
	{
		result =
		{ {
			{ { -R,  R, -R,  0 } },
			{ { -R, -R,  0, -R } },
			{ { R, -R,  R,  0 } },
			{ { R,  R,  0,  R } }
			} };
	}

	return result;
}

bool Interpreter::checkArcFeed(Atomic_action *node)
{
	bool result = true;

	int error = 0;

	int X = (int)(node->position.X * getMachineBLU());
	int Y = (int)(node->position.Y * getMachineBLU());
	int Z = (int)(node->position.Z * getMachineBLU());
	int I = (int)(node->position.I * getMachineBLU());
	int J = (int)(node->position.J * getMachineBLU());
	int K = (int)(node->position.K * getMachineBLU());

	switch (machine_vector->plane)
	{
	case XY:
		error = abs(sqrt(pow(I, 2) + pow(J, 2)) - sqrt(pow(I - X, 2) + pow(J - Y, 2)));
		break;
	case ZX:
		error = abs(sqrt(pow(K, 2) + pow(I, 2)) - sqrt(pow(K - Z, 2) + pow(I - X, 2)));
		break;
	case YZ:
		error = abs(sqrt(pow(J, 2) + pow(K, 2)) - sqrt(pow(J - Y, 2) + pow(K - Z, 2)));
		break;
	}

	if (error > h_variables->H[ARC_TOL])
		result = false;

	return result;
}

void Interpreter::checkBlockVector(void)
{
	queue *gcodes = &block_vector->G;
	queue *mcodes = &block_vector->M;

	// check for invalid G codes
	checkValidCodes(gcodes);

	// check for invalid M codes
	checkValidCodes(mcodes);

	//check for repeated G codes
	checkRepeatedCodes(gcodes);

	// check for repeated M codes
	checkRepeatedCodes(mcodes);

	// check for repeated G modals
	checkRepeatedModals(gcodes);

	//checks for repeated M modals
	checkRepeatedModals(mcodes);

	// check for code error
	checkCodeErrors();

}

void Interpreter::checkValidCodes(queue *q)
{
	int head = q->queue_in;
	int tail = q->queue_out;
	int size = q->size;

	// loop around the queue looking for invalid G codes
	while (tail != head)
	{
		int code = q->queue[tail];

		if (find_group(code) == EMPTYCODE)
		{
			error(ERR_INVGCODE, code);
		}
		tail = (tail + 1) % size;
	}
}

void Interpreter::checkRepeatedCodes(queue *q) {

	int head = q->queue_in;
	int tail_i = q->queue_out;
	int size = q->size;

	while (tail_i != head)
	{
		int tail_j = (tail_i + 1) % size;

		while (tail_j != head)
		{
			int code1 = q->queue[tail_i];
			int code2 = q->queue[tail_j];

			if (code1 == code2)
			{
				error(ERR_REPGWORD, code1);
			}
			tail_j = (tail_j + 1) % size;
		}
		tail_i = (tail_i + 1) % size;
	}
}

void Interpreter::checkRepeatedModals(queue *q) // bug, multiple messages
{
	int head = q->queue_in;
	int tail = q->queue_out;
	int size = q->size;

	int flags[NUM_MODALS] = { false };

	while (tail != head)
	{
		int code = q->queue[tail];
		int group = find_group(code);

		// there can be more than one non-modals
		if (group != 0) {
			// if modal
			if (flags[group] == false) {
				flags[group] = true;
			}
			else {
				error(ERR_REPMODAL, group);
			}
		}
		tail = (tail + 1) % size;
	}
}

void Interpreter::checkCodeErrors(void) {

	int head = block_vector->G.queue_in;
	int tail = block_vector->G.queue_out;
	int size = block_vector->G.size;

	// initialize flags
	int modal0_motion_code = EMPTYCODE;
	int modal1_motion_code = EMPTYCODE;

	while (tail != head) {

		int code = block_vector->G.queue[tail];

		switch (code) {
		case G00:
			/*checks for conflicting motion commands*/
			modal0_motion_code = G00;
			if (modal1_motion_code != EMPTYCODE) {
				error(ERR_REPMOTWD, modal0_motion_code, modal1_motion_code);
			}
			break;
		case G01:
			/*checks for conflicting motion commands*/
			modal1_motion_code = G01;
			if (modal0_motion_code != EMPTYCODE) {
				error(ERR_REPMOTWD, modal0_motion_code, modal1_motion_code);
			}
			/*checks for feed set */
			if (machine_vector->feed == EMPTYCODE && isnan(block_vector->F)) {
				error(ERR_FEEDSET);
			}
			break;
		case G02:
			/*checks for conflicting motion commands*/
			modal1_motion_code = G02;
			if (modal0_motion_code != EMPTYCODE) {
				error(ERR_REPMOTWD, modal0_motion_code, modal1_motion_code);
			}
			/*checks for feed set */
			if (machine_vector->feed == EMPTYCODE && isnan(block_vector->F)) {
				error(ERR_FEEDSET);
			}
			break;
		case G03:
			/*checks for conflicting motion commands*/
			modal1_motion_code = G03;
			if (modal0_motion_code != EMPTYCODE) {
				error(ERR_REPMOTWD, modal0_motion_code, modal1_motion_code);
			}
			/*checks for feed set */
			if (machine_vector->feed == EMPTYCODE && isnan(block_vector->F)) {
				error(ERR_FEEDSET);
			}
			break;
		case G10:
			/*checks for conflicting motion commands*/
			modal0_motion_code = G10;
			if (modal1_motion_code != EMPTYCODE) {
				error(ERR_REPMOTWD, modal0_motion_code, modal1_motion_code);
			}
			break;
		case G28:
			/*checks for conflicting motion commands*/
			modal0_motion_code = G28;
			if (modal1_motion_code != EMPTYCODE) {
				error(ERR_REPMOTWD, modal0_motion_code, modal1_motion_code);
			}
			break;
		case G30:
			/*checks for conflicting motion commands*/
			modal0_motion_code = G30;
			if (modal1_motion_code != EMPTYCODE) {
				error(ERR_REPMOTWD, modal0_motion_code, modal1_motion_code);
			}
			break;
		case G92:
			/*checks for conflicting motion commands*/
			modal0_motion_code = G92;
			if (modal1_motion_code != EMPTYCODE) {
				error(ERR_REPMOTWD, modal0_motion_code, modal1_motion_code);
			}
			break;
		}

		tail = (tail + 1) % size;
	}
}

bool Interpreter::newDimensions(void)
{
	if (!isnan(block_vector->X)) {
		return true;
	}
	if (!isnan(block_vector->Y)) {
		return true;
	}
	if (!isnan(block_vector->Z)) {
		return true;
	}
	if (!isnan(block_vector->I)) {
		return true;
	}
	if (!isnan(block_vector->J)) {
		return true;
	}
	if (!isnan(block_vector->K)) {
		return true;
	}
	return false;
}

void Interpreter::error(int num)
{
	// allocate memory for the message 
	std::string header = "Error: Interpreter " + std::to_string(num) + ": ";
	std::string body;

	// body 
	switch (num) {
	case ERR_FEEDSET:                  // deprecated to exeparser error handler
		resetBlockVector();
		body = "Unspecified feed rate parameter";
		break;
	case ERR_ARCDIMEN:
		body = "Incorrect arc segment dimensions";
		// query the user to coninue or termnate the program 
		//while (1)
		//{
		//	printf("do you want to continue execution [y/n]: ");
		//	char c = getchar();
		//	if (c == 'y')
		//		break;
		//	if (c == 'n')
		//	{
		//		// generate atomic action to stop program
		//		(*action_in)->action = SET_PROGRAM;
		//		(*action_in)->mode = PROGRAM_STOP;
		//		(*action_in) = add_action((*action_in));
		//	}
		//}
		break;
	}

	// concatenate message
	static std::string message = header + body;
	
	// create error handler atomic action
	(*action_in)->action = INTERPRETER_ERROR;
	(*action_in)->error_message = message.c_str(); // 
	(*action_in)->lineno = yylineno;
	(*action_in)->variable = num;
	(*action_in) = add_action((*action_in));
}

void Interpreter::error(int num, int val)
{
	// allocate memory for the message 
	std::string header = "Error: Interpreter %d: ";
	std::string body;

	// body 
	switch (num) {
	case ERR_INVGCODE:
		resetBlockVector();
		body = "Cannot identify code: G" + std::to_string(val);
		break;
	case ERR_INVMCODE:
		resetBlockVector();
		body = "Cannot identify code: M" + std::to_string(val);
		break;
	case ERR_REPMODAL:
		resetBlockVector();
		body = "Repeated modal group: " + std::to_string(val);
		break;
	case ERR_REPGWORD:
		resetBlockVector();
		body = "Repeated code: G" + std::to_string(val);
		break;
	case ERR_REPMWORD:
		resetBlockVector();
		body = "Repeated code: G" + std::to_string(val);
		break;
	}

	// concatenate message
	static std::string message = header + body;

	// create error handler atomic action
	(*action_in)->action = INTERPRETER_ERROR;
	(*action_in)->error_message = message.c_str(); // 
	(*action_in)->lineno = yylineno;
	(*action_in)->variable = num;
	(*action_in) = add_action((*action_in));
}

void Interpreter::error(int num, int val1, int val2) // overloaded function
{
	// allocate memory for the message 
	std::string header = "Error: Interpreter %d: ";
	std::string body;

	switch (num) {
	case ERR_REPMOTWD:
		resetBlockVector();
		body = "Conflicting motion commands:  G" + std::to_string(val1) + " G" + std::to_string(val1);
		break;
	default:
		break;
	}

	// concatenate message
	static std::string message = header + body;

	// create error handler atomic action
	(*action_in)->action = INTERPRETER_ERROR;
	(*action_in)->error_message = message.c_str(); // 
	(*action_in)->lineno = yylineno;
	(*action_in)->variable = num;
	(*action_in) = add_action((*action_in));
}

/**************************************************************
Utility Functions Outside the class
***************************************************************/

void print_interpreter_variable(int var) {

	JSON_Value *root_value = json_value_init_object();
	JSON_Object *root_object = json_value_get_object(root_value);
	char *serialized_string = NULL;

	printf("\n");

	char str[8];
	sprintf(str, "H%d", var);

	json_object_set_number(root_object, str, Interpreter::getHvariable(var)); 
	serialized_string = json_serialize_to_string_pretty(root_value);

	/* print to the console */
	puts(serialized_string);

	json_free_serialized_string(serialized_string);
	json_value_free(root_value);
}

void print_modal_vector(void)
{
	Modal_vector *modal_vector = Interpreter::getModalVector();
	
	JSON_Value *root_value = json_value_init_object();
	JSON_Object *root_object = json_value_get_object(root_value);

	char *serialized_string = NULL;

	printf("\n");

	json_object_set_string(root_object, "type", "modal vector");

	/* modals codes */
	char non_modals[100];
	serialize_queue(non_modals, &modal_vector->modal0);
	json_object_dotset_value(root_object, "modal 0", json_parse_string(non_modals));

	/* modal 1 */
	if (modal_vector->modal1 != EMPTYCODE) {
		json_object_set_number(root_object, "modal 1", modal_vector->modal1);
	}

	/* modal 2 */
	if (modal_vector->modal2 != EMPTYCODE) {
		json_object_set_number(root_object, "modal 2", modal_vector->modal2);
	}

	/* modal 3 */
	if (modal_vector->modal3 != EMPTYCODE) {
		json_object_set_number(root_object, "modal 3", modal_vector->modal3);
	}

	/* modal 4 */
	if (modal_vector->modal4 != EMPTYCODE) {
		json_object_set_number(root_object, "modal 4", modal_vector->modal4);
	}

	/* modal 5 */
	if (modal_vector->modal5 != EMPTYCODE) {
		json_object_set_number(root_object, "modal 5", modal_vector->modal5);
	}

	/* modal 6 */
	if (modal_vector->modal6 != EMPTYCODE) {
		json_object_set_number(root_object, "modal 6", modal_vector->modal6);
	}

	/* modal 7 */
	if (modal_vector->modal7 != EMPTYCODE) {
		json_object_set_number(root_object, "modal 7", modal_vector->modal7);
	}

	/* modal 8 */
	if (modal_vector->modal8 != EMPTYCODE) {
		json_object_set_number(root_object, "modal 8", modal_vector->modal8);
	}

	/* modal 9 */
	if (modal_vector->modal9 != EMPTYCODE) {
		json_object_set_number(root_object, "modal 9", modal_vector->modal9);
	}

	/* modal 10 */
	if (modal_vector->modal10 != EMPTYCODE) {
		json_object_set_number(root_object, "modal 10", modal_vector->modal10);
	}

	/* modal 11 */
	if (modal_vector->modal11 != EMPTYCODE) {
		json_object_set_number(root_object, "modal 11", modal_vector->modal11);
	}

	/* modal 12 */
	if (modal_vector->modal12 != EMPTYCODE) {
		json_object_set_number(root_object, "modal 12", modal_vector->modal12);
	}

	/* modal 13 */
	if (modal_vector->modal13 != EMPTYCODE) {
		json_object_set_number(root_object, "modal 13", modal_vector->modal13);
	}

	/* print to the console */
	serialized_string = json_serialize_to_string_pretty(root_value);
	puts(serialized_string);

	/* free memory */
	json_free_serialized_string(serialized_string);
	json_value_free(root_value);

}

void print_block_vector(void)
{
	Block_vector *block_vector = Interpreter::getBlockVector();

	JSON_Value *root_value = json_value_init_object();
	JSON_Object *root_object = json_value_get_object(root_value);

	char *serialized_string = NULL;

	printf("\n");

	json_object_set_string(root_object, "type", "block vector");

	/* seq. word */
	if (block_vector->N != EMPTYCODE) {
		json_object_set_number(root_object, "N", block_vector->N);
	}

	/* G codes */
	char gcodes[100];
	serialize_queue(gcodes, &block_vector->G);
	json_object_dotset_value(root_object, "G", json_parse_string(gcodes));

	/* x dimension */
	if (!isnan(block_vector->X)) {
		json_object_set_number(root_object, "X", block_vector->X);
	}

	/* y dimension */
	if (!isnan(block_vector->Y)) {
		json_object_set_number(root_object, "Y", block_vector->Y);
	}

	/* z dimension */
	if (!isnan(block_vector->Z)) {
		json_object_set_number(root_object, "Z", block_vector->Z);
	}

	/* i dimension */
	if (!isnan(block_vector->I)) {
		json_object_set_number(root_object, "I", block_vector->I);
	}

	/* j dimension */
	if (!isnan(block_vector->J)) {
		json_object_set_number(root_object, "J", block_vector->J);
	}

	/* k dimension */
	if (!isnan(block_vector->K)) {
		json_object_set_number(root_object, "K", block_vector->K);
	}

	/* r dimension */
	if (!isnan(block_vector->R)) {
		json_object_set_number(root_object, "R", block_vector->R);
	}

	/* q dimension */
	if (!isnan(block_vector->Q)) {
		json_object_set_number(root_object, "Q", block_vector->Q);
	}

	/* l parameter */
	if (block_vector->L != EMPTYCODE) {
		json_object_set_number(root_object, "L", block_vector->L);
	}

	/* p parameter */
	if (block_vector->P != EMPTYCODE) {
		json_object_set_number(root_object, "P", block_vector->P);
	}

	/* feed */
	if (!isnan(block_vector->F)) {
		json_object_set_number(root_object, "F", block_vector->F);
	}

	/* speed */
	if (!isnan(block_vector->S)) {
		json_object_set_number(root_object, "S", block_vector->S);
	}

	/* tool */
	if (block_vector->T != EMPTYCODE) {
		json_object_set_number(root_object, "T", block_vector->T);
	}

	/* print to the console */
	serialized_string = json_serialize_to_string_pretty(root_value);
	puts(serialized_string);

	/* free memory */
	json_free_serialized_string(serialized_string);
	json_value_free(root_value);
}

void print_machine_motion_mode(void)
{
	Machine_vector *machine_vector = Interpreter::getMachineVector();

	JSON_Value *root_value = json_value_init_object();
	JSON_Object *root_object = json_value_get_object(root_value);
	char *serialized_string = NULL;

	char *motion_mode[] = { "rapid", "linear", "cw", "ccw", "dwell" };

	/* print the motion mode */
	json_object_set_string(root_object, "motion mode", motion_mode[machine_vector->motion_mode]);

	/* print to the console */
	serialized_string = json_serialize_to_string_pretty(root_value);
	puts(serialized_string);

	json_free_serialized_string(serialized_string);
	json_value_free(root_value);
}

void arc_to_arcxy(axis *src, axis *dst)
{	
	switch (Interpreter::getModalVector()->modal2)
	{
    // if the XY plane
	case G17:
		dst->X = src->X;
		dst->Y = src->Y;
		dst->I = src->I;
		dst->J = src->J;
		break;
	case G18:
		dst->X = src->Z;
		dst->Y = src->X;
		dst->I = src->K;
		dst->J = src->I;
		break;
	case G19:
		dst->X = src->Y;
		dst->Y = src->Z;
		dst->I = src->J;
		dst->J = src->K;
		break;
	}
}

void arcxy_to_arc(axis *src, axis *dst)
{	
	switch (Interpreter::getModalVector()->modal2)
	{
		// if the XY plane
	case G17:
		dst->X = src->X;
		dst->Y = src->Y;
		dst->I = src->I;
		dst->J = src->J;
		break;
	case G18:
		dst->Z = src->X;
		dst->X = src->Y;
		dst->K = src->I;
		dst->I = src->J;
		break;
	case G19:
		dst->Y = src->X;
		dst->Z = src->Y;
		dst->J = src->I;
		dst->K = src->J;
		break;
	}
}







