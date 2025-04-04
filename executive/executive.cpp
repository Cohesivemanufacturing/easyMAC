#include "executive.h"

/**************************************************************
Static functions outside the class
***************************************************************/

// function prototypes
static H_variables    *init_H_variables(void);
static Machine_vector *init_machine_vector(void);
static Modal_vector   *init_modal_vector(void);
static Block_vector   *init_block_vector(void);
static Atomic_action  *init_atomic_action(void);

// create serialize method from the atomic action object
static void	       file_append_action(char* filename, Atomic_action *node);
static inline char *serialize_error_message(const char* message, int num);
static inline char *serialize_aknowledge_message(const char* message, int num);
static inline void free_serialized_string(char* buffer);
static inline void file_concatenate(char* srcfile, char* destfile);
static inline void print_file(FILE* filepointer);
static inline std::string file2string(char* filename);

// function definitions
//part of the H_var object constructor 
static H_variables *init_H_variables(void)
{
	// alocates dynamic memory for the interpreter variables
	H_variables *new_variables = new H_variables;

	// initializes variables to zero
	*new_variables = H_variables();

	// set the default values of the interpreter

	// set the default values of the interpolator 
	new_variables->H[INTERPOLATOR_PORT] = EMPTYCODE;  // default COMP port
	new_variables->H[INTERPOLATOR_BAUD] = 115200;     // default baudrate
	new_variables->H[INTERPOLATOR_MFRQ] = 15000;      // default max. frequency
	new_variables->H[INTERPOLATOR_FREG] = 12;         // (2^12)-1 is the largest feed rate number

													  // set the default values of the axes

													  // set the default values of the machine

	return new_variables;
}
//part of the block_vector object constructor 
static Block_vector *init_block_vector(void)
{
	/* link the new block vector to the internal pointer intp_block*/
	Block_vector *new_block = new Block_vector;

	// initialize the G and M FIFO ciruclar queues 
	init_queue(&new_block->G);

	init_queue(&new_block->M);

	return new_block;
}
//part of the modal_vector object constructor 
static Modal_vector *init_modal_vector(void)
{
	/* link the new block vector to the internal pointer intp_block*/
	Modal_vector *new_modal = new Modal_vector;

	/* initilialize the non-modal FIFO circular queue*/
	init_queue(&new_modal->modal0);

	return new_modal;
}
//part of the machine vector object constructor 
static Machine_vector *init_machine_vector(void)
{
	/* create a new machine state vector*/
	Machine_vector *new_machine_state = new Machine_vector;

	*new_machine_state = Machine_vector();

	new_machine_state->feed = EMPTYCODE;

	return new_machine_state;
}
//part of the machine vector object constructor 
static Atomic_action *init_atomic_action(void) {

	// make a comment about how awesome init_atomic_actions function is.
	Atomic_action *new_action = new Atomic_action;
	*new_action = Atomic_action();

	new_action->next = NULL;

	return new_action;

	/*
	* alternatively:
	*
	* void init_atomic_action(atomic_action **action_in) {
	*
	* atomic_action *new_action = new atomic_action;
	* new_action->NEXT = NULL;
	* *action_in = new_action;  // the location in memory of a pointer defined the outside scope of a function cannot
	*							 // be globally modified by the function. However, the pointed value of a pointer
	*							 // can be.
	*
	*/
}

