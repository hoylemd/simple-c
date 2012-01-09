/**
 * Header file for the giant main code file
 */

#ifndef _MAIN_H_
#define _MAIN_H_

#include "assemble.h"

#define DEBUGMODE 1

/* declaration of parsing fields */
char tokenString[MAXTOKENLEN];
int lineno = 0;

/* io streams */
FILE * source = NULL;
FILE * out = NULL;
FILE * error = NULL;

/* flag for if the AST or symbol table will be printed */
int PrintTreeFlag = 0;
int PrintTableFlag = 0;

/* pointers to the scope variables */
Scope * scopeStack[64];
int scopeCount = 0;

/* global scopes */
Scope * global = NULL;
Scope * currentScope = NULL;

/* pointer to the AST */
TreeNode * AST = NULL;

/* error flag */
int errorEncountered = 0;

/* scope stack function prototypes */
void pushScope(Scope *);
Scope * popScope();

#endif