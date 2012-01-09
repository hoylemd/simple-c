#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define MAXTOKENLEN 64

/* declaration of parsing fields */
extern char tokenString[MAXTOKENLEN];
extern int lineno;

/* io streams */
extern FILE * source;
extern FILE * out;
extern FILE * error;

/* flag for if the AST or symbol table will be printed */
extern int PrintTreeFlag;
extern int PrintTableFlag;

/* prototype for lex function */
int getToken();

/* prototype for yacc function */
int yyparse();

/* pointer to the error flag*/
extern int errorEncountered;

#endif