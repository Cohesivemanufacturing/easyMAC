/**************************************************
* easyMAC. Main          				          *
*                                                 *
* Jorge Correa                                    *
* Placid Ferreira                                 *
*                                                 *
* University of Illinois at urbana champaign      *
* 2016 - 2017                                     *
*                                                 *
***************************************************/
/*
* cloudNC-Executive Main
*
* This program is the multi-threaded operating system for the easyMAC modular architecture controller.
* It coordinates the non-real time services of the architecture (processing command, parsing and interpretation)
* and the execution of the real time services (interpolation, servo-control, PLC) in both physical and virtual
* Machines.
*
* Source files:
*
* Todo 11-9-2017 :
* 1.  complete the actionHandler.
* 2.  program interpolator actions and thread for collection of data in packages (burst mode),
*     transmition of packages, etc.
* 3.  update machine configuration file to include number of axis and axis servo variables
* 4.  comments classes properly (easyMAC box)
* 5.  program set commands to enable logs
* 6.  program get commands for atomic actions, modal vector, etc 
* 7.  make atomic actions, machine_vector, h_variables, modal_vector and block vector C++ classes
* 8.  replace parson with json.hpp
* 9.  continue development of cloudNC thread (virtualizer and interfaces)
*
* References:
*
* 1. Modern OpenGl tutorial:						   https://www.youtube.com/watch?v=bgckX62f4EA
* 2. writing Your First Multithreaded Program in C++   https://www.go4expert.com/articles/writing-multithreaded-program-cpp-t29980/
* 3. Sharing resources								   http://www.bogotobogo.com/cplusplus/C11/7_C11_Thread_Sharing_Memory.php
* 4. Win32 API Programming Tutorial                    http://www.winprog.org/tutorial/simple_window.html
* 5. Arduino and C++ Serial commication (for Windows)  http://playground.arduino.cc/Interfacing/CPPWindows
* 6. Polymorphism in C++                               https://www.tutorialspoint.com/cplusplus/cpp_polymorphism.htm
*/

#include <winsock2.h>        // needed for executive.h, Winsocket.h
#define WIN32_LEAN_AND_MEAN  // prevents windows.h from including winsock v1
#include <windows.h>         // needed for executive.h
#include <iostream> 
#include <string>
#include <thread>
#include <mutex>

#include "executive.h"       // includes interpreter.h, exeparser.h, interpolator.h, virtualmac.h, winsocket.h 
#include "ConsoleLogger.h"

/**************************************************************
Global variables (shared between threads)
***************************************************************/
/*
* mutex block specific lines of code executing in multiple threads so
* that one thread can access them at a time.
*/
std::mutex     myMutex;
Executive	   *executive;
CConsoleLogger *ppConsole, *cnConsole, *inConsole;

/**************************************************************
 Function definitions
 ***************************************************************/

void splashWindow(Display& display, Shader& shader)
{
	Mesh    mesh0("./res/virtualMachine/virtualMAC.txt");
	Texture texture0("./res/virtualMachine/virtualMAC.jpg");
	Camara  camara0(glm::vec3(0, 0, 2), 70.0f, (float)WIDTH / (float)HEIGHT, 0.01f, 1000.0f);
	display.Clear(0.0f, 0.15f, 0.3f, 1.0f);

	glm::mat4 model0 = camara0.GetViewProjection();

	shader.Bind();

	texture0.Bind(0);
	shader.Update(model0);
	mesh0.Draw();

	display.Update();
}

void hello_OAC(void)
{
	int c;
	FILE *file;
	file = fopen("./res/executive/config/easyMAC.txt", "r");

	if (file) {
		while ((c = getc(file)) != EOF)
			putchar(c);
		fclose(file);
	}
}

/**************************************************************
 Threads
 ***************************************************************/
 /*
 * userTerminal. Controls the excecution of the console
 */
void thread_console()
{
	char *pblock = new char[256];

	while (Executive::isEnabled())
	{
		// wait for a console command
		std::cin.getline(pblock, 256);

		// aknowledge message
		std::cout << "\n>";

		// the interpreter and exeparser need a termination character
		strcat(pblock, "\n");

		/*
		* parses the block. protect with mutex as the interpreter and
		* executive parser modify global variables.
		*/

		myMutex.lock();

		// option spetial character interpreter
		if (pblock[0] == '$')
		{
			Executive::interpretBlock(++pblock);
			Executive::logAtomicActions();
			Executive::actionHandler();
		}
		else
		{
			Executive::parseBlock(pblock);
			Executive::logAtomicActions();
			Executive::actionHandler();
		}

		myMutex.unlock();
	}

	delete pblock;
}

