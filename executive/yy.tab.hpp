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

#ifndef YY_YY_YY_TAB_HPP_INCLUDED
# define YY_YY_YY_TAB_HPP_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     T_N = 258,
     T_G = 259,
     T_X = 260,
     T_Y = 261,
     T_Z = 262,
     T_I = 263,
     T_J = 264,
     T_K = 265,
     T_F = 266,
     T_R = 267,
     T_L = 268,
     T_Q = 269,
     T_P = 270,
     T_S = 271,
     T_T = 272,
     T_M = 273,
     T_H = 274,
     FILENAME = 275,
     STRING = 276,
     NUMBER = 277,
     FNUMBER = 278
   };
#endif
/* Tokens.  */
#define T_N 258
#define T_G 259
#define T_X 260
#define T_Y 261
#define T_Z 262
#define T_I 263
#define T_J 264
#define T_K 265
#define T_F 266
#define T_R 267
#define T_L 268
#define T_Q 269
#define T_P 270
#define T_S 271
#define T_T 272
#define T_M 273
#define T_H 274
#define FILENAME 275
#define STRING 276
#define NUMBER 277
#define FNUMBER 278



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 2058 of yacc.c  */
#line 28 "Interpreter.y"

	int   num; 
	float fnum; 
	char  id;
	char  *sval;


/* Line 2058 of yacc.c  */
#line 111 "yy.tab.hpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_YY_YY_TAB_HPP_INCLUDED  */
