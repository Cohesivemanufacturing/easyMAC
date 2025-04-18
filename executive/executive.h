#pragma once

/**************************************************
* easyMAC. Excecutive          				      *
*                                                 *
* Jorge Correa                                    *
* Placid Ferreira                                 *
*                                                 *
* University of Illinois at urbana champaign      *
* 2016 - 2017                                     *
*                                                 *
***************************************************/
/*
* Executive
*
* Finite-state-machine atomaton with the methods for handling the threads of the operating system 
* and the atomic_actions produced by the executive parser and the interpreter.
*
* Todo:
*  
* 
* References:
* 1. Singleton Pattern & its implementation with C++ : https://www.codeproject.com/Articles/1921/Singleton-Pattern-its-implementation-with-C
*
*/

#include <winsock2.h>        // needed for Winsocket.h
#define WIN32_LEAN_AND_MEAN  // prevents windows.h from including winsock v1
#include <windows.h>         // needed for Interpolator.h
#include <iostream> 
#include <string>
#include <sstream>           // std::stringstream
#include <filesystem>        // Include the filesystem header

#include "parson.h"          // deprecated

#include "executive_defs.h"  // error and message handlers constants
#include "interpreter.h"     // atomic_actions.h, machine_vector.h, h_variables.h, modal_vector.h, block_vector.h
#include "exeparser.h"       // atomic_actions.h, machine_vector.h, h_variables.h, modal_vector.h, block_vector.h
#include "interpolator.h"    // serial.h, buffer.h, interpoltor_parameters.h 
#include "virtualMachine.h"  // display.h, shader.h, mesh.h, texture.h, transform, camara.h
#include "Winsocket.h"       // Winsocket class 

class Executive
{
public:

	/**************************************************************
	Executive Services
	***************************************************************/
	static Executive	  *Executive::initExecutive(void);
	static int			   Executive::loadConfiguration(char* fileName);
	static Interpolator   *Executive::initInterpolator(void);
	static VirtualMachine *Executive::initVirtualMachine(const std::string& fileName, Display* display, Shader* shader, Camara* camara);
	static VirtualMachine* Executive::initCloudNC(void);
	static Winsocket      *Executive::initWinSocket(int port);
	static Winsocket      *Executive::initCloudNCSocket(int port);
	static char           *Executive::serialize_machine_vector(void);
	static int			   Executive::checkProgram(void);
	static void			   Executive::parseBlock(char* block);
	static void		       Executive::interpretBlock(char* block);
	static void			   Executive::actionHandler(void);       
	static void			   Executive::actionHandler(Atomic_action* node); // ovearlaoding function
	static void			   Executive::logAtomicActions(void);
	static void			   Executive::messageHandler(int num);
	static void			   Executive::errorHandler(int num);

	// public virtual distructor
	virtual ~Executive();

	/**************************************************************
	getters
	***************************************************************/
	// executive structures
	static inline float			  getHvariable(int var) { return h_variables->H[var]; }
	static inline Atomic_action **getActionIn() { return &atomic_action_in; }
	static inline Atomic_action **getActionOut() { return &atomic_action_out; }
	static inline char           *getProgramFileName() { return programFileName; }

	// thread handling
	static inline bool isEnabled() { return m_isEnabled; }
	static inline bool isEstop() { return m_Estop; }
	static inline bool isThreading()  
	{ 
		return  (isProgramParsing() || isVirtualMachining() || isInterpolating()); 
	}
	static inline bool areBuffersFull() 
	{
		if (isVirtualMachining())
		{
			if (virtualMachine->buffer->isFull())
				return true;
		}

		if (isCloudNCStreaming()) {
			if (virtualMachine->buffer->isFull())
				return true;
		}
			
		if (isInterpolating())
		{
			if (interpolator->buffer->isFull())
				return true;
		}
		return false;
	}
	static inline bool areBuffersWaiting()
	{
		if (isVirtualMachining())
		{
			if (virtualMachine->buffer->isFull())
				return true;
		}

		if (isCloudNCStreaming()) {
			if (cloudNCVM->buffer->isFull())
				return true;
		}
		
		if (isInterpolating())
		{
			if (interpolator->buffer->isWaiting())
				return true;
		}
		return false;
	}

	// parseProgram thread
	static inline bool isProgramStarted(){ return m_startProgram; }
	static inline bool isProgramExited() { return m_exitProgram; }
	static inline bool isProgramParsing() { return m_parsingProgram; }
	static inline bool isProgramPaused() { return m_pausedProgram; }

	// interpolator thread
	static inline bool isInterpolatorStarted() { return m_startInterpolator; }
	static inline bool isInterpolatorExited() { return m_exitInterpolator; }
	static inline bool isInterpolating() { return m_interpolating; }

	// virtual Machine thread
	static inline bool isvirtualMachineStarted() { return m_startVirtualMachine; }
	static inline bool isvirtualMachineExited() { return m_exitVirtualMachine; }
	static inline bool isVirtualMachining() { return m_virtualMachining;}