/*
* ParseProgram. Interpretes a program block by block and terminates
* after parsing through the entire file.
*/
void thread_parseProgram()
{
	// log a new console
	ppConsole = new CConsoleLogger();
	ppConsole->Create("Program file interpreter console");

	// open program 
	char *fileName = Executive::getProgramFileName();

	FILE* fp = fopen(fileName, "r");

	// if no sintax errors
	if (fp != NULL)
	{
		Executive::messageHandler(MSG_OPENPROG);

		// check program

		// resets the interpreter line number
		Interpreter::resetLineNumber();

		// a buffer for the block
		char block[256];

		// pointer to the head and tail actions
		Atomic_action **head = Executive::getActionIn();
		Atomic_action **tail = Executive::getActionOut();

		// parseProgram Thread main loop
		while (Executive::isEnabled() && !feof(fp))
		{
			// if the executive stops the program execution
			if (!Executive::isProgramParsing())
				break;
			// if buffers full or waiting for msg confirmation 
			if (!Executive::areBuffersWaiting())
			{
				// protect with mutex
				myMutex.lock();

				// 1. process the next action on the queue 
				if (*tail != *head)
				{
					Executive::actionHandler(*tail);
					*tail = delete_action(*tail);
				}

				// 2. parse next block 
				else
				{
					// get new block
					fgets(block, sizeof(block), fp);

					// let the user know
					ppConsole->printf("%s\n>", block);

					// parse block (generate actions)
					Executive::interpretBlock(block);
					Executive::logAtomicActions();
				}

				myMutex.unlock();
			}

			// a state to pause the program
			while (Executive::isProgramPaused())
			{
				Sleep(50);
			}
		}

		// to message handler
		printf("end of program\n\n>");

		// end program
		fclose(fp);
	}

	else
	{
		Executive::errorHandler(ERR_OPENPROG);
	}

	// close the console
	ppConsole->Close();
	delete ppConsole;

	// notifies the threadhandler that the parseProgram thread has been finalized
	Executive::exitProgram();
}

/*
* virtualMachine. Has a virtual representation of the NC machine with
* replicas of the interpolation and servo control programs in easyMAC.
*/
void thread_virtualMachine()
{
	// initialized the SDL and openGL enviroments 
	Display display(800, 600, "Hello virtualMAC!");
	Shader  shader("./res/virtualMachine/Shader");
	Camara  camara(glm::vec3(0, 5, 15), 70.0f, (float)WIDTH / (float)HEIGHT, 0.01f, 1000.0f);

	// a humble splash window
	splashWindow(display, shader);

	// machine configuratiion file
	std::string  mcf = "./res/virtualMachine/3AxisMill.smd";

	// initilize the Virtual Machine and bind the to the SLD and openGL enviroments
	VirtualMachine *virtualMachine = Executive::initVirtualMachine(mcf, &display, &shader, &camara);

	volatile Buffer *buffer = virtualMachine->buffer;

	while (virtualMachine->isEnabled() && Executive::isEnabled())
	{
		if (display.IsClosed())
			// to message handler
			break;

		// virtual machine interpolator
		if (!buffer->isEmpty())
		{
			// gets the block and extracts is from the buffer
			char* data_in = buffer->read();

			// process the block
			virtualMachine->ProcessCommand(data_in);
		}

		// this a good point to insert a replica of the PLC
		else
		{
			display.GetEvents();
			virtualMachine->Render();
		}
	}

	// delete virtualMachine object
	delete virtualMachine;

	// notifies the executive that the parseProgram thread has been finalized
	Executive::exitVirtualMachine();
}

