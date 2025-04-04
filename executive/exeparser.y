/***************************************************
 * easyMAC. executive parser 1.0                   *
 * Parser               				           *
 *                                                 *
 * Jorge Correa & Placid Ferreira                  *
 * 2016                                            *
 *                                                 *
 ***************************************************/
 
/*
 * This instance of the program contais the Interpreters's- parser grammar deinitions 
 * and the actions assosiated terminals. 
 *
 * References:
 * 1. Example of a calculator in Yacc. This example illustrates how to return pointer 
 *    form functions and use those pointers inside the productions actions. 
 *    http://epaperpress.com/lexandyacc/calcy.html
 */
%{
							   /* C declarations used in actions */ 
#include "exeparser.h"		  					   

%}
									  /* Yacc definitions */

%name-prefix "exe"
									  
/* lexical analyser yylval types */ 
%union 
{
	int   num; 
	float fnum; 
	char  id;
	char  *sval;
}  

/* starting production */ 	
%start block			

/* non-terminals */
%type <num>  block
%type <id>   systemcommand
%type <id>   setcommand 
%type <id>   getcommand
%type <fnum> expression

/* terminals of the type yylval */    
%token <id>   T_H
%token <id>   T_h
%token <id>   T_g
%token <id>   T_s
%token <sval> FILENAME 
%token <sval> STRING
%token <num>  NUMBER
%token <fnum> FNUMBER

/* system terminals */
%token HELP
%token EXIT
%token CLS

/* set terminals */ 
%token SET
%token PROGRAM 
%token CONFIGURATION 
%token ACTIONS_LOG
%token HISTORY_LOG
%token MOTION_MODE 
%token MOTION_PLANE 
%token DISTANCE_MODE 
%token FEED_MODE 
%token LENGTH_UNITS 
%token TOOL_RADIUS_COMP 
%token TOOL_LENGTH_COMP
%token RETRACT_MODE 
%token COORDINATE_SYSTEM 
%token PATH_MODE COOLANT 
%token MAX_FEED_OVERRIDE 
%token STEPS_PER_MM 
%token STEPS_PER_IN 
%token ARC_TOLERANCE
%token INTERPOLATOR 
%token STATUS
%token BAUD 
%token MAX_FRQ
%token COMPORT 
%token LOGGING 
%token BURST 
%token VIRTUALMAC 
%token CLOUDNC
%token EXECUTIVE

/* get terminals */
%token GET
%token MACHINE_VECTOR
%token BLOCK_VECTOR 
%token MODAL_VECTOR 
%token ATOMIC_ACTIONS 
%token ACTIONS_HISTORY

%%

/* descriptions of the syntactic rules: */
/*
*  1. block: print '/n' 
*     This rule implies that the command "print" by itself is not a valid sintax.   
*  2. block : print 
	        | block '/n' (left recursion is recommended in yacc as the blocks are processes
			              on a FIFO basis. A right recurtion stores the entire block
						  in the stack to process the block on a FILO bases)
*	  This rule implies that in the command "print \n", "print" is the first token identified and 
*     assigned to block  and the token "\n" is identified in a second itertion of block.
*     If a second command "print \n" is input, the previos command is identified as 
*     block in the first iteration, then the tocken "print" is identified and assigned to 
*	  block in the seconnd iteration and finally, the tokecn "\n" is identified in a third iteration
* notes :
* 1. there is no need for the empty character or the recursion rule. Seldomnly a block is empty or more 
*    one is specified at a time
* 2. a right recursion can be used to emmit the ready message in the empty rule (the buffer is 
*    empty at the end of the stack). However, the entired program is stored in the stack as a block 
*    causeing memory issues 
*/

block 		   : /* empty */              		  {;}
			   | block SET setcommand '\n'        {;}
			   | block GET getcommand '\n'		  {;} 
			   | block systemcommand '\n'		  {;}
			   | block '\n'                       {;}
			   ;		

systemcommand  : HELP                             {Exeparser::help();}
			   | HELP T_h                         {Exeparser::help_h();}
               | HELP T_g                         {Exeparser::help_g();}
			   | HELP CLOUDNC					  {Exeparser::help_cloudnc();}
			   | HELP T_s					      {Exeparser::help_start();}
 			   | CLS							  {system("@cls||clear");}
			   | EXIT							  {Exeparser::setExecutiveStatAction("disable"); 
												   return 0;} // return causes the program to exit yyparse() to the main loop, 
			   ;									          // alternativelly use: {exit(EXIT_SUCCESS);} to exit execution  

