/***************************************************
 * easyMAC Executive parser 1.0                    *
 * Lexical scanner      				           *
 *                                                 *
 * Jorge Correa & Placid Ferreira                  *
 * 2016                                            *
 *                                                 *
 ***************************************************/

/*
 * This instance of the program contais the executive's parser lexical definitions (token 
 * identifications and return values). 
 *
 * execlval  : is a union declared in bison to store different data types passed from the
 * 			   scanner to the parser. 
 * exectex   : exetext is an array of characters, which acts as a buffer for the input currently being parsed.
 * execlex   : returns the value of the token currently being parsed as an integer. 
 * execlineno: this gives the line number for input currently being parsed
 */

%{
	
#include "exe.tab.hpp"
#include "exeparser.h"   

#ifdef _WIN32
    #define YY_NO_UNISTD_H 1  // Prevents <unistd.h> from being included
	#include <io.h>           // Windows equivalent for isatty
#endif

static void comment(void);

%}

/* Add this to get line numbers...(only in windows) */
%option yylineno
%option prefix = "exe"

%%
"help"                                  {return HELP; }
"cls"			       					{return CLS; }
"exit"				   					{return EXIT; }

"set"									{return SET;}
"program"								{return PROGRAM;}
"configuration"						    {return CONFIGURATION;}
"actions_log"                           {return ACTIONS_LOG;}
"history_log"							{return HISTORY_LOG;}
"motion_mode"							{return MOTION_MODE;}
"motion_plane"							{return MOTION_PLANE;}
"distance_mode"							{return DISTANCE_MODE;}
"feed_mode"								{return FEED_MODE; }
"length_units"							{return LENGTH_UNITS;}
"tool_radius_comp"						{return TOOL_RADIUS_COMP;}
"tool_lenght_comp"						{return TOOL_LENGTH_COMP;}
"retract_mode"							{return RETRACT_MODE;}
"coordinate_system"						{return COORDINATE_SYSTEM;}
"path_mode"								{return PATH_MODE;}
"coolant"								{return COOLANT;}
"max_feed_override"						{return MAX_FEED_OVERRIDE;}
"steps_per_mm"							{return STEPS_PER_MM;}
"steps_per_in"							{return STEPS_PER_IN;}
"arc_tolerance"							{return ARC_TOLERANCE;}
"interpolator"						    {return INTERPOLATOR;}
"status"                                {return STATUS;}
"comport"						        {return COMPORT;}
"baud"						            {return BAUD;}
"max_frequency"                         {return MAX_FRQ;}
"logging"				            	{return LOGGING;}
"burst"									{return BURST;}
"virtualmac"				            {return VIRTUALMAC;}
"cloudnc"								{return CLOUDNC;}
"executive"								{return EXECUTIVE;}

"rapid"								    {/* could define a generic type STRING [a-zA-Z]+ with the disadvange 
										 of not been able to identify unexpected commands*/
										 exelval.sval = &exetext[0]; return STRING;}
"linear"								{exelval.sval = &exetext[0]; return STRING;}
"cw"								    {exelval.sval = &exetext[0]; return STRING;}
"ccw"								    {exelval.sval = &exetext[0]; return STRING;}
"dwell"								    {exelval.sval = &exetext[0]; return STRING;}
"xy"								    {exelval.sval = &exetext[0]; return STRING;}
"zx"								    {exelval.sval = &exetext[0]; return STRING;}
"yz"								    {exelval.sval = &exetext[0]; return STRING;}
"abs"								    {exelval.sval = &exetext[0]; return STRING;}
"rel"								    {exelval.sval = &exetext[0]; return STRING;}
"inverse_time"						    {exelval.sval = &exetext[0]; return STRING;}
"units_per_min"						    {exelval.sval = &exetext[0]; return STRING;}
"units_per_rev"							{exelval.sval = &exetext[0]; return STRING;}
"in"									{exelval.sval = &exetext[0]; return STRING;}
"mm"									{exelval.sval = &exetext[0]; return STRING;}
"tool_left"								{exelval.sval = &exetext[0]; return STRING;} 
"tool_right"							{exelval.sval = &exetext[0]; return STRING;}
"origin"						    	{exelval.sval = &exetext[0]; return STRING;}
"specified"						        {exelval.sval = &exetext[0]; return STRING;}
"exact"						            {exelval.sval = &exetext[0]; return STRING;}
"start"									{exelval.sval = &exetext[0]; return STRING;}
"pause"									{exelval.sval = &exetext[0]; return STRING;}
"stop"						            {exelval.sval = &exetext[0]; return STRING;}
"continue"								{exelval.sval = &exetext[0]; return STRING;}
"blending"						        {exelval.sval = &exetext[0]; return STRING;}
"enable"								{exelval.sval = &exetext[0]; return STRING;}
"disable"								{exelval.sval = &exetext[0]; return STRING;}
"receive"                               {exelval.sval = &exetext[0]; return STRING;}

"get"									{return GET; }
"machine_vector"					    {return MACHINE_VECTOR; }
"block_vector"                          {return BLOCK_VECTOR; }
"modal_vector"						    {return MODAL_VECTOR; }
"atomic_actions"						{return ATOMIC_ACTIONS; }
"actions_history"						{return ACTIONS_HISTORY; }

"/*"                                    {comment(); }
"//".*                                  { /* consume //-comment */ }
"(".*                                   { /* consume //-comment */ }
[H]			           					{exelval.id   = yytext[0]; return T_H;}
[h]										{exelval.id   = yytext[0]; return T_h;}
[g]										{exelval.id   = yytext[0]; return T_g;}
[s]										{exelval.id   = yytext[0]; return T_s;}

[0-9]+\.[0-9]+ 				            {exelval.fnum = atof(exetext); return FNUMBER;} 
[0-9]+                 					{exelval.num  = atoi(exetext); return NUMBER;}
[a-zA-Z0-9_]+\.[a-zA-Z]+   			    {// we have to copy because we can't rely on exetext not changing underneath us:
										 char *res = new char[strlen(exetext) + 1];
										 strcpy(res, exetext);
										 exelval.sval = res;
										 return FILENAME;}
[ \t%]                  				; 
[\n]									{return '\n';}
[\*]									{return '*';}
[-+=]	                   				{return exetext[0];}
[a-zA-Z]*							    {ECHO; exeerror(" unexpected command");}
.                      				    {ECHO; exeerror(" unexpected character");}
%%

int exewrap (void) {return 1;}

static void comment(void)
{
	int c;

	while ((c = yyinput()) != 0)
		if (c == '*')
		{
			while ((c = yyinput()) == '*')
				;

			if (c == '/')
				return;

			if (c == 0)
				break;
		}
	exeerror("unterminated comment");
}

/* 
 *yy_scan_string and yy_delete_buffer is defined as static in exe.lex.cc. 
 * therefore they cannot be defined extern in exeparser.h
 */

void exescan_string(char *str)
{
    exe_scan_string(str);    
}

void exedelete_buffer(void)
{
    exe_delete_buffer(YY_CURRENT_BUFFER);
}