static void file_append_action(char* filename, Atomic_action *node)
{
	/*
	* References:
	* 1. Parson is a lighweight json library written in C.    https://github.com/kgabis/parson
	*/

	JSON_Value *root_value = json_value_init_object();
	JSON_Object *root_object = json_value_get_object(root_value);
	bool log_action = true;

	char *program_mode[] = { "close program", "load program" };
	char *motion_mode[] = { "rapid", "linear", "cw", "ccw", "dwell" };
	char *planes[] = { "xy", "xz", "yz" };
	char *distance_mode[] = { "abs", "rel" };
	char *feed_mode[] = { "inverse", "units per min", "units per rev" };
	char *units[] = { "in", "mm" };
	char *radius_comp[] = { "disable", "tool left" , "tool right" };
	char *length_comp[] = { "disable", "enable" };
	char *interpolator_stat[] = { "disable", "enable" };
	char *interpolator_logging[] = { "stop", "start" };
	char *interpolator_burst[] = { "stop", "start", "receive" };
	char *virtualmac_stat[] = { "disable", "enable" };
	char *coolant[] = { "disable", "enable" };

	json_object_set_string(root_object, "type", "atomic action");

	switch (node->action) {
	case SET_SEQ_NUMBER:
		json_object_set_string(root_object, "action", "set seq. number");
		json_object_set_number(root_object, "seq. number", node->sequence);
		break;

	case SET_FEED_MODE:
		json_object_set_string(root_object, "action", "set feed mode");
		json_object_set_string(root_object, "feed mode", feed_mode[node->feed_mode]);
		break;

	case SET_FEED_RATE:
		json_object_set_string(root_object, "action", "set feed rate");
		json_object_set_number(root_object, "feed rate", node->feed);
		break;

	case SET_SPINDLE_SPEED:
		json_object_set_string(root_object, "action", "set spindle speed");
		json_object_set_number(root_object, "spindle speed", node->speed);
		break;

	case SET_PLANE:
		json_object_set_string(root_object, "action", "set plane");
		json_object_set_string(root_object, "plane", planes[node->plane]);
		break;

	case SET_LENGTH_UNITS:
		json_object_set_string(root_object, "action", "set length units");
		json_object_set_string(root_object, "units", units[node->units]);
		break;

	case SET_RADIUS_COMP:
		json_object_set_string(root_object, "action", "set radius compensation");
		json_object_set_string(root_object, "compensation mode", radius_comp[node->radius_comp]);
		break;

	case SET_LENGTH_COMP:
		json_object_set_string(root_object, "action", "set length compensation");
		json_object_set_string(root_object, "compensation mode", length_comp[node->length_comp]);
		break;

	case SET_COORD_SYSTEM:
		json_object_set_string(root_object, "action", "set coordinate system");
		json_object_set_number(root_object, "coordinate system", node->coordinate_system);
		break;

	case SET_DISTANCE_MODE:
		json_object_set_string(root_object, "action", "set distance mode");
		json_object_set_string(root_object, "distance mode", distance_mode[node->distance_mode]);
		break;

	case SET_ORIGIN_OFFSET:
		json_object_set_string(root_object, "action", "set origin offset");
		json_object_dotset_number(root_object, "origin.x", node->origin_offset.X);
		json_object_dotset_number(root_object, "origin.y", node->origin_offset.Y);
		json_object_dotset_number(root_object, "origin.z", node->origin_offset.Z);
		break;

	case SET_MOTION_MODE:
		json_object_set_string(root_object, "action", "set motion mode");
		json_object_set_string(root_object, "motion mode", motion_mode[node->distance_mode]);
		break;

	case STRAIGHT_FEED:
		json_object_set_string(root_object, "action", "straight feed");
		json_object_dotset_number(root_object, "distance.x", node->position.X);
		json_object_dotset_number(root_object, "distance.y", node->position.Y);
		json_object_dotset_number(root_object, "distance.z", node->position.Z);
		break;

	case ARC_FEED:
		json_object_set_string(root_object, "action", "arc feed");
		json_object_dotset_number(root_object, "distance.x", node->position.X);
		json_object_dotset_number(root_object, "distance.y", node->position.Y);
		json_object_dotset_number(root_object, "distance.z", node->position.Z);
		json_object_dotset_number(root_object, "distance.i", node->position.I);
		json_object_dotset_number(root_object, "distance.j", node->position.J);
		json_object_dotset_number(root_object, "distance.k", node->position.K);
		break;

	case SET_H_VARIABLE:
		json_object_set_string(root_object, "action", "set interp. var");
		json_object_set_number(root_object, "variable number", node->variable);
		json_object_set_number(root_object, "value", node->value);
		break;

	case SET_BLU_IN:
		json_object_set_string(root_object, "action", "set BLU in");
		json_object_set_number(root_object, "value", node->value);
		break;

	case SET_BLU_MM:
		json_object_set_string(root_object, "action", "set BLU mm");
		json_object_set_number(root_object, "value", node->value);
		break;

	case SET_ARC_TOL:
		json_object_set_string(root_object, "action", "set arc tol");
		json_object_set_number(root_object, "value", node->value);
		break;

	case SET_INTERPO_STAT:
		json_object_set_string(root_object, "action", "set interpolator status");
		json_object_set_string(root_object, "status", interpolator_stat[node->mode]);
		break;

	case SET_INTERPO_PORT:
		json_object_set_string(root_object, "action", "set interpolator COM port");
		json_object_set_number(root_object, "port COM", node->value);
		break;

	case SET_INTERPO_BAUD:
		json_object_set_string(root_object, "action", "set interpolator baudrate");
		json_object_set_number(root_object, "baud", node->value);
		break;

	case SET_INTERPO_LOGGING:
		json_object_set_string(root_object, "action", "set interpolator logging communications");
		json_object_set_string(root_object, "motion mode", interpolator_logging[node->mode]);
		break;

	case SET_INTERPO_BURST:
		json_object_set_string(root_object, "action", "set interpolator logging communications");
		json_object_set_string(root_object, "motion mode", interpolator_burst[node->mode]);
		break;

	case SET_VIRTMAC_STAT:
		json_object_set_string(root_object, "action", "set virtual machine status");
		json_object_set_string(root_object, "status", virtualmac_stat[node->mode]);
		break;

	default:
		log_action = false;
		break;
	}

	// log the atomic action
	FILE *fp = NULL;
	char *serialized_string = NULL;

	if (log_action) {
		fp = fopen(filename, "a+");
		serialized_string = json_serialize_to_string_pretty(root_value);
		fprintf(fp, "%s\n", serialized_string);
		fclose(fp);
	}

	// free memory
	json_free_serialized_string(serialized_string);
	json_value_free(root_value);
}

static char *serialize_error_message(const char* message, int num)
{
	JSON_Value *root_value = json_value_init_object();
	JSON_Object *root_object = json_value_get_object(root_value);
	char *serialized_string = NULL;

	// type 
	json_object_set_string(root_object, "type", "error_message");

	// message 
	json_object_set_string(root_object, "message", message);

	// message number
	json_object_set_number(root_object, "number", num);

	// serialize string
	serialized_string = json_serialize_to_string_pretty(root_value);

	// free root value
	json_value_free(root_value);

	return serialized_string;
}

static char *serialize_aknowledge_message(const char* message, int num)
{
	JSON_Value *root_value = json_value_init_object();
	JSON_Object *root_object = json_value_get_object(root_value);
	char *serialized_string = NULL;

	// type 
	json_object_set_string(root_object, "type", "aknowledge_message");

	// message
	json_object_set_string(root_object, "message", message);

	// message number
	json_object_set_number(root_object, "number", num);

	// serialize string
	serialized_string = json_serialize_to_string_pretty(root_value);

	// free root value
	json_value_free(root_value);

	return serialized_string;
}

static inline void free_serialized_string(char* buffer)
{
	delete buffer;
}

static inline void file_concatenate(char* srcfile, char* destfile)
{
	FILE *src = fopen(srcfile, "r+");
	FILE *dst = fopen(srcfile, "a+");

	int c;

	if (src) {
		while ((c = getc(src)) != EOF)
			fputc(c, dst);
	}
}

static inline void print_file(FILE *filepointer)
{
	int c;

	rewind(filepointer);

	printf("\n");

	if (filepointer) {
		while ((c = getc(filepointer)) != EOF)
			putchar(c);
	}
}

static inline std::string file2string(char* filename) {
	std::ifstream t(filename);
	std::stringstream buffer;
	buffer << t.rdbuf();
	return buffer.str();
}

/**************************************************************
Executive object
***************************************************************/

