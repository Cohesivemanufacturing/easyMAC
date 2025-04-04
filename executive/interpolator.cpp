#include "interpolator.h"

/**************************************************************
Static functions outside the class
***************************************************************/
// function prototypes
static inline int  parse_block(char* block, char code);
static inline int* parse_H_variable(char* block);

// function definitions
static inline int* parse_H_variable(char* block)
{
	static int temp[4] = { 0 };					    // All members of temp are init to zero
	temp[0] = parse_block(block, 'H');
	if (temp[0] != 0) {								// If we need to change any parameter
		if (temp[0] == 102) {					    // Specific for changing origin
			temp[1] = parse_block(block, 'X');
			temp[2] = parse_block(block, 'Y');
			temp[3] = parse_block(block, 'Z');
			return temp;
		}
		temp[1] = parse_block(block, '=');			// Change other parameter different from origin
	}
	return temp;
}

static inline int parse_block(char* block, char code)
{
	const char* cstr = block;

	while (cstr != NULL && *cstr != '\n')
	{
		if (*cstr == code) {        // if the char associated with the pointer pnt is equal to the code
			return atoi(cstr + 1);  // convert the string pointed by  ptr + 1 into a float and returned
		}
		cstr++;
	}
	return 0;                      // otherwise returns cero.
}

/**************************************************************
Interpolator object
***************************************************************/

// initialize static members
Interpolator    *Interpolator::interpolator = NULL;
Serial          *Interpolator::port = NULL;
volatile Buffer *Interpolator::buffer = NULL;
H_variables     *Interpolator::h_variables = NULL;
Machine_vector  *Interpolator::machine_vector = NULL;
axis			 Interpolator::position;
axis 			 Interpolator::target;

// initiliazize state variables
volatile bool   Interpolator::m_isEnabled = true;
volatile bool	Interpolator::m_isLogging = false;
volatile bool	Interpolator::m_isBurstGathering = false;
volatile bool	Interpolator::m_isBurstReceiving = false;

// Interpolator public services
Interpolator *Interpolator::initInterpolator(char *portName, int baud)
{
	// instanciates the executive object
	interpolator = new Interpolator();

	// initializes the serial port for the interpolator
	interpolator->port = new Serial(portName, baud);

	// initializes the circular buffers 
	interpolator->buffer = new Buffer();

	return interpolator;
}

Interpolator::~Interpolator()
{
	// deletes the serial port and the buffer
	delete port;
	delete buffer;
}

void Interpolator::startupSequence()
{
	// send a message to enable the interpolator
	std::string listen_cmd = "H600 = 1\n";
	port->write((char*)listen_cmd.c_str(), listen_cmd.length());  // send message

	// send a message to reset the interpolator
	listen_cmd = "H610\n";
	port->write((char*)listen_cmd.c_str(), listen_cmd.length());  // send message

	// 
	listen_cmd = "H612\n";
	port->write((char*)listen_cmd.c_str(), listen_cmd.length());  // send message
}

std::string Interpolator::straightFeed(Atomic_action *node)
{
	// extracts targets and positon registers from the atomic action

	int CB = 0;
	int IB = 0;
	int TB = 0;
	int x = node->position.X * get_machine_BLU();
	int y = node->position.Y * get_machine_BLU();
	int z = node->position.Z * get_machine_BLU();
	int px = abs(x);
	int py = abs(y);
	int pz = abs(z);
	int pf = calulate_feed_rate_number();
	int s = machine_vector->speed;

	// construct the message

	std::string data_out =
		"C" + std::to_string(CB) +
		"I" + std::to_string(IB) +
		"T" + std::to_string(TB) +
		"X" + std::to_string(x) +
		"Y" + std::to_string(y) +
		"Z" + std::to_string(z) +
		"P" + std::to_string(px) +
		"Q" + std::to_string(py) +
		"R" + std::to_string(pz) +
		"F" + std::to_string(pf) +
		"S" + std::to_string(s);

	int strlength = data_out.length() + std::to_string(data_out.length()).length() + 2; // 2 = 'L' + '/n'

	std::string eob =
		"L" + std::to_string(strlength) +
		'\n';

	return data_out + eob;
}