/*
* interpolator. Receives interpolator incomming messages and hands
* them to the other components in the architecture
*/
void thread_interpolator()
{
	// log a new console
	inConsole = new CConsoleLogger();
	inConsole->Create("Interpolator communications console");
	/*
	 * creates an interpolator object and gets pointers for
	 * the serial port an circular buffers
	 */
	Interpolator    *interpolator = Executive::initInterpolator();
	Serial          *port = Interpolator::port;
	volatile Buffer *sendBuffer = Interpolator::buffer;

	Interpolatordata interpolator_data;
	Bufferdata buffer_data;

	// if port succesfully connected
	if (port->isConnected())
	{
		// notifies user a succesfull connection event
		Executive::messageHandler(MSG_CONINTP);

		// interpolator startup sequence
		Interpolator::startupSequence();

		// interpolator serial event
		while (Interpolator::isEnabled() && Executive::isEnabled())
		{
			// if the COM port is not longer available
			if (!port->isCOMportAvailable())
			{
				Executive::errorHandler(ERR_COMPORTAV);
				break;
			}

			// if there is data in the serial buffer
			if (port->available())
			{
				char *data_in = port->readln();
				/*
				* Modification
				* Process to keep track of the interpolated position
				* by Jorge Correa 8-4-2017
				* 1. maintain the interpolated position inside the interpolator object.
				* 2. implemented convection for the interpolator's response messages:
				*	end of block character: >
				*	message character: $
				*	position update character: #
				*/

				// protect the buffers and globals from a write-write clash
				//myMutex.lock();

				// interpolator message parser

				// case: end of block
				if (*data_in == '>')
				{
					// update the interpolator's position
					Interpolator::updateTarget();

					// read form the fifo com. buffer
					inConsole->printf(">%s\n", sendBuffer->read());

				}
				// case : message
				if (*data_in == '$')
				{
					// print the interpolator message
					inConsole->printf("<%s\n", ++data_in);
				}
				// case: position feed 
				if (*data_in == '#') {
					if (Interpolator::parseFeedData(++data_in, interpolator_data)) {
						// parse interpolator data
						//inConsole->printf("<%s\n", ++data_in);

						// if tail poniters of the send and receive buf. not sync 
						if (interpolator_data.countin != sendBuffer->countback) {
							sendBuffer->countback = interpolator_data.countin;
						}

						if (interpolator_data.countout != sendBuffer->countout) {
							sendBuffer->countout = interpolator_data.countout;
						}

						// update the interpolator position data
						Interpolator::updatePosition(interpolator_data);
					}
				}
				// new block aknowledgment
				if (*data_in == '%') {
					// parse buffer data
					Interpolator::parseBufferData(++data_in, buffer_data);

					// if no error in message transmission 
					if (buffer_data.error == 13)
					{
						// send the message at the head pointer of the receiving buffer
						std::string block = (char *)sendBuffer->buffer[sendBuffer->countback];
						port->write((char*)block.c_str(), block.length());
					} else 
					{
						sendBuffer->countback = buffer_data.countin;
					}
				}
				//myMutex.unlock();
			}
		}
	}
	else
	{
		Executive::errorHandler(ERR_CONINTP);
	}

	// close the console
	inConsole->Close();
	delete inConsole;

	// delete the interpolator object
	delete interpolator;

	// notifies the executive that the parseProgram thread has been finalized
	Executive::exitInterpolator();
}

/*
* winsock. Waits for a windows socket connection from the NC server.
* This socket receives easyMAC commands to control the open control
* architecture.
*/
void thread_winsocket(int portnb)
{
	// creates a windows listening socket at port 7171
	// for API communications with the NC server  
	Winsocket *socket = Executive::initWinSocket(portnb);

	Executive::winSocketStarted();

	char *data_in;
	char *buffer;

	while (Executive::isEnabled())
	{
		// two possible states:
		if (socket->isConnected())
		{
			// read incomming messgages
			if (socket->isAvailable() != 0)
			{
				data_in = socket->read();

				// the interpreter and exeparser need a termination character
				strcat(data_in, "\n");

				// option spetial character interpreter
				if (data_in[0] == '$')
				{
					// protect with mutex
					myMutex.lock();

					Executive::interpretBlock(++data_in);
					Executive::logAtomicActions();
					Executive::actionHandler();

					myMutex.unlock();
				}
				else if (data_in[0] == '#') 
				{
					// a handle for the files and serialized buffers 
					buffer = Executive::serialize_machine_vector();
					
					// print to the socket
					socket->write(std::string(buffer));
					
					// free memory
					json_free_serialized_string(buffer);
				}
				// go to the executive parser
				else
				{
					// protect with mutex
					myMutex.lock();

					Executive::parseBlock(data_in);
					Executive::logAtomicActions();
					Executive::actionHandler();

					myMutex.unlock();
				}
			}
			else {
				// notifies user a succesfull connection with the socket
				Executive::errorHandler(ERR_DISCSOCKET);
				// send a heartbeat to OSCM. If error sending, disconnect the socket 
				socket->write("ping");
			}
		}

		if (!socket->isConnected())
		{
			// wait for a connection on the listening socket
			if (socket->connect())
			{
				// notifies user a succesfull connection with the socket
				Executive::messageHandler(MSG_CONSOCKET);
			}
		}
	}
}