// initialize executive static members
Executive	   *Executive::executive = NULL;
Interpreter    *Executive::interpreter = NULL;
H_variables    *Executive::h_variables = NULL;
Block_vector   *Executive::block_vector = NULL;
Modal_vector   *Executive::modal_vector = NULL;
Machine_vector *Executive::machine_vector = NULL;
Atomic_action  *Executive::atomic_action_in = NULL;
Atomic_action  *Executive::atomic_action_out = NULL;
FILE           *Executive::history_file = NULL;
Exeparser      *Executive::exeparser = NULL;
Interpolator   *Executive::interpolator = NULL;
VirtualMachine *Executive::virtualMachine = NULL;
Winsocket      *Executive::winsocket = NULL;
VirtualMachine *Executive::cloudNCVM = NULL;
Winsocket      *Executive::cloudNCsocket = NULL;
char           *Executive::historyFileName = NULL;
char           *Executive::actionsFileName = NULL;
char           *Executive::rootFolderName = NULL;
char           *Executive::programFileName = NULL;

// initialize thread state variables
volatile bool   Executive::m_isEnabled = true;
volatile bool   Executive::m_logActions = false;
volatile bool   Executive::m_logHistory = false;
volatile bool   Executive::m_Estop = false;
volatile bool   Executive::m_startProgram = false;
volatile bool   Executive::m_exitProgram = false;
volatile bool   Executive::m_parsingProgram = false;
volatile bool   Executive::m_pausedProgram = false;
volatile bool   Executive::m_startInterpolator = false;
volatile bool   Executive::m_exitInterpolator = false;
volatile bool   Executive::m_interpolating = false;
volatile bool   Executive::m_startVirtualMachine = false;
volatile bool   Executive::m_exitVirtualMachine = false;
volatile bool   Executive::m_virtualMachining = false;
volatile bool   Executive::m_startCloudNC = false;
volatile bool   Executive::m_exitCloudNC = false;
volatile bool   Executive::m_cloudNCStreaming = false;
volatile bool   Executive::m_isWinSocketStarted = false;
volatile bool   Executive::m_isCLNCSocketStarted = false;

// Executive services

Executive* Executive::initExecutive(void)
{
	// instanciates the executive object
	executive = new Executive();

	// initialize the executive structures
	h_variables = init_H_variables();
	block_vector = init_block_vector();
	modal_vector = init_modal_vector();
	machine_vector = init_machine_vector();
	atomic_action_in = init_atomic_action();
	atomic_action_out = atomic_action_in;

	// bind executive parser
	exeparser = Exeparser::initExeparser
	(
		h_variables,
		modal_vector,
		machine_vector,
		&atomic_action_in,
		&atomic_action_out
	);

	// binds the intepreter 
	interpreter = Interpreter::initInterpreter
	(
		h_variables,
		block_vector,
		modal_vector,
		machine_vector,
		&atomic_action_in,
		&atomic_action_out
	);

	// set the interpreter options
	Interpreter::setInput(stdin);
	Interpreter::enableChecks();

	// links the interpolator object to the executive structures
	Interpolator::setHvariables(h_variables);
	Interpolator::setMachineVector(machine_vector);

	return executive;
}

Executive::~Executive()
{
	// free executive variables 
	delete[] h_variables, block_vector, modal_vector, machine_vector;

	// erase the atomic actions
	while (atomic_action_out != atomic_action_in)
		atomic_action_out = delete_action(atomic_action_out);

	// free executive parser object
	delete[] exeparser;

	// free interpreter object
	delete[] interpreter;

	// free the history file object 
	if (m_logHistory)
		fclose(history_file);
}

int Executive::loadConfiguration(char* fileName)
{
	// open program 
	FILE* program_file = fopen(fileName, "r");

	Exeparser::resetLineNumber();

	char block[256];

	// make sure it is valid :
	if (!program_file)
	{
		errorHandler(ERR_CONFIGFILE);
		return 0;
	}

	if (checkProgram()) // if no sintax errors
	{
		while (fgets(block, sizeof(block), program_file))
		{
			// parse the block
			parseBlock(block);

			// log actions
			logAtomicActions();

			// handle actions
			actionHandler();
		}
	}

	else
	{
		// to executive error handler;
		return 0;
	}

	return 1;
}

Interpolator* Executive::initInterpolator(void)
{
	// get serial port variables
	int portnb = getHvariable(INTERPOLATOR_PORT); // COM port
	std::string portName = "\\\\.\\COM" + std::to_string(portnb); 

	int baud = getHvariable(INTERPOLATOR_BAUD);   // baudrate

	// start the interpolator object
	interpolator = Interpolator::initInterpolator((char*)portName.c_str(), baud);

	// link the interpolator to the machine_vector
	Interpolator::setMachineVector(machine_vector);

	return interpolator;
}

VirtualMachine* Executive::initVirtualMachine(const std::string& fileName, Display* display, Shader* shader, Camara* camara)
{
	// start the virtual machine object
	virtualMachine = new VirtualMachine(fileName, display, shader, camara);

	// link the virtual machine to the machine_vector
	virtualMachine->setMachineVector(machine_vector);

	// link the virtual machine to the h_variables
	virtualMachine->setHvariables(h_variables);

	return virtualMachine;
}

VirtualMachine* Executive::initCloudNC(void)
{
	// start the virtual machine object
	cloudNCVM = new VirtualMachine(cloudNCsocket);

	// link the virtual machine to the machine_vector
	cloudNCVM->setMachineVector(machine_vector);

	// link the virtual machine to the h_variables
	cloudNCVM->setHvariables(h_variables);

	return cloudNCVM;
}

Winsocket* Executive::initWinSocket(int port)
{
	// creates a windows listening socket at "port"
	winsocket = new Winsocket(port);

	if (winsocket->isCreated()) {
		messageHandler(MSG_CLNCSOCKETCREATED);
	}
	else {
		errorHandler(ERR_SOCKETCREATED);
	}

	return winsocket;
}

