/***************************************************
 * easyMAC Interpreter 1.0                         *
 * Lexical scanner      				           *
 *                                                 *
 * Jorge Correa & Placid Ferreira                  *
 * 2016                                            *
 *                                                 *
 ***************************************************/

/*
 * This instance of the program contais the Interpreters's lexical definitions (token 
 * identifications and return values). 
 *
 * yylval  : is a union declared in bison to store different data types passed from the
 * scanner to the parser. 
 * yytex   : yytext is an array of characters, which acts as a buffer for the input currently being parsed.
 * yylex   : returns the value of the token currently being parsed as an integer. 
 * yylineno: this gives the line number for input currently being parsed
 */

 /* 
 
 // generic type string
 
 [a-zA-Z]+   			                {char *res = new char[strlen(yytext) + 1];
 strcpy(res, yytext);
 yylval.sval = res;
 return STRING;}
 */

%{
	
#include "yy.tab.hpp"
#include "interpreter.h"

#ifdef _WIN32
    #define YY_NO_UNISTD_H 1  // Prevents <unistd.h> from being included
	#include <io.h>           // Windows equivalent for isatty
#endif

static void comment(void);

%}

/* Add this to get line numbers...(only in windows) */
%option yylineno

%%
"/*"                                    { comment(); }
"//".*                                  { /* consume //-comment */ }
"(".*                                   { /* consume //-comment */ }

[N]			           					{yylval.id = yytext[0]; return T_N;} 
[G]			          				    {yylval.id = yytext[0]; return T_G;}
[X]			           					{yylval.id = yytext[0]; return T_X;}
[Y]			           					{yylval.id = yytext[0]; return T_Y;}
[Z]			           					{yylval.id = yytext[0]; return T_Z;}
[I]			           					{yylval.id = yytext[0]; return T_I;}
[J]			          					{yylval.id = yytext[0]; return T_J;}
[K]			           					{yylval.id = yytext[0]; return T_K;}
[F]			           					{yylval.id = yytext[0]; return T_F;}
[R]										{yylval.id = yytext[0]; return T_R;}
[L]			           					{yylval.id = yytext[0]; return T_L;}
[Q]										{yylval.id = yytext[0]; return T_Q;}
[P]										{yylval.id = yytext[0]; return T_P;}
[S]			           					{yylval.id = yytext[0]; return T_S;}
[T]			           					{yylval.id = yytext[0]; return T_T;}
[M]			           					{yylval.id = yytext[0]; return T_M;}
[H]			           					{yylval.id = yytext[0]; return T_H;} 

[0-9]+\.[0-9]+ 				            {yylval.fnum = atof(yytext); return FNUMBER;} 
[0-9]+                 					{yylval.num  = atoi(yytext); return NUMBER;}
[a-zA-Z0-9_]+\.[a-zA-Z]+   			    {// we have to copy because we can't rely on yytext not changing underneath us:
										 char *res = new char[strlen(yytext) + 1];
										 strcpy(res, yytext);
										 yylval.sval = res;
										 return FILENAME;}
[ \t%]                  				; 
[\n]									{return '\n';}
[\*]									{return '*';}
[-+=]	                   				{return yytext[0];}
[a-zA-Z]*							    {ECHO; yyerror(" unexpected command");}
.                      				    {ECHO; yyerror(" unexpected character");}
%%

int yywrap (void) {return 1;}

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
	yyerror("unterminated comment");
}

void scan_string(char *str)
{
    yy_scan_string(str);
}

void delete_buffer(void)
{
    yy_delete_buffer(YY_CURRENT_BUFFER);
}