/*
* cloudNC. Virtual representation of the numerically controlled machine with
* a replica of the interpolation programs specifically tailored to the
* clould-control of manufacturing resources.
*/
void thread_cloudNC()
{
	// log a new console
	cnConsole = new CConsoleLogger();
	cnConsole->Create("Program file interpreter console");

	// initilization and settings of the CloudNC controller 
	VirtualMachine *cloudController = Executive::initCloudNC();  // no mech. structure or SLD graphics needed
	
	cloudController->setConsole(cnConsole);                      // 

	volatile Buffer *buffer = cloudController->buffer;           // FIFO buffer for communications with the executive

	while (cloudController->isEnabled() && Executive::isEnabled())
	{
		// virtual machine interpolator
		if (!buffer->isEmpty())
		{
			// gets the block and extracts is from the buffer
			char* data_in = buffer->read();

			// process the block
			cloudController->ProcessCommand(data_in);
		}
	}

	// delete virtualMachine object
	delete cloudController;

	// close the console
	cnConsole->Close();
	delete cnConsole;

	// notifies the executive that the parseProgram thread has been finalized
	Executive::exitCloudNC();
}

/*
* winsock. Waits for a windows-socket connection from the NC server.
* This socket sends a compiled motion stream to the NC server for cloud
* control of manufactuirng resources.
*/
void thread_cloudNCsocket(int portnb)
{
	// creates a windows listening socket at port 8181
	// for API communications with the NC server  
	Winsocket *cloudNCsocket = Executive::initCloudNCSocket(portnb);

	Executive::clncSocketStarted();

	char *data_in;
	char *buffer;

	while (Executive::isEnabled())
	{
		// two possible states:
		if (cloudNCsocket->isConnected())
		{
			// read incomming messgages
			if (cloudNCsocket->isAvailable() != 0)
			{
				// an oportunity to read from the socket
				data_in = cloudNCsocket->read();
			}
			else {
				// notifies user a succesfull connection with the socket
				Executive::errorHandler(ERR_DISCCLNCSOCKET);
				// send a heartbeat to OSCM. If error sending, disconnect the socket 
				cloudNCsocket->write("ping");
			}
		}

		if (!cloudNCsocket->isConnected())
		{
			// wait for a connection on the listening socket
			if (cloudNCsocket->connect())
			{
				// notifies user a succesfull connection with the socket
				Executive::messageHandler(MSG_CONCLNCSOCKET);
			}
		}
	}
}

/**************************************************************
main Thread
***************************************************************/