Winsocket* Executive::initCloudNCSocket(int port)
{
	// creates a windows listening socket at "port"
	cloudNCsocket = new Winsocket(port);

	if (cloudNCsocket->isCreated()) {
		messageHandler(MSG_SOCKETCREATED);
	}
	else {
		errorHandler(ERR_SOCKETCREATED);
	}

	return cloudNCsocket;
}

char* Executive::serialize_machine_vector(void)
{
	/*
	* Modification
	* by Jorge Correa 8-4-2017
	* 1. machine vector has space in memory for the interpolator and target
	* positions in native coordinates
	* 2. created methods getTargetPosition() and getPosition() to obtain the
	* interpolator and target positions
	* w.r.t the current coordinate systems
	*/
	
	JSON_Value *root_value = json_value_init_object();
	JSON_Object *root_object = json_value_get_object(root_value);
	char *serialized_string = NULL;

	char *feed_mode[] = { "inverse", "units per min", "units per rev" };
	char *planes[] = { "xy", "xz", "yz" };
	char *units[] = { "in", "mm" };
	char *radius_comp[] = { "off", "tool left" , "tool right" };
	char *length_comp[] = { "disable", "enable" };
	char *distance_mode[] = { "abs", "rel" };
	char *motion_mode[] = { "rapid", "linear", "cw", "ccw", "dwell" };
	char *coolant[] = { "on", "off" };

	axis target_position = Interpreter::getTargetPosition();
	axis native_position = axis();
	axis position = axis();

	// if inside virtual interpolating thread
	if (isInterpolating())
	{
		native_position = Interpolator::getInterpolatorNativePosition();
		position        = Interpolator::getInterpolatorPosition();
	}
	// if inside virtual machine thread
	else if (isVirtualMachining())
	{
		native_position = virtualMachine->getInterpolatorNativePosition();
		position = virtualMachine->getInterpolatorPosition();
	}
	// if inside cloudNC thread
	else if (isCloudNCStreaming())
	{
		native_position = cloudNCVM->getInterpolatorNativePosition();
		position = cloudNCVM->getInterpolatorPosition();
	}

	json_object_set_string(root_object, "type", "machine_vector");

	/* print sequence number */
	json_object_set_number(root_object, "sequence_number", machine_vector->sequence);

	/* axes */
	/* axis 1 */
	json_object_dotset_string(root_object, "axes.axis_1.name", "X");
	json_object_dotset_string(root_object, "axes.axis_1.status", "enabled");
	json_object_dotset_number(root_object, "axes.axis_1.native_position", native_position.X);
	json_object_dotset_number(root_object, "axes.axis_1.position", position.X);
	json_object_dotset_number(root_object, "axes.axis_1.target_position", target_position.X);

	/* axis 1 */
	json_object_dotset_string(root_object, "axes.axis_2.name", "Y");
	json_object_dotset_string(root_object, "axes.axis_2.status", "enabled");
	json_object_dotset_number(root_object, "axes.axis_2.native_position", native_position.Y);
	json_object_dotset_number(root_object, "axes.axis_2.position", position.Y);
	json_object_dotset_number(root_object, "axes.axis_2.target_position", target_position.Y);

	/* axis 3 */
	json_object_dotset_string(root_object, "axes.axis_3.name", "Z");
	json_object_dotset_string(root_object, "axes.axis_3.status", "enabled");
	json_object_dotset_number(root_object, "axes.axis_3.native_position", native_position.Z);
	json_object_dotset_number(root_object, "axes.axis_3.position", position.Z);
	json_object_dotset_number(root_object, "axes.axis_3.target_position", target_position.Z);

	/* print feed rate  */
	json_object_set_number(root_object, "feed_rate", machine_vector->feed);

	/* print speed parameter */
	json_object_set_number(root_object, "speed", machine_vector->speed);

	/* print tool number */
	json_object_set_number(root_object, "tool_number", machine_vector->tool);

	/* print tool offset */
	json_object_dotset_number(root_object, "tool_offset.x", machine_vector->tool_offset.X);
	json_object_dotset_number(root_object, "tool_offset.y", machine_vector->tool_offset.Y);
	json_object_dotset_number(root_object, "tool_offset.z", machine_vector->tool_offset.Z);

	/* print tool radius */
	json_object_set_number(root_object, "tool_radius", machine_vector->tool_radius);

	/* print coolant parameter */
	json_object_set_string(root_object, "coolant", coolant[machine_vector->coolant]);

	/* print the motion mode */
	json_object_set_string(root_object, "motion_mode", motion_mode[machine_vector->motion_mode]);

	/* print active plane */
	json_object_set_string(root_object, "active_plane", planes[machine_vector->plane]);

	/* print distance mode */
	json_object_set_string(root_object, "distance_mode", distance_mode[machine_vector->distance_mode]);

	/* print feed rate mode */
	json_object_set_string(root_object, "feed_mode", feed_mode[machine_vector->feed_mode]);

	/* print length units */
	json_object_set_string(root_object, "units", units[machine_vector->units]);

	/* print tool radius compensation */
	json_object_set_string(root_object, "cutter_radius_compensation", radius_comp[machine_vector->radius_comp]);

	/* print tool lenght compensation */
	json_object_set_string(root_object, "cutter_length_compensation", length_comp[machine_vector->length_comp]);

	/* print current coorinate system */
	json_object_set_number(root_object, "coordinate_system", machine_vector->coordinate_system);

	/* serialize string*/
	serialized_string = json_serialize_to_string_pretty(root_value);

	/* free memory */
	json_value_free(root_value);

	return serialized_string;
}

void Executive::parseBlock(char* block)
{
	// parses block
	Exeparser::scanString(block);
	Exeparser::parse();
	Exeparser::deleteBuffer();
}

void Executive::interpretBlock(char* block)
{
	// interprets block
	Interpreter::scanString(block);
	Interpreter::parse();
	Interpreter::deleteBuffer();
}

