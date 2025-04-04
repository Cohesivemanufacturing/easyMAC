/***************************************************
 * easyMAC. Intepreter 1.0                         *
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
#include "interpreter.h"

%}
									  /* Yacc definitions */
									  
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
%type <id>   RS274_block
%type <num>  block seq_word 
%type <num>  prep_sub_block 
%type <num>  dim_sub_block 
%type <num>  feed_sub_block 
%type <num>  speed_sub_block 
%type <num>  tool_sub_block
%type <num>  misc_sub_block
%type <num>  set_sub_block
%type <num>  dim_sub_block_X dim_sub_block_Y dim_sub_block_Z
%type <num>  dim_sub_block_I dim_sub_block_J dim_sub_block_K
%type <num>  dim_sub_block_R dim_sub_block_L dim_sub_block_Q
%type <num>  dim_sub_block_P
%type <fnum> expression

/* terminals of the type yylval */    
%token <id>   T_N T_G T_X T_Y T_Z T_I T_J T_K T_F T_R T_L T_Q T_P T_S T_T T_M T_H 
%token <sval> FILENAME 
%token <sval> STRING
%token <num>  NUMBER
%token <fnum> FNUMBER

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
			   | block RS274_block '\n' 		  {;}
			 //| block '\n'
			   ;		

// inforces the order (seq_word)(prep_sub_block)..etc
RS274_block    : /* empty */					  
			   | seq_word prep_sub_block  
				 dim_sub_block feed_sub_block
				 speed_sub_block tool_sub_block   
			     misc_sub_block					  {Interpreter::processBlock();}
			   ;

seq_word       : /* empty */				      {Interpreter::getBlockVector()->N = EMPTYCODE;}
               | T_N NUMBER                       {Interpreter::getBlockVector()->N = $2;}
		       ;

prep_sub_block : /* empty */
			   | prep_sub_block T_G NUMBER        {add_queue(Interpreter::getBlockGcodes(), $3);}
			   | prep_sub_block T_G FNUMBER       {;}
               | prep_sub_block T_G NUMBER 
				 set_sub_block                    {;}				  
	           ;

set_sub_block  : T_L NUMBER                       {;} // use in combination with G10 to set tool offset and CS tables
               ;			   

// this set of production rules inforces the order (dim block X)(dim block Y)..etc, 
// and allows a subblock to have repeated dimensions

dim_sub_block  :  /* empty */       
			   | dim_sub_block_X dim_sub_block_Y 
				 dim_sub_block_Z dim_sub_block_I dim_sub_block_J 
				 dim_sub_block_K dim_sub_block_R dim_sub_block_L 
				 dim_sub_block_Q dim_sub_block_P				
											      {;}
			   ;

// dimension words
dim_sub_block_X  : /* empty */                    {Interpreter::getBlockVector()->X = NAN;}
			     | T_X expression        	      {Interpreter::getBlockVector()->X = $2;}
			     ;

dim_sub_block_Y  : /* empty */				      {Interpreter::getBlockVector()->Y = NAN;}
				 | T_Y expression			      {Interpreter::getBlockVector()->Y = $2;}
				 ;

dim_sub_block_Z  : /* empty */				      {Interpreter::getBlockVector()->Z = NAN;}
				 | T_Z expression                 {Interpreter::getBlockVector()->Z = $2;}
				 ;

dim_sub_block_I  : /* empty */				      {Interpreter::getBlockVector()->I = NAN;}
				 | T_I expression			      {Interpreter::getBlockVector()->I = $2;}
				 ;

dim_sub_block_J  : /* empty */				      {Interpreter::getBlockVector()->J = NAN;}
				 | T_J expression			      {Interpreter::getBlockVector()->J = $2;}
				 ;

dim_sub_block_K  : /* empty */				      {Interpreter::getBlockVector()->K = NAN;}
				 | T_K expression			      {Interpreter::getBlockVector()->K = $2;}
				 ;

dim_sub_block_R  : /* empty */				      {Interpreter::getBlockVector()->R = NAN;}
				 | T_R expression                 {Interpreter::getBlockVector()->R = $2;}
				 ;

dim_sub_block_Q  : /* empty */				      {Interpreter::getBlockVector()->Q = NAN;}
				 | T_Q expression			      {Interpreter::getBlockVector()->Q = $2;}
				 ;

dim_sub_block_L  : /* empty */				      {Interpreter::getBlockVector()->L = EMPTYCODE;}
				 | T_L expression			      {Interpreter::getBlockVector()->L = $2;}
				 ;

dim_sub_block_P  : /* empty */				      {Interpreter::getBlockVector()->P = EMPTYCODE;}
				 | T_P expression			      {Interpreter::getBlockVector()->P = $2;}
				 ;

// feed_sub_block
feed_sub_block   : /* empty */				      {Interpreter::getBlockVector()->F = NAN;}
				 | T_F expression			      {Interpreter::getBlockVector()->F = $2;}
				 ;

// speed_sub_block
speed_sub_block  : /* empty */                    {Interpreter::getBlockVector()->S = NAN;}
				 | T_S expression                 {Interpreter::getBlockVector()->S = $2;}
				 ;

// tool_sub_block
tool_sub_block   : /* empty */				      {Interpreter::getBlockVector()->T = EMPTYCODE;}
				 |	T_T	expression			      {Interpreter::getBlockVector()->T = $2;}
				 ;

misc_sub_block   : /* empty */
			     | misc_sub_block T_M NUMBER      {;}		  
	             ;


expression       : FNUMBER                        {$$ =  $1;}
				 | '+' FNUMBER					  {$$ =  $2;}
				 | '-' FNUMBER					  {$$ = -$2;} 
			     | NUMBER                         {$$ =  $1;} 
				 | '+' NUMBER					  {$$ =  $2;}
				 | '-' NUMBER					  {$$ = -$2;}
			     | T_H NUMBER                     {$$ = Interpreter::getHvariable($2);}
			     ;
%% 	
			  
/* C code */


 