int main(int argc, char ** argv)
{
	/**************************************************************
	initialize Excecutive
	***************************************************************/

	// hello open control architecture
	hello_OAC();

	// starts the executive structures and thread handler
	executive = Executive::initExecutive();

	// set log files and root folders
	Executive::setHistoryLogFile("./res/interpreter/logs/history.log");
	Executive::setActionsLogFile("./res/interpreter/logs/actions.log");
	Executive::setRootFolder("./res/interpreter/programs/");

	// loads the system initial configurations
	if (Executive::loadConfiguration("./res/executive/config/easyMAC.ini"))
	{
		Executive::messageHandler(MSG_INICONFIG);
	}
	else
	{
		Executive::errorHandler(ERR_INICONFIG);
	}

	/**************************************************************
	load Peripherals
	***************************************************************/

	// declare the threads

	std::thread twc;   // a thread for the windows console
	std::thread tpp;   // a thread to parse NC programs
	std::thread tin;   // a thread for communicating with the interpolator 
	std::thread tvm;   // a thread for the virtual machine
	std::thread tws;   // a thread for the windows socket
	std::thread tcn;   // a thread for cloudNC 
	std::thread tcs;   // a thread for the cloudNC socket 

	twc = std::thread(thread_console);    // start the windows console

	// start socket threads for interprocess communications
	if (argc == 3) {
		char *p;
		int winSocketPort = strtol(argv[1], &p, 10);
		int cloudNCport = strtol(argv[2], &p, 10);
		tws = std::thread(thread_winsocket, winSocketPort);   // start the windows socket at portnb argv[0]
		while (!Executive::isWinSocketStarted()) { }          // wait for the winsocket to start
		tcs = std::thread(thread_cloudNCsocket, cloudNCport); // start cloudnd socket at portnb argv[1]
		while (!Executive::isCLNCSocketStarted()) { }         // wait for the clncsocket to start
	}
	else {
		tws = std::thread(thread_winsocket, 7171);            // start windows socket at portnb 7171     
		while (!Executive::isWinSocketStarted()) { };		  // wait for the winsocket to start	
		tcs = std::thread(thread_cloudNCsocket, 8181);        // start cloudNC socket at portnb 8181
		while (!Executive::isCLNCSocketStarted()) { }         // wait for the clncsocket to start
	}
	
	/**************************************************************
	machine startup sequence
	***************************************************************/

	/**************************************************************
	main thread infinite loop
	***************************************************************/

	while (Executive::isEnabled() || Executive::isThreading())
	{
		if (Executive::isProgramStarted())
		{
			// launches the parse program thread
			tpp = std::thread(thread_parseProgram);

			// notifies the executive that the parse program thread has been launched
			Executive::programStarted();

			if (tpp.joinable())
				Executive::messageHandler(MSG_STARTPROG);
			else
				Executive::errorHandler(ERR_STARTPROG);
		}

		if (Executive::isProgramExited())
		{
			// finalized the program
			tpp.join();

			// notifies the executive that the parse program thread has been finalized
			Executive::programExited();

			if (tpp.joinable())
				Executive::errorHandler(ERR_EXITPROG);
			else
				Executive::messageHandler(MSG_EXITPROG);
		}

		if (Executive::isInterpolatorStarted())
		{
			// launched the interpolator thread
			tin = std::thread(thread_interpolator);

			// notifies the executive that the interpolator thread has been launched
			Executive::interpolatorStarted();

			if (tin.joinable())
				Executive::messageHandler(MSG_STARTINTP);
			else
				Executive::errorHandler(ERR_STARTINTP);
		}

		if (Executive::isInterpolatorExited())
		{
			// finalized the program
			tin.join();

			// notifies the executive that the interpolator thread  has been finalized
			Executive::interpolatorExited();

			if (tpp.joinable())
				Executive::errorHandler(ERR_EXITINTP);
			else
				Executive::messageHandler(MSG_EXITINTP);
		}

		if (Executive::isvirtualMachineStarted())
		{
			// launched the virtual machine thread
			tvm = std::thread(thread_virtualMachine);

			// notifies the executive that the virtual machine thread has been launched
			Executive::virtualMachineStarted();

			if (tvm.joinable())
				Executive::messageHandler(MSG_STARTVIRTMAC);
			else
				Executive::errorHandler(ERR_STARTVIRTMAC);
		}

		if (Executive::isvirtualMachineExited())
		{
			// finalized the program
			tvm.join();

			// notifies the executive that the virtual machine thread  has been finalized
			Executive::virtualMachineExited();

			if (tvm.joinable())
				Executive::errorHandler(ERR_EXITVIRTMAC);
			else
				Executive::messageHandler(MSG_EXITVIRTMAC);
		}

		if (Executive::isCloudNCStarted()) {
			// launched the cloudNC thread
			tcn = std::thread(thread_cloudNC);

			// notifies the executive that the cloudNC thread has been launched
			Executive::cloudNCVMStarted();

			if (tcn.joinable())
				Executive::messageHandler(MSG_STARTCLOUDNC);
			else
				Executive::errorHandler(ERR_STARTCLOUDNC);
		}

		if (Executive::isCloudNCExited())
		{
			// finalized the program
			tcn.join();

			// notifies the executive that the cloudNC thread  has been finalized
			Executive::cloudNCVMExited();

			if (tcn.joinable())
				Executive::errorHandler(ERR_EXITCLOUDNC);
			else
				Executive::messageHandler(MSG_EXITCLOUDNC);
		}

		// other cases (jogging, lock servos..etc)

		Sleep(20); // be kind to the processor
	}

	// waits until all threads finalize

	twc.detach();     // detaches the console thread
	tws.detach();     // detaches the windows socket thread
	tcs.detach();     // detaches the cloudNC socket thread  

	// deletes exective object
	delete executive;

	return 0;
}