int Executive::checkProgram()
{
	return true;
}

void Executive::actionHandler(void)
{
	while (atomic_action_out != atomic_action_in)
	{
		// handle atomic action
		actionHandler(atomic_action_out);

		// the action handler 
		if (atomic_action_out == atomic_action_in)
			break;

		// deletes and get the following action
		atomic_action_out = delete_action(atomic_action_out);
	}
}

void Executive::logAtomicActions(void)
{
	// a pointer to the head and tail actions
	Atomic_action *head = atomic_action_in;
	Atomic_action *tail = atomic_action_out;
	bool is_cleared = false;

	if (m_logActions)
	{
		// clear atomic actions file
		if (tail->action != PRINT_ACTION)
			fclose(fopen(actionsFileName, "w+"));

		// log atomic actions
		while (tail != head)
		{
			// serialize atomic action
			file_append_action(actionsFileName, tail);

			// log next action
			tail = tail->next;
		}

		// concatenate the actions_file to the history_file
		if (m_logHistory)
			file_concatenate(actionsFileName, historyFileName);
	}
}

void Executive::actionHandler(Atomic_action* node)
{
	switch (node->action)
	{
	case LOAD_PROGRAM_FILE:
		loadProgramFileAction(node);
		break;
	case LOAD_CONFIG_FILE:
		loadConfigFileAction(node);
		break;
	case SET_ACTIONS_LOG:
		setActionsLogAction(node);
		break;
	case SET_HISTORY_LOG:
		setHistoryLogAction(node);
		break;
	case SET_PROGRAM:
		setProgramAction(node);
		break;
	case SET_SEQ_NUMBER:
		setSeqNumberAction(node);
		break;
	case SET_FEED_MODE:
		setFeedModeAction(node);
		break;
	case SET_FEED_RATE:
		setFeedRateAction(node);
		break;
	case SET_SPINDLE_SPEED:
		setSpindleSpeedAction(node);
		break;
	case SET_PLANE:
		setPlaneAction(node);
		break;
	case SET_LENGTH_UNITS:
		setLengthUnitsAction(node);
		break;
	case SET_RADIUS_COMP:
		setRadiusCompensationAction(node);
		break;
	case SET_LENGTH_COMP:
		setLengthCompensationAction(node);
		break;
	case SET_RETRACTION_MODE:
		setRetractionModeAction(node);
		break;
	case SET_COORD_SYSTEM:
		setCoordinateSytemAction(node);
		break;
	case SET_PATH_MODE:
		setPathModeAction(node);
		break;
	case SET_DISTANCE_MODE:
		setDistanceModeAction(node);
		break;
	case SET_ORIGIN_OFFSET:
		setOriginOffsetsAction(node);
		break;
	case SET_MOTION_MODE:
		setMotionModeAction(node);
		break;
	case STRAIGHT_FEED:
		straightFeedAction(node);
		break;
	case ARC_FEED:
		arcFeedAction(node);
		break;
	case SET_H_VARIABLE:
		setHvariableAction(node);
		break;
	case SET_BLU_IN:
		setBLUinAction(node);
		break;
	case SET_BLU_MM:
		setBLUmmAction(node);
		break;
	case SET_ARC_TOL:
		setArcTolAction(node);
		break;
	case SET_INTERPO_STAT:
		setInterpoStatAction(node);
		break;
	case SET_INTERPO_PORT:
		setInterpoPortAction(node);
		break;
	case SET_INTERPO_BAUD:
		setInterpoBaudAction(node);
		break;
	case SET_INTERPO_MAXFRQ:
		setInterpoMaxFrqAction(node);
		break;
	case SET_INTERPO_LOGGING:
		setInterpoLoggingAction(node);
		break;
	case SET_INTERPO_BURST:
		setInterpoLoggingAction(node);
		break;
	case SET_VIRTMAC_STAT:
		setVirtualMacStatAction(node);
		break;
	case SET_CLOUDNC_STAT:
		setCloudNCStatAction(node);
		break;
	case PRINT_ACTION:
		printAction(node);
		break;
	case INTERPRETER_ERROR:
		interpreterErrorAction(node);
		break;
	case EXEPARSER_ERROR:
		exeparserErrorAction(node);
		break;
	case SET_EXECUTIVE_STAT:
		setExecutiveStatAction(node);
		break;
	default:
		break;
	}
}

void Executive::messageHandler(int num)
{
	// start attributes
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	WORD saved_attributes;

	// Save current attributes 
	GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
	saved_attributes = consoleInfo.wAttributes;

	// set console attributes
	SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN); // yellow messages

	int portnb = getHvariable(INTERPOLATOR_PORT);
	std::string message;

	switch (num)
	{
	case MSG_INICONFIG:
		message = "The executive has loaded the initial configuration";
		break;
	case MSG_PROGCHECK:
		message = "program check has exited without errors";
		break;
	case MSG_STARTPROG:
		message = "The executive has started the program file thread";
		break;
	case MSG_EXITPROG:
		message = "The executive has exited the program file thread";
		break;
	case MSG_OPENPROG:
		message = "The executive has open the program file";
		break;
	case MSG_STARTINTP:
		message = "The executive has started the interpolator thread";
		break;
	case MSG_EXITINTP:
		message = "The executive has ended the interpolator thread";
		break;
	case MSG_CONINTP:
		message = "The executive has connected with the interpolator: COM port " + std::to_string(portnb);
		break;
	case MSG_CONSOCKET:
		message = "Socket Adapter: New client got connected, ready to receive and send data";
		break;
	case MSG_SOCKETCREATED:
		message = "The executive has opened the socket adapter for inter-process communications";
		break;
	case MSG_CONCLNCSOCKET:
		message = "cloudNC Adapter: New client got connected, ready to stream data";
		break;
	case MSG_CLNCSOCKETCREATED:
		message = "The executive has opened the socket adapter for cloudNC stream communications";
		break;
	case MSG_STARTVIRTMAC:
		message = "The executive has started the virtual machine thread";
		break;
	case MSG_EXITVIRTMAC:
		message = "The executive has ended the virtual machine thread";
		break;
	case MSG_STARTCLOUDNC:
		message = "The executive has started the cloudnc thread";
		break;
	case MSG_EXITCLOUDNC:
		message = "The executive has ended the cloudnc thread";
		break;
	}

	//print to the console
	std::cout << message;
	SetConsoleTextAttribute(hConsole, saved_attributes); // reset console atributes
	std::cout << "\n\n>";

	// write to the socket
	if (winsocket != NULL) {
		if (winsocket->isConnected())
		{
			// serialized to JSON
			char* JSON_message = serialize_aknowledge_message(message.c_str(), num);
			winsocket->write(std::string(JSON_message));

			// free memory
			json_free_serialized_string(JSON_message);
		}
	}
}