std::string Interpolator::arcFeed(Atomic_action *node)
{
	/*
	* this function convert the arc parameters to an XY-plane arc equivalent parameters
	* ans uses this to calulate the inverison bit the targets and correponding velocity
	* registers. Finally, the function maps the parameters back to their original plane
	*
	*/

	InterpolatorParameters parameters = InterpolatorParameters();
	InterpolatorParameters parameters_xy = InterpolatorParameters();

	axis arc = node->position;     // incremental vectors for the full arc
	axis arc_xy = axis();          // incremental vectors for the full equivalent xy arc

	// convert the arc targets to XY-arc equivalent targets 
	arc_to_arcxyblu(&arc, &arc_xy);

	parameters_xy.CB = 1;
	parameters_xy.pf = calulate_feed_rate_number();
	parameters_xy.s = machine_vector->speed;

	// Sets the values of the position and velocity registers in the plain
	switch (machine_vector->motion_mode)
	{
	case CW:
		if ((arc_xy.X > 0 && arc_xy.Y > 0) || (arc_xy.X < 0 && arc_xy.Y < 0))
		{
			parameters_xy.IB = 0;
			parameters_xy.x = arc_xy.X * get_machine_BLU(); // the x and y parameters are never inverted
			parameters_xy.y = arc_xy.Y * get_machine_BLU();

			// invert the targets
			parameters_xy.xtarget = abs(parameters_xy.x);
			parameters_xy.ytarget = abs(parameters_xy.y);
			parameters_xy.px = abs(arc_xy.J) * get_machine_BLU();
			parameters_xy.py = abs(arc_xy.I) * get_machine_BLU();
		}

		else
		{
			parameters_xy.IB = 1;
			parameters_xy.x = arc_xy.X * get_machine_BLU();
			parameters_xy.y = arc_xy.Y * get_machine_BLU();

			// invert the targets
			parameters_xy.xtarget = abs(parameters_xy.y);
			parameters_xy.ytarget = abs(parameters_xy.x);
			parameters_xy.px = abs(arc_xy.I) * get_machine_BLU();
			parameters_xy.py = abs(arc_xy.J) * get_machine_BLU();
		}

		break;
	case CCW:
		if ((arc_xy.X > 0 && arc_xy.Y > 0) || (arc_xy.X < 0 && arc_xy.Y < 0))
		{
			parameters_xy.IB = 1;
			parameters_xy.x = arc_xy.X * get_machine_BLU();
			parameters_xy.y = arc_xy.Y * get_machine_BLU();

			// invert the targets
			parameters_xy.xtarget = abs(parameters_xy.y);
			parameters_xy.ytarget = abs(parameters_xy.x);
			parameters_xy.px = abs(arc_xy.I) * get_machine_BLU();
			parameters_xy.py = abs(arc_xy.J) * get_machine_BLU();
		}

		else
		{
			parameters_xy.IB = 0;
			parameters_xy.x = arc_xy.X * get_machine_BLU();
			parameters_xy.y = arc_xy.Y * get_machine_BLU();

			// invert the targets
			parameters_xy.xtarget = abs(parameters_xy.x);
			parameters_xy.ytarget = abs(parameters_xy.y);
			parameters_xy.px = abs(arc_xy.J) * get_machine_BLU();
			parameters_xy.py = abs(arc_xy.I) * get_machine_BLU();
		}

		break;
	}

	// convert back to circular motion in any of the cannonical plains
	targetsxy_to_targets(&parameters_xy, &parameters);

	// construct the message

	std::string data_out =
		"C" + std::to_string(parameters.CB) +
		"I" + std::to_string(parameters.IB) +
		"T" + std::to_string(parameters.PB) +
		"X" + std::to_string(parameters.x) +
		"Y" + std::to_string(parameters.y) +
		"Z" + std::to_string(parameters.z) +
		"P" + std::to_string(parameters.px) +
		"Q" + std::to_string(parameters.py) +
		"R" + std::to_string(parameters.pz) +
		"F" + std::to_string(parameters.pf) +
		"S" + std::to_string(parameters.s);

	int strlength = data_out.length() + std::to_string(data_out.length()).length() + 2; // 2 = 'L' + '/n'

	std::string eob =
		"L" + std::to_string(strlength) +
		'\n';

	return data_out + eob;
}

void Interpolator::updateTarget() {
	target = position;
}

void Interpolator::updatePosition(Interpolatordata& data) {

	float BLU = get_machine_BLU();

	// updates the internal position 
	position.X = target.X + data.x / BLU;
	position.Y = target.Y + data.y / BLU;
	position.Z = target.Z + data.z / BLU;
}