	// cloudNC 
	static inline bool isCloudNCStarted() { return m_startCloudNC; }
	static inline bool isCloudNCExited() { return m_exitCloudNC; }
	static inline bool isCloudNCStreaming() { return m_cloudNCStreaming; }

	// 
	static inline bool isCLNCSocketStarted() { return m_isCLNCSocketStarted; }
	static inline bool isWinSocketStarted() { return m_isWinSocketStarted; }

	/**************************************************************
	setters
	***************************************************************/
	// executive program
	static inline void enable()  { m_isEnabled = true; }
	static inline void disable() { m_isEnabled = false; }

	// log files

	static inline void setActionsLogFile(char* fileName)
	{
		// save actions file name
		actionsFileName = fileName;

		// gratiously try to open the file and clear it
		try {
			// Use filesystem to create directories if they don't exist
			std::filesystem::path filePath(actionsFileName);
			if (filePath.has_parent_path()) {
				std::filesystem::create_directories(filePath.parent_path());
			}

			FILE* file = fopen(actionsFileName, "w"); // Open in write mode to clear the file
			if (file == NULL) {
				std::cerr << "Error creating actions log file: " << actionsFileName << std::endl;
				return;
			}
			fclose(file);
		}
		catch (const std::exception& e) {
			// caught by reference to base
			std::cout << " a standard exception was caught, with message '"
				<< e.what() << "'\n";
		}
	}
	static inline void enableActionLog()
	{
		if (actionsFileName != NULL)
		{
			m_logActions = true;
		}
	}
	static inline void disableActionLog() 
	{
		if (m_logActions)
		{
			m_logActions = false;
		}
	}

	// parseProgram thread states
	static inline void startProgram()
	{
		m_startProgram = true;
		m_exitProgram = false;
		m_parsingProgram = false;
		m_pausedProgram = false;
	}
	static inline void programStarted()
	{
		m_startProgram = false;
		m_exitProgram = false;
		m_parsingProgram = true;
		m_pausedProgram = false;
	}
	static inline void pauseProgram() 
	{
		m_startProgram = false;
		m_exitProgram  = false;
		m_parsingProgram = true;
		m_pausedProgram = true;
	}
	static inline void stopProgram() 
	{
		m_startProgram = false;
		m_exitProgram = false;
		m_parsingProgram = false;
		m_pausedProgram = false;
	}
	static inline void continueProgram()
	{
		m_startProgram = false;
		m_exitProgram = false;
		m_parsingProgram = true;
		m_pausedProgram = false;
	}
	static inline void exitProgram()
	{
		m_startProgram = false;
		m_exitProgram = true;
		m_parsingProgram = true;
		m_pausedProgram = false;
	}
	static inline void programExited()
	{
		m_startProgram = false;
		m_exitProgram = false;
		m_parsingProgram = false;
		m_pausedProgram = false;
	}

	// interpolator thread states
	static inline void startInterpolator()
	{
		m_startInterpolator = true;
		m_exitInterpolator = false;
		m_interpolating = false;
	}
	static inline void interpolatorStarted()
	{
		m_startInterpolator = false;
		m_exitInterpolator = false;
		m_interpolating = true;
	}
	static inline void exitInterpolator()
	{
		m_startInterpolator = false;
		m_exitInterpolator = true;
		m_interpolating = true;
	}
	static inline void interpolatorExited()
	{
		m_startInterpolator = false;
		m_exitInterpolator = false;
		m_interpolating = false;
	}

	// virtualMachine thread states
	static inline void startVirtualMachine()
	{
		m_startVirtualMachine = true;
		m_exitVirtualMachine = false;
		m_virtualMachining = false;
	}
	static inline void virtualMachineStarted()
	{
		m_startVirtualMachine = false;
		m_exitVirtualMachine = false;
		m_virtualMachining  = true;
	}
	static inline void exitVirtualMachine()
	{
		m_startVirtualMachine = false;
		m_exitVirtualMachine = true;
		m_virtualMachining = true;
	}
	static inline void virtualMachineExited()
	{
		m_startVirtualMachine = false;
		m_exitVirtualMachine = false;
		m_virtualMachining = false;
	}

	// cloudNC virtual machine thread states
	static inline void startCloudNC()
	{
		m_startCloudNC = true;
		m_exitCloudNC = false;
		m_cloudNCStreaming = false;
	}
	static inline void cloudNCVMStarted()
	{
		m_startCloudNC = false;
		m_exitCloudNC = false;
		m_cloudNCStreaming = true;
	}
	static inline void exitCloudNC()
	{
		m_startCloudNC = false;
		m_exitCloudNC = true;
		m_cloudNCStreaming = true;
	}
	static inline void cloudNCVMExited()
	{
		m_startCloudNC = false;
		m_exitCloudNC = false;
		m_cloudNCStreaming = false;
	}

