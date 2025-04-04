/* A Bison parser, made by GNU Bison 2.7.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2012 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_EXE_EXE_TAB_HPP_INCLUDED
# define YY_EXE_EXE_TAB_HPP_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int exedebug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     T_H = 258,
     T_h = 259,
     T_g = 260,
     T_s = 261,
     FILENAME = 262,
     STRING = 263,
     NUMBER = 264,
     FNUMBER = 265,
     HELP = 266,
     EXIT = 267,
     CLS = 268,
     SET = 269,
     PROGRAM = 270,
     CONFIGURATION = 271,
     ACTIONS_LOG = 272,
     HISTORY_LOG = 273,
     MOTION_MODE = 274,
     MOTION_PLANE = 275,
     DISTANCE_MODE = 276,
     FEED_MODE = 277,
     LENGTH_UNITS = 278,
     TOOL_RADIUS_COMP = 279,
     TOOL_LENGTH_COMP = 280,
     RETRACT_MODE = 281,
     COORDINATE_SYSTEM = 282,
     PATH_MODE = 283,
     COOLANT = 284,
     MAX_FEED_OVERRIDE = 285,
     STEPS_PER_MM = 286,
     STEPS_PER_IN = 287,
     ARC_TOLERANCE = 288,
     INTERPOLATOR = 289,
     STATUS = 290,
     BAUD = 291,
     MAX_FRQ = 292,
     COMPORT = 293,
     LOGGING = 294,
     BURST = 295,
     VIRTUALMAC = 296,
     CLOUDNC = 297,
     EXECUTIVE = 298,
     GET = 299,
     MACHINE_VECTOR = 300,
     BLOCK_VECTOR = 301,
     MODAL_VECTOR = 302,
     ATOMIC_ACTIONS = 303,
     ACTIONS_HISTORY = 304
   };
#endif
/* Tokens.  */
#define T_H 258
#define T_h 259
#define T_g 260
#define T_s 261
#define FILENAME 262
#define STRING 263
#define NUMBER 264
#define FNUMBER 265
#define HELP 266
#define EXIT 267
#define CLS 268
#define SET 269
#define PROGRAM 270
#define CONFIGURATION 271
#define ACTIONS_LOG 272
#define HISTORY_LOG 273
#define MOTION_MODE 274
#define MOTION_PLANE 275
#define DISTANCE_MODE 276
#define FEED_MODE 277
#define LENGTH_UNITS 278
#define TOOL_RADIUS_COMP 279
#define TOOL_LENGTH_COMP 280
#define RETRACT_MODE 281
#define COORDINATE_SYSTEM 282
#define PATH_MODE 283
#define COOLANT 284
#define MAX_FEED_OVERRIDE 285
#define STEPS_PER_MM 286
#define STEPS_PER_IN 287
#define ARC_TOLERANCE 288
#define INTERPOLATOR 289
#define STATUS 290
#define BAUD 291
#define MAX_FRQ 292
#define COMPORT 293
#define LOGGING 294
#define BURST 295
#define VIRTUALMAC 296
#define CLOUDNC 297
#define EXECUTIVE 298
#define GET 299
#define MACHINE_VECTOR 300
#define BLOCK_VECTOR 301
#define MODAL_VECTOR 302
#define ATOMIC_ACTIONS 303
#define ACTIONS_HISTORY 304



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 2058 of yacc.c  */
#line 30 "exeparser.y"

	int   num; 
	float fnum; 
	char  id;
	char  *sval;


/* Line 2058 of yacc.c  */
#line 163 "exe.tab.hpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE exelval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int exeparse (void *YYPARSE_PARAM);
#else
int exeparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int exeparse (void);
#else
int exeparse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_EXE_EXE_TAB_HPP_INCLUDED  */
