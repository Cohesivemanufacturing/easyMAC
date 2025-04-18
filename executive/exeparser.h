#pragma once
/***************************************************
* easyMac executive parser 1.0 				       *
* Utility function declarations                    *
*                                                  *
* Jorge Correa & Placid Ferreira                   *
* 2016                                             *
*                                                  *
****************************************************/

#include <iostream>
#include <string>

#include "exeparser_defs.h"  // error handler definitions, constants and macros
#include "atomic_actions.h"  // atomic_actions type, defs and methods
#include "machine_vector.h"  // machine_vector type and defs
#include "modal_vector.h"    // modal_vector type, defs and methods 
#include "h_variables.h"     // h_variables type and defs

// create exeparse_defs.h for constants and error handlers defs.
# define LONG_STRING_SIZE 128

/**************************************************************
Lex and Yacc extern variables
***************************************************************/

extern FILE *exein;						     // reading from pointer for lex (declared in lex.yy.c)
extern int   exelineno;						 // this gives the line number (declared in lex.yy.c) 
extern int   exenerrs;					     // this gives the number of syntax errors (declared in lex.yy.c) 
extern char *exetext;						 // this gives the input test to the scanner

/**************************************************************
Lex and Yacc prototypes
***************************************************************/

extern int  exeparse();
extern void exescan_string(char *str);
extern void exedelete_buffer(void);
extern void exerestart(FILE *input_file);
extern int  exelex(void);
void		exeerror(char *s);                // lex requires the definition of this function

/**************************************************************
Exeparser Object
***************************************************************/

class Exeparser
{
public:

	/**************************************************************
	Executive parser public services
	***************************************************************/
	static Exeparser *Exeparser::initExeparser(H_variables *variables, Modal_vector *modal, Machine_vector *machine,
		Atomic_action **head, Atomic_action **tail);

	// set commands
	static void       Exeparser::loadConfigFileAction(char* filename);
	static void       Exeparser::loadProgramFileAction(char* filename);
	static void		  Exeparser::setActionsLogAction(char* stat);
	static void       Exeparser::setProgramAction(char* filename);
	static void		  Exeparser::setFeedModeAction(char* mode);
	static void       Exeparser::setPlaneAction(char* plane);
	static void		  Exeparser::setLengthUnitsAction(char* units);
	static void       Exeparser::setRadiusCompensationAction(char *mode);
	static void       Exeparser::setLengthCompensationAction(char *mode);
	static void       Exeparser::setRetractModeAction(char *mode);
	static void		  Exeparser::setCoordinateSystemAction(int code);
	static void       Exeparser::setPathModeAction(char* mode);
	static void       Exeparser::setDistanceModeAction(char* mode);
	static void       Exeparser::setMotionModeAction(char* mode);	
	static void       Exeparser::setHvariableAction(int var, float value);
	static void		  Exeparser::setBLUmmAction(float val);
	static void		  Exeparser::setBLUinAction(float val);
	static void       Exeparser::setArcTolAction(int val);
	static void       Exeparser::setInterpoMaxFrqAction(int val);
	// static void Exeparser::interpoFeedReg(int val);
	static void		  Exeparser::setInterpoStatusAction(char* stat);
	static void       Exeparser::setInterpoPortAction(int port);
	static void       Exeparser::setInterpoBaudAction(int baud);
	static void		  Exeparser::setInterpoLoggingAction(char* mode);
	static void		  Exeparser::setInterpoBurstAction(char* mode);
	static void		  Exeparser::setVirtualmacStatAction(char *stat);
	static void       Exeparser::setCloudNCStatAction(char *stat);
	static void       Exeparser::setExecutiveStatAction(char *stat);

	// get commands
	static void		  Exeparser::getStateVector(void);
	static void	      Exeparser::getAtomicActions(void);
	static void       Exeparser::help(void);
	static void		  Exeparser::help_start(void);
	static void		  Exeparser::help_h(void);
	static void		  Exeparser::help_g(void);
	static void		  Exeparser::help_cloudnc(void);

	// lex and yacc wrappers
	static inline void scanString(char *str) { exescan_string(str); }
	static inline void deleteBuffer(void) { exedelete_buffer(); }
	static inline void parse(void) { exeparse(); }

	/**************************************************************
	getters
	***************************************************************/
	static inline float getHvariable(int var){ return h_variables->H[var];}
	static inline Atomic_action **getActionIn() { return action_in; }
	static inline Atomic_action **getActionOut() { return action_out; }

	/**************************************************************
	setters
	***************************************************************/
	static inline void resetLineNumber(void) { exelineno = 1;}

	// executive parser virtual distructor
	virtual ~Exeparser();

private:

	// private constructor 
	Exeparser() {}

	/**************************************************************
	Executive parser private Services
	***************************************************************/
	static void		  Exeparser::error(int num);

	/**************************************************************
	Executive parser private members
	***************************************************************/
	static Exeparser	  *exeparser;           // a pointer to the singleton object 
	static H_variables    *h_variables;
	static Modal_vector   *modal_vector;
	static Machine_vector *machine_vector;
	static Atomic_action  **action_in;
	static Atomic_action  **action_out;
};