void Executive::errorHandler(int num)
{
	// start attributes
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	WORD saved_attributes;

	// Save current attributes 
	GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
	saved_attributes = consoleInfo.wAttributes;

	// set console attributes
	//SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY | FOREGROUND_RED); // yellow messages

	// initiaize variables and allocate memory for the message 
	int portnb = getHvariable(INTERPOLATOR_PORT);  // comport number

	// allocate memory for the message 
	std::string header = "Error: Executive " + std::to_string(num) + ": ";
	std::string body;

	// body
	switch (num)
	{
	case ERR_CONFIGFILE:
		body = "failed to find the configuration file";
		break;
	case ERR_INICONFIG:
		body = "failed to load machine initial configuration";
		break;
	case ERR_PROGCHECK:
		body = "program check has exited with errors";
		break;
	case ERR_STARTPROG:
		body = "failed to start the program file thread";
		break;
	case ERR_EXITPROG:
		body = "failed to exit program file thread";
		break;
	case ERR_OPENPROG:
		body = "failed to open program file";
		break;
	case ERR_STARTINTP:
		body = "failed to start the interpolator thread";
		break;
	case ERR_EXITINTP:
		body = "failed to exit the interpolator thread";
		break;
	case ERR_MULTINTP:
		body = "disable the interpolator thread before starting a new thread";
		break;
	case ERR_CONINTP:
		body = "failed to connect with the interpolator: COM port " + std::to_string(portnb);
		break;
	case ERR_DISCSOCKET:
		body = "Socket Adapter: client got disconnected";
		break;
	case ERR_SOCKETCREATED:
		body = "failed to create socket connection for inter-process communications";
		break;
	case ERR_DISCCLNCSOCKET:
		body = "cloudNC Socket Adapter: client got disconnected";
		break;
	case ERR_CLNCSOCKETCREATED:
		body = "failed to create socket connection for cloudNC stream communications";
		break;
	case ERR_COMPORTAV:
		body = "COM port " + std::to_string(portnb) + " not available";
		break;
	case ERR_STARTVIRTMAC:
		body = "failed to start the virtual machine thread";
		break;
	case ERR_EXITVIRTMAC:
		body = "failed to exit the virtual machine thread";
		break;
	case ERR_MULTVIRTMAC:
		body = "disable the active virtual machine before starting a new thread";
		break;
	case ERR_STARTCLOUDNC:
		body = "disable the active virtual machine before starting a new thread";
		break;
	case ERR_EXITCLOUDNC:
		body = "failed to exit the virtual machine thread";
		break;
	case ERR_MULTCLOUDNC:
		body = "disable the active virtual machine before starting a new thread";
		break;
	}

	// concatenate message
	std::string message = header + body;

	//print to the console
	std::cout << message;
	SetConsoleTextAttribute(hConsole, saved_attributes); // reset console atributes
	std::cout << "\n\n>";

	// write to the socket
	if (winsocket->isConnected())
	{
		// serialized to JSON
		char* JSON_message = serialize_error_message(message.c_str(), num);
		winsocket->write(std::string(JSON_message));

		// free memory
		json_free_serialized_string(JSON_message);
	}
}

// atomic action handlers

void Executive::loadProgramFileAction(Atomic_action* node)
{
	// concatenate root folder directory with filename
	static char filename[64];
	strcpy(filename, rootFolderName);
	strcat(filename, node->filename);

	// if not parsing a program
	if (!isProgramParsing())
	{
		// store program name
		programFileName = filename;

		// start parsing program thread
		startProgram();
	}
	// if parsing a program
	else
	{
		// error handler
	}
}

void Executive::loadConfigFileAction(Atomic_action* node)
{

	// root folder directory with filename
	static char filename[64];
	strcpy(filename, "./res/executive/config/");
	strcat(filename, node->filename);

	// clear the loadConfigurationfile action (avoid infinite loop)
	atomic_action_out = delete_action(atomic_action_out);

	// parse configuration file
	loadConfiguration(filename);
}

void Executive::setActionsLogAction(Atomic_action* node)
{
	switch (node->mode)
	{
	case ACTION_LOG_ENABLE:
		enableActionLog();
		break;
	case ACTION_LOG_DISABLE:
		disableActionLog();
		break;
	}
}

void Executive::setHistoryLogAction(Atomic_action* node)
{
	switch (node->mode)
	{
	case HISTORY_LOG_ENABLE:
		enableHistoryLog();
		break;
	case ACTION_LOG_DISABLE:
		disableHistoryLog();
		break;
	}
}

void Executive::setProgramAction(Atomic_action* node)
{
	switch (node->mode)
	{
	case PROGRAM_PAUSE:
		pauseProgram();
		break;
	case PROGRAM_STOP:
		stopProgram();
		break;
	case PROGRAM_CONTINUE:
		continueProgram();
		break;
	}
}

void Executive::setSeqNumberAction(Atomic_action* node)
{
	machine_vector->sequence = node->sequence;
}

