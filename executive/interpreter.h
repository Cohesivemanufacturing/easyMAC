#pragma once
/**************************************************
* easyMac Interpreter 1.0 				          *
* Utility function declarations                   *
*                                                 *
* Jorge Correa & Placid Ferreira                  *
* 2016                                            *
*                                                 *
***************************************************/

#include <iostream>
#include <string>
#include <math.h>               // atof
#include <array>                // std::array

#include "parson.h"             // deprecated

#include "interpreter_defs.h"   // error handler definitions, constants and macros
#include "atomic_actions.h"     // atomic_actions type, defs and methods
#include "machine_vector.h"     // machine_vector type and defs
#include "h_variables.h"        // h_variables type and defs
#include "modal_vector.h"       // modal_vector type, defs and methods
#include "block_vector.h"       // block_vector type and defs

typedef std::array<std::array<float, 4>, 4> mat4;

/**************************************************************
Lex and Yacc extern variables
***************************************************************/

/* Lex and Yacc (global variables of the non-reentrant parser) */

extern FILE *yyin; 			    // reading from pointer for lex (declared in lex.yy.c)
extern int   yylineno;		    // this gives the line number (declared in lex.yy.c) 
extern int   yynerrs;           // this gives the number of syntax errors (declared in lex.yy.c) 
extern char *yytext;		    // this gives the input test to the scanner

/**************************************************************
Lex and Yacc prototypes
***************************************************************/

extern int  yyparse();
extern void scan_string(char *str);
extern void delete_buffer(void);
extern void yyrestart(FILE *input_file);
extern int  yylex(void);
void        yyerror(char *s);

/**************************************************************
interpreter Object
***************************************************************/

class Interpreter
{
public:

	/**************************************************************
	Interpreter services
	***************************************************************/
	static Interpreter *Interpreter::initInterpreter(H_variables *variables, Block_vector *block, Modal_vector *modal, 
		Machine_vector *machine,Atomic_action **head, Atomic_action **tail);
	static void			Interpreter::processBlock(void);

	// lex and yacc wrappers
	static inline void scanString(char *str) { scan_string(str); }
	static inline void deleteBuffer(void) { delete_buffer(); }
	static inline void parse(void) { yyparse(); }

	/**************************************************************
	getters
	***************************************************************/
	static inline float           getHvariable(int var) { return h_variables->H[var]; }
	static inline Modal_vector   *getModalVector(void) { return modal_vector; }
	static inline Block_vector   *getBlockVector(void) { return block_vector; }
	static inline Machine_vector *getMachineVector(void) { return machine_vector; }
	static inline Atomic_action **getActionIn() { return action_in; }
	static inline Atomic_action **getActionOut() { return action_out; }
	static inline queue          *getBlockGcodes(void) { return &block_vector->G; }
	static inline queue          *getBlockMcodes(void) { return &block_vector->M; }
	static void			          getIncrements(axis *increment);
	static axis			          getTargetPosition(void);                              // make getPosition(axis* position)                                   
	// static void                getOrignOffsets(axis* offests);
	static inline float           getMachineBLU(void)
	{
		if (machine_vector->units == MM) {
			return h_variables->H[BLU_MM];
		}
		else {
			return h_variables->H[BLU_IN];
		}
	}
	static inline bool            checksEnabled(void) { return  m_checksEnabled; }

	/**************************************************************
	setters
	***************************************************************/
	static inline void resetBlockVector(void){ *block_vector = Block_vector();}   // deprecated block_vector_object
	static inline void setInput(FILE *input) 
	{
		yyin = input;
		yyrestart(yyin);
	}
	static inline void enableChecks() { m_checksEnabled = true; }
	static inline void disableChecks() { m_checksEnabled = false; }
	static inline void resetLineNumber(void) { yylineno = 1; }

	// Interpreter virtual distructor
	virtual ~Interpreter();

private:
	
	// private constructor  
	Interpreter() {}

	/**************************************************************
	Interpreter private members
	***************************************************************/
	static Interpreter	  *interpreter;      // a pointer to the singleton object 
	static H_variables    *h_variables;      // executive H variables 
	static Modal_vector   *modal_vector;     // interpreter modal vector 
	static Machine_vector *machine_vector;   // executive machine_vector
	static Atomic_action  **action_in;       // 
	static Atomic_action  **action_out;
	static Block_vector   *block_vector;     // interprter block vector 

	// Interpreter state variables
	static volatile bool   m_checksEnabled;

	/**************************************************************
	Interpreter private Services
	***************************************************************/
	static void           Interpreter::generateActions(void);
	static Atomic_action *Interpreter::setOriginOffset(Atomic_action *node);                        // uses getOriginOffsets()
	static Atomic_action *Interpreter::setIncrements(Atomic_action *node);						    // uses getIncrements() 
	static Atomic_action *Interpreter::calculateOffsets(Atomic_action *node, struct offset origin); // make a public getter Offset getOriginOffsets (){}                   
	static void			  Interpreter::generateArcs(void);
	static int			  Interpreter::initialQuadrant(float I, float J);
	static int			  Interpreter::finalQuadrant(float Xf, float Yf);
	static int			  Interpreter::numberOfQuadrants(int quad0, int quadf, float xo, float yo, float xf, float yf);
	static bool			  Interpreter::classifyPoints(float xo, float yo, float xf, float yf);
	static mat4			  Interpreter::getStateArray(float R);
	static bool			  Interpreter::checkArcFeed(Atomic_action *node);                           // use Struct axis() instead of the atomic action
	static void			  Interpreter::checkBlockVector(void);    
	static void			  Interpreter::checkValidCodes(queue *q);
	static void			  Interpreter::checkRepeatedCodes(queue *q);
	static void			  Interpreter::checkRepeatedModals(queue *q);
	static void			  Interpreter::checkCodeErrors(void);
	static bool			  Interpreter::newDimensions(void);
	static void			  Interpreter::error(int num);
	static void			  Interpreter::error(int num, int va);
	static void			  Interpreter::error(int num, int val1, int val2);
};

/**************************************************************
Utility Functions  outside the class
***************************************************************/

void print_interpreter_variable(int var);  // serialize method in H_variables object
void print_modal_vector(void);             // serialize method in Modal_vector object
void print_block_vector(void);             // serialize method in Block_vector object
void print_machine_motion_mode(void);
void arc_to_arcxy(axis *src, axis *dst);   // used by interpolator.cpp
void arcxy_to_arc(axis *src, axis *dst);