bool Interpolator::parseFeedData(char* block, Interpolatordata& data) {

	static char temp[MAX_LEN];
	strcpy(temp, block);
	
	char* pch;

	pch = strtok(temp, " ,.");
	if (pch != NULL) {
		data.x = atoi(pch);
	}
	else {
		return false;
	}
	pch = strtok(NULL, " ,.");
	if (pch != NULL) {
		data.y = atoi(pch);
	}
	else {
		return false;
	}
	pch = strtok(NULL, " ,.");
	if (pch != NULL) {
		data.z = atoi(pch);
	}
	else {
		return false;
	}
	pch = strtok(NULL, " ,.");
	if (pch != NULL) {
		data.countin = atoi(pch);
	}
	else {
		return false;
	}
	pch = strtok(NULL, " ,.");
	if (pch != NULL) {
		data.countout = atoi(pch);
	}
	else {
		return false;
	}

	return true;
}

void Interpolator::parseBufferData(char* block, Bufferdata& data) {

	char* pch;

	data.error = false;

	pch = strtok(block, " ,.");
	if (pch != NULL) data.error = atoi(pch);
	pch = strtok(NULL, " ,.");
	if (pch != NULL) data.countin = atoi(pch);
}

axis Interpolator::getInterpolatorPosition(void)
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

	result.X = position.X - Xorigin - Xoffset;
	result.Y = position.Y - Yorigin - Yoffset;
	result.Z = position.Z - Zorigin - Zoffset;
	return result;
}

axis Interpolator::getInterpolatorNativePosition(void) {
	
	struct axis result;
	
	result.X = position.X;
	result.Y = position.Y;
	result.Z = position.Z;
	
	return result;
}

// Interpolator private services

int Interpolator::calulate_feed_rate_number(void)
{
	/*
	* calculas the feed rate number based on the maximun feed rate number
	* and the maximun interpolated frequency
	*/

	float frq_max = get_interpolator_maxfreq(); // maximun interpolated frequency
	int   M = get_interpolator_feedreg();      // size of the feed register
	int   frn_max = pow(2, M);                // maximun feed rate number	
	float feed_max = 0; // maximun linear velocity of the machine (units/min) 

	// if rapid mode
	if (machine_vector->motion_mode == RAPID)
	{
		return frn_max;
	}

	else
	{
		int feed = machine_vector->feed;

		if (machine_vector->feed_mode == UNITS_PER_MIN)
		{
			feed_max = (float)60 * frq_max / get_machine_BLU();
		}

		return (int)frn_max * (feed / feed_max);  // a proportion of the maximun frn
	}
}

void Interpolator::arc_to_arcxyblu(axis *src, axis *dst)
{
	switch (machine_vector->plane)
	{
		// if the XY plane
	case XY:
		dst->X = src->X;
		dst->Y = src->Y;
		dst->I = src->I;
		dst->J = src->J;
		break;
	case ZX:
		dst->X = src->Z;
		dst->Y = src->X;
		dst->I = src->K;
		dst->J = src->I;
		break;
	case YZ:
		dst->X = src->Y;
		dst->Y = src->Z;
		dst->I = src->J;
		dst->J = src->K;
		break;
	}
}

void Interpolator::targetsxy_to_targets(InterpolatorParameters *src, InterpolatorParameters *dst)
{
	dst->CB = src->CB;
	dst->pf = src->pf;
	dst->s  = src->s;
	dst->IB = src->IB;

	switch (machine_vector->plane)
	{
		// if the XY plane
	case XY:
		dst->PB = 1;
		dst->x = src->x;
		dst->y = src->y;
		dst->px = src->px;
		dst->py = src->py;
		break;
	case ZX:
		dst->PB = 2;
		dst->z = src->x;
		dst->x = src->y;
		dst->pz = src->px;
		dst->px = src->py;
		break;
	case YZ:
		dst->PB = 3;
		dst->y = src->x;
		dst->z = src->y;
		dst->py = src->px;
		dst->pz = src->py;
		break;
	}
}

float Interpolator::get_machine_BLU(void)
{
	if (machine_vector->units == MM) {
		return h_variables->H[BLU_MM];
	}
	else {
		return h_variables->H[BLU_IN];
	}
}

float Interpolator::get_interpolator_maxfreq(void)
{
	return h_variables->H[INTERPOLATOR_MFRQ];
}

float Interpolator::get_interpolator_feedreg(void)
{
	return h_variables->H[INTERPOLATOR_FREG];
}