void Executive::setFeedModeAction(Atomic_action* node)
{
	switch (node->feed_mode)
	{
	case INVERSE_TIME:
		machine_vector->feed_mode = node->feed_mode;
		break;
	case UNITS_PER_MIN:
		machine_vector->feed_mode = node->feed_mode;
		break;
	case UNITS_PER_REV:
		machine_vector->feed_mode = node->feed_mode;
		break;
	default:
		break;
	}
}

void Executive::setFeedRateAction(Atomic_action *node)
{
	machine_vector->feed = node->feed;
}

void Executive::setSpindleSpeedAction(Atomic_action *node)
{
	machine_vector->speed = node->speed;
}

void Executive::setPlaneAction(Atomic_action* node)
{
	switch (node->plane)
	{
	case XY:
		machine_vector->plane = node->plane;
		break;
	case ZX:
		machine_vector->plane = node->plane;
		break;
	case YZ:
		machine_vector->plane = node->plane;
		break;
	default:
		break;
	}
}

void Executive::setLengthUnitsAction(Atomic_action* node)
{
	switch (node->units)
	{
	case IN:
		machine_vector->units = node->units;
		break;
	case MM:
		machine_vector->units = node->units;
		break;
	default:
		break;
	}
}

void Executive::setRadiusCompensationAction(Atomic_action* node)
{
	switch (node->radius_comp)
	{
	case RCOMP_DISABLE:
		machine_vector->radius_comp = node->radius_comp;
		break;
	case TOOL_LEFT:
		machine_vector->radius_comp = node->radius_comp;
		break;
	case TOOL_RIGHT:
		machine_vector->radius_comp = node->radius_comp;
		break;
	default:
		break;
	}
}

void Executive::setLengthCompensationAction(Atomic_action* node)
{
	switch (node->length_comp)
	{
	case LCOMP_ENABLE:
		machine_vector->length_comp = node->length_comp;
		break;
	case LCOMP_DISABLE:
		machine_vector->length_comp = node->length_comp;
		break;
	default:
		break;
	}
}

void Executive::setRetractionModeAction(Atomic_action* node)
{
	switch (node->retraction_mode)
	{
	case ORIGIN:
		machine_vector->retraction_mode = node->retraction_mode;
		break;
	case SPECIFIED:
		machine_vector->retraction_mode = node->retraction_mode;
		break;
	default:
		break;
	}
}

void Executive::setCoordinateSytemAction(Atomic_action* node)
{
	machine_vector->coordinate_system = node->coordinate_system;
}

void Executive::setPathModeAction(Atomic_action* node)
{
	switch (node->path_mode)
	{
	case EXACT:
		machine_vector->path_mode = node->path_mode;
		break;
	case STOP:
		machine_vector->path_mode = node->path_mode;
		break;
	case BLENDING:
		machine_vector->path_mode = node->path_mode;
		break;
	default:
		break;
	}
}

void Executive::setDistanceModeAction(Atomic_action* node)
{
	switch (node->distance_mode)
	{
	case ABS:
		machine_vector->distance_mode = node->distance_mode;
		break;
	case REL:
		machine_vector->distance_mode = node->distance_mode;;
		break;
	default:
		break;
	}
}

void Executive::setOriginOffsetsAction(Atomic_action* node)
{
	h_variables->H[ORGIN_OFFSET_X] = node->origin_offset.X;
	h_variables->H[ORGIN_OFFSET_Y] = node->origin_offset.Y;
	h_variables->H[ORGIN_OFFSET_Z] = node->origin_offset.Z;
}

void Executive::setMotionModeAction(Atomic_action* node)
{
	machine_vector->motion_mode = node->motion_mode;
}

void Executive::straightFeedAction(Atomic_action* node)
{
	/*
	* compile message to inteprolator packages format
	*/
	std::string data_out = Interpolator::straightFeed(node);

	// if connected to the interpolator
	if (isInterpolating())
	{
		// try to send the message
		Interpolator::port->write((char*)data_out.c_str(), data_out.length());
		// keeps in the FIFO buffer
		Interpolator::buffer->write((char*)data_out.c_str());
	}

	// if connected to the virtual machine
	if (isVirtualMachining())
	{
		// send message
		virtualMachine->buffer->write((char*)data_out.c_str());
	}

	// if connected to cloudNC
	if (isCloudNCStreaming())
	{
		// send message
		cloudNCVM->buffer->write((char*)data_out.c_str());
	}

	// updates the machine state vector
	updateTargetPositionAction(node);

}

void Executive::arcFeedAction(Atomic_action* node)
{
	/*
	* compile message to inteprolator packages format
	*/
	std::string data_out = Interpolator::arcFeed(node);

	// if connected to the interpolator
	if (isInterpolating())
	{
		// try to send the message
		Interpolator::port->write((char*)data_out.c_str(), data_out.length());
		// keeps in the FIFO buffer
		Interpolator::buffer->write((char*)data_out.c_str());
	}

	// if connected to the virtual machine
	if (isVirtualMachining())
	{
		// send message
		virtualMachine->buffer->write((char*)data_out.c_str());
	}

	// if connected to cloudNC
	if (isCloudNCStreaming())
	{
		// send message
		cloudNCVM->buffer->write((char*)data_out.c_str());
	}

	// updates the machine state vector
	updateTargetPositionAction(node);
}

void Executive::setHvariableAction(Atomic_action* node)
{
	h_variables->H[node->variable] = node->value;
}

void Executive::setBLUinAction(Atomic_action* node)
{
	h_variables->H[BLU_IN] = node->value;
}

void Executive::setBLUmmAction(Atomic_action* node)
{
	h_variables->H[BLU_MM] = node->value;
}

void Executive::setArcTolAction(Atomic_action* node)
{
	h_variables->H[ARC_TOL] = node->value;
}

void Executive::setInterpoStatAction(Atomic_action* node)
{
	switch (node->mode)
	{
	case INTERPOLATOR_DISABLE:
		// if inside thread interpolator
		if (isInterpolating())
		{
			// disables the interpolator 
			Interpolator::disable();
		}
		break;
	case INTERPOLATOR_ENABLE:
		// if outside thread interpolator
		if (!isInterpolating())
		{
			// enables interpolator 
			Interpolator::enable();

			// flags the executive to launch a new interpolator thread
			startInterpolator();
		}
		break;
	}
}