	// 
	static inline void winSocketStarted() {
		m_isWinSocketStarted = true;
	}
	static inline void clncSocketStarted() {
		m_isCLNCSocketStarted = true;
	}

protected:

private:
	
	// private constructor and virtual distructor
	Executive() {}

	// executive members
	static Executive	  *executive;           // a pointer to the singleton object 
	static H_variables    *h_variables;         // H variables for configuration              
	static Block_vector   *block_vector;        // vector to store the incomming block of Gcodes
	static Modal_vector   *modal_vector;        // vector with the current modals coonfiguration 
	static Machine_vector *machine_vector;      // machine state vector
	static Atomic_action  *atomic_action_in;    // pointer to the head atomic action
	static Atomic_action  *atomic_action_out;   // pointer to the tail atomic action 
	static FILE           *history_file;        // pointer to the executive history file
	static Interpreter    *interpreter;         // pointer to the interpreter object
	static Exeparser      *exeparser;           // pointer to the executive parser object
	static Interpolator   *interpolator;        // pointer to the inteprolator communciations object
	static VirtualMachine *virtualMachine;      // pointer to the virtualMachine object
	static Winsocket      *winsocket;           // pointer to the windows socket to receive easyMAC commands
	static VirtualMachine *cloudNCVM;           // pointer to the virtualMachine object
	static Winsocket      *cloudNCsocket;       // pointer to the windows socket to send cloudMAC compiled motion streams 
	static char			  *actionsFileName;     // pointer to the atomic actions log file name
	static char           *programFileName;     // pointer to the program file name
	
	// thread state variables
	static volatile bool m_isEnabled;           // executive program 
	static volatile bool m_logActions;          // log files
	static volatile bool m_logHistory;          
	static volatile bool m_Estop;               // emergency stop 
	static volatile bool m_startProgram;	    // parseProgram thread         
	static volatile bool m_exitProgram;
	static volatile bool m_parsingProgram;      
	static volatile bool m_pausedProgram;
	static volatile bool m_startInterpolator;   // interpolator thread
	static volatile bool m_exitInterpolator;
	static volatile bool m_interpolating;
	static volatile bool m_startVirtualMachine; // virtual machine thread 
	static volatile bool m_exitVirtualMachine;
	static volatile bool m_virtualMachining;
	static volatile bool m_startCloudNC;
	static volatile bool m_exitCloudNC;
	static volatile bool m_cloudNCStreaming;
	static volatile bool m_isWinSocketStarted;
	static volatile bool m_isCLNCSocketStarted;

	// atomic action handler
	static void Executive::loadProgramFileAction(Atomic_action* node);
	static void Executive::loadConfigFileAction(Atomic_action* node);
	static void Executive::setActionsLogAction(Atomic_action* node);
	static void Executive::setProgramAction(Atomic_action* node);
	static void Executive::setSeqNumberAction(Atomic_action* node);
	static void Executive::setFeedModeAction(Atomic_action* node);
	static void Executive::setFeedRateAction(Atomic_action *node);
	static void Executive::setSpindleSpeedAction(Atomic_action *node);
	static void Executive::setPlaneAction(Atomic_action* node);
	static void Executive::setLengthUnitsAction(Atomic_action* node);
	static void Executive::setRadiusCompensationAction(Atomic_action* node);
	static void Executive::setLengthCompensationAction(Atomic_action* node);
	static void Executive::setRetractionModeAction(Atomic_action* node);
	static void Executive::setCoordinateSytemAction(Atomic_action* node);
	static void Executive::setPathModeAction(Atomic_action* node);
	static void Executive::setDistanceModeAction(Atomic_action* node);
	static void Executive::setOriginOffsetsAction(Atomic_action* node);
	static void Executive::setMotionModeAction(Atomic_action* node);
	static void Executive::straightFeedAction(Atomic_action* node);
	static void Executive::arcFeedAction(Atomic_action* node);
	static void Executive::setHvariableAction(Atomic_action* node);
	static void Executive::setBLUinAction(Atomic_action* node);
	static void Executive::setBLUmmAction(Atomic_action* node);
	static void Executive::setArcTolAction(Atomic_action* node);
	static void Executive::setInterpoStatAction(Atomic_action* node);
	static void Executive::setInterpoPortAction(Atomic_action* node);
	static void Executive::setInterpoBaudAction(Atomic_action* node);
	static void Executive::setInterpoMaxFrqAction(Atomic_action* node);
	static void Executive::setInterpoLoggingAction(Atomic_action* node);
	static void Executive::setInterpoBurstAction(Atomic_action* node);
	static void Executive::setVirtualMacStatAction(Atomic_action* node);
	static void Executive::setCloudNCStatAction(Atomic_action* node);
	static void Executive::printAction(Atomic_action* node);
	static void Executive::interpreterErrorAction(Atomic_action* node);
	static void Executive::exeparserErrorAction(Atomic_action* node);
	static void Executive::setExecutiveStatAction(Atomic_action* node);

	// other private members
	static void Executive::updateTargetPositionAction(Atomic_action *node);
};
