#pragma once

#include <windows.h> // for serial.h
#include <iostream>

#include "serial.h"                     // serial communications object
#include "buffer.h"                     // buffer for thread communcations
#include "h_variables.h"                // h_variables type and defs
#include "machine_vector.h"             // machine_vector type and defs
#include "atomic_actions.h"             // atomic_actions type, defs and methods
#include "modal_vector.h"               // modal_vector type, defs and methods
#include "interpolator_parameters.h"    // interpolator communication structures

class Interpolator
{ 
public:

	/**************************************************************
	Interpolator public members
	***************************************************************/
	static Serial          *port;			     // a serial port to communicate with the interpolator
	static volatile Buffer *buffer;				 // a circular buffer of strings the messages sent to the interpolator 
	
	/**************************************************************
	Interpolator Services
	***************************************************************/
	static Interpolator* Interpolator::initInterpolator(char *portName, int baud);
	static void			 Interpolator::startupSequence();
	static std::string   Interpolator::straightFeed(Atomic_action *node);
	static std::string   Interpolator::arcFeed(Atomic_action *node);
	static void			 Interpolator::updateTarget();
	static void			 Interpolator::updatePosition(Interpolatordata& data);
	static bool		     Interpolator::parseFeedData(char* block, Interpolatordata& data);
	static void		     Interpolator::parseBufferData(char* block, Bufferdata& data);
	static axis          Interpolator::getInterpolatorPosition(void);
	static axis			 Interpolator::getInterpolatorNativePosition(void);
	// message handler

	// public virtual distructor
	virtual ~Interpolator();

	/**************************************************************
	getters
	***************************************************************/
	static inline bool isEnabled() { return m_isEnabled;}  
	static inline bool isLogging() { return m_isLogging;}
	static inline bool isBurstGathering() { return m_isBurstGathering;}
	static inline bool isBurstReceiving() { return m_isBurstReceiving;}

	/**************************************************************
	setters
	***************************************************************/
	// executive structures
	static inline void setHvariables(H_variables *var) { h_variables = var; }
	static inline void setMachineVector(Machine_vector *vector) { machine_vector = vector; }

	// interpolator program
	static inline void enable()  { m_isEnabled = true; }
	static inline void disable() { m_isEnabled = false; }

	// communictaions mode
	static inline void startLogging() 
	{ 
		m_isLogging = true;
		m_isBurstGathering = false;
		m_isBurstReceiving = false;
	}
	static inline void stopLogging()  
	{
		m_isLogging = false;
		m_isBurstGathering = false;
		m_isBurstReceiving = false;
	}
	static inline void startBurst()   
	{
		m_isLogging = false;
		m_isBurstGathering = true;
		m_isBurstReceiving = false;
	}
	static inline void stopBurst()    
	{ 
		m_isLogging = false;
		m_isBurstGathering = false;
		m_isBurstReceiving = false;
	}
	static inline void receiveBurst()
	{
		m_isLogging = false;
		m_isBurstGathering = false;
		m_isBurstReceiving = false;
	}

private:
	
	// private constructor 
	Interpolator() {}

	// interpolator private members
	static Interpolator    *interpolator;        // a pointer to the singleton object
	static H_variables     *h_variables;	     // H variables for configuration      
	static Machine_vector  *machine_vector;      // machine state vector
	static axis				position;            // the current interpolatuion position 
	static axis 			target;              // the starting target for the current interpollation task  

	// interpolator state variables
	static volatile bool m_isEnabled;            // interpolator program
	static volatile bool m_isLogging;
	static volatile bool m_isBurstGathering;
	static volatile bool m_isBurstReceiving;

	// private methods
	static int   Interpolator::calulate_feed_rate_number(void);
	static void  Interpolator::arc_to_arcxyblu(axis *src, axis *dst);
	static void  Interpolator::targetsxy_to_targets(InterpolatorParameters *src, InterpolatorParameters *dst);

	// this would be nice functions to integrate an H_variables singleton object
	static float Interpolator::get_machine_BLU(void);
	static float Interpolator::get_interpolator_maxfreq(void);
	static float Interpolator::get_interpolator_feedreg(void);
};