void Executive::setInterpoPortAction(Atomic_action* node)
{
	h_variables->H[INTERPOLATOR_PORT] = node->value;
}

void Executive::setInterpoBaudAction(Atomic_action* node)
{
	h_variables->H[INTERPOLATOR_BAUD] = node->value;
}

void Executive::setInterpoMaxFrqAction(Atomic_action* node)
{
	h_variables->H[INTERPOLATOR_MFRQ] = node->value;
}

void Executive::setInterpoLoggingAction(Atomic_action* node)
{
	switch (node->mode)
	{
	case STOP_LOGGING:
		Interpolator::stopLogging();
		break;
	case START_LOGGING:
		Interpolator::startLogging();
		break;
	}
}

void Executive::setInterpoBurstAction(Atomic_action* node)
{
	switch (node->mode)
	{
	case STOP_BURST:
		Interpolator::startLogging();
		break;
	case START_BURST:
		Interpolator::stopBurst();
		break;
	case RECEIVE_BURST:
		Interpolator::receiveBurst();
	}
}

void Executive::setVirtualMacStatAction(Atomic_action* node)
{
	switch (node->mode)
	{
	case VIRTMAC_DISABLE:
		// if inside thread virtualMachine
		if (isVirtualMachining())
		{
			virtualMachine->disable();
		}
		break;
	case VIRTMAC_ENABLE:
		// if outside thread virtualMachine
		if (!isVirtualMachining())
		{
			// flags the executive to launch a new virtual machine thread
			startVirtualMachine();
		}
		else {
			// to error handler
			errorHandler(ERR_MULTVIRTMAC);
			errorHandler(ERR_STARTVIRTMAC);
		}
		break;
	}
}

void Executive::setCloudNCStatAction(Atomic_action* node)
{
	switch (node->mode)
	{
	case CLOUDNC_DISABLE:
		// if inside thread cloudNC
		if (isCloudNCStreaming())
		{
			cloudNCVM->disable();
		}
		break;
	case CLOUDNC_ENABLE:
		// if outside thread cloudNC
		if (isCloudNCStreaming())
		{
			// to error handler
			errorHandler(ERR_MULTCLOUDNC);
		}
		// if inside virtual machine thread
		else if (isVirtualMachining())
		{
			// to error handler
			errorHandler(ERR_MULTVIRTMAC);
		}
		// if inside virtual interpolating thread
		else if (isInterpolating())
		{
			// to error handler
			errorHandler(ERR_MULTINTP);
		}
		// start the cloudNC VM thread
		else
		{
			// flags the executive to launch a new cloudNC thread
			startCloudNC();
		}
		break;
	}
}

void Executive::printAction(Atomic_action* node)
{
	// a handle for the files and serialized buffers 
	char *buffer = NULL;

	// serialize structures
	switch (node->mode)
	{
	case PRINT_ATOMIC_ACTIONS:
		// print to the socket
		if (winsocket->isConnected())
			winsocket->write(file2string(actionsFileName));
		// print to the console
		else {
			std::cout << file2string(actionsFileName) << "\n\n>";
		}
		break;

	case PRINT_ACTIONS_HISTORY:
		break;

	case PRINT_BLOCK_VECTOR:
		break;

	case PRINT_MODAL_VECTOR:
		break;

	case PRINT_MACHINE_VECTOR:
		buffer = serialize_machine_vector();
		// print to the socket
		if (winsocket->isConnected())
			winsocket->write(std::string(buffer));
		// print to the console
		else {
			std::cout << std::string(buffer) << "\n\n>";
		}
		// free memory
		json_free_serialized_string(buffer);
		break;
	default:
		break;
	}

}

void Executive::interpreterErrorAction(Atomic_action* node)
{
	// allocate memory for the message 
	std::string header = "";
	std::string body(node->error_message);
	int         lineno = node->lineno;
	int		    error_num = node->variable;
	std::string message;


	if (isProgramParsing())
	{
		// adds a line number header to the meesage 
		header = "line " + std::to_string(lineno) + ": ";
	}

	message = header + body;

	//print to the console
	std::cout << message << "\n\n>";

	// write to the socket
	if (winsocket->isConnected())
	{
		// serialze to JSON
		char* JSON_message = serialize_error_message(message.c_str(), error_num);
		winsocket->write(std::string(JSON_message));
		// free memory
		json_free_serialized_string(JSON_message);
	}
}

void Executive::exeparserErrorAction(Atomic_action* node)
{
	// allocate memory for the message 
	std::string header = "";
	std::string body(node->error_message);
	int         lineno = node->lineno;
	int		    error_num = node->variable;
	std::string message;

	if (isProgramParsing())
	{
		// adds a line number header to the meesage 
		header = "line " + std::to_string(lineno) + ": ";
	}

	message = header + body;

	//print to the console
	std::cout << message << "\n\n>";

	// write to the socket
	if (winsocket->isConnected())
	{
		// serialze to JSON
		char* JSON_message = serialize_error_message(message.c_str(), error_num);
		winsocket->write(std::string(JSON_message));
		// free memory
		json_free_serialized_string(JSON_message);
	}
}

void Executive::setExecutiveStatAction(Atomic_action* node)
{
	switch (node->mode)
	{
	case EXECUTIVE_DISABLE:
		disable();
		break;
	}
}

// other private methods

void Executive::updateTargetPositionAction(Atomic_action *node)
{
	/* update position */
	machine_vector->target_position.X = machine_vector->target_position.X + node->position.X;
	machine_vector->target_position.Y = machine_vector->target_position.Y + node->position.Y;
	machine_vector->target_position.Z = machine_vector->target_position.Z + node->position.Z;
	machine_vector->target_position.I = node->position.I;
	machine_vector->target_position.J = node->position.J;
	machine_vector->target_position.K = node->position.K;
}