setcommand     : PROGRAM			  FILENAME	  {Exeparser::loadProgramFileAction($2);}
			   | CONFIGURATION		  FILENAME	  {Exeparser::loadConfigFileAction($2);}
			   | ACTIONS_LOG          STRING	  {Exeparser::setActionsLogAction($2);}
			   | HISTORY_LOG          STRING	  {Exeparser::setHistoryLogAction($2);}
			   | PROGRAM			  STRING      {Exeparser::setProgramAction($2);}
			   | T_H NUMBER  '='	  expression  {Exeparser::setHvariableAction($2, $4);}
			   | MOTION_MODE		  STRING	  {Exeparser::setMotionModeAction($2);}
			   | MOTION_PLANE		  STRING	  {Exeparser::setPlaneAction($2);}
			   | DISTANCE_MODE		  STRING	  {Exeparser::setDistanceModeAction($2);}
			   | FEED_MODE			  STRING	  {Exeparser::setFeedModeAction($2);}
			   | LENGTH_UNITS		  STRING      {Exeparser::setLengthUnitsAction($2);}
			   | TOOL_RADIUS_COMP	  STRING      {Exeparser::setRadiusCompensationAction($2);}
			   | TOOL_LENGTH_COMP	  STRING      {Exeparser::setLengthCompensationAction($2);}
			   | RETRACT_MODE		  STRING      {Exeparser::setRetractModeAction($2);}
			   | COORDINATE_SYSTEM	  NUMBER	  {Exeparser::setCoordinateSystemAction($2);}
			   | PATH_MODE			  STRING	  {Exeparser::setPathModeAction($2);}
			   | COOLANT			  STRING      {;}
			   | MAX_FEED_OVERRIDE	  expression  {;}
			   | STEPS_PER_MM		  NUMBER      {Exeparser::setBLUmmAction($2);}
			   | STEPS_PER_IN		  NUMBER      {Exeparser::setBLUinAction($2);}
			   | ARC_TOLERANCE	      NUMBER      {Exeparser::setArcTolAction($2);}
			   | INTERPOLATOR         STRING	  {Exeparser::setInterpoStatusAction($2);}
			   | INTERPOLATOR COMPORT NUMBER	  {Exeparser::setInterpoPortAction($3);}
			   | INTERPOLATOR BAUD    NUMBER	  {Exeparser::setInterpoBaudAction($3);}
			   | INTERPOLATOR MAX_FRQ NUMBER      {Exeparser::setInterpoMaxFrqAction($3);}
			   | INTERPOLATOR LOGGING STRING      {Exeparser::setInterpoLoggingAction($3);}
			   | INTERPOLATOR BURST   STRING      {Exeparser::setInterpoBurstAction($3);} 
			   | VIRTUALMAC           STRING      {Exeparser::setVirtualmacStatAction($2);}
			   | CLOUDNC              STRING	  {Exeparser::setCloudNCStatAction($2);}
               | EXECUTIVE            STRING      {Exeparser::setExecutiveStatAction($2);}
			   ;

// must include querys for the elements of the machine state vector and the corresponding potting functions
getcommand     : MACHINE_VECTOR					  {Exeparser::getStateVector();}
			   | BLOCK_VECTOR				      {;}
			   | MODAL_VECTOR					  {;}
               | ATOMIC_ACTIONS                   {Exeparser::getAtomicActions();}
			   | ACTIONS_HISTORY                  {;}
			   | T_H '*'                          {;}
			   | T_H NUMBER						  {;}
			   | MOTION_MODE					  {;}
			   | ARC_TOLERANCE					  {;}
			   ;

expression     : FNUMBER                          {$$ =  $1;}
			   | '+' FNUMBER					  {$$ =  $2;}
			   | '-' FNUMBER					  {$$ = -$2;} 
			   | NUMBER                           {$$ =  $1;} 
			   | '+' NUMBER					      {$$ =  $2;}
			   | '-' NUMBER					      {$$ = -$2;}
			   | T_H NUMBER                       {$$ = Exeparser::getHvariable($2);}
			   ;

%% 	
			  
/* C code */


 
