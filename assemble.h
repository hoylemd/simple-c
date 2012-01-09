/**
 * header file for assembler library
 */

#ifndef _ASSEMBLE_H_
#define _ASSEMBLE_H_

#include <string>
#include <sstream>

/* local includes */
#include "common.h"
#include "AssemblyCode.h"

/* define register shorthands */
#define AC 0
#define AC1 1
#define OP1 2
#define OP2 3
#define RES 4
#define PC 7
#define GP 6
#define FP 5
#define LABELSIZE 128

using namespace std;

/* pointers to the scope variables */
extern Scope * global;
extern Scope * currentScope;
extern Scope * scopeStack[64];
extern int scopeCount;

/* pointer to the AST */
extern TreeNode * AST;

/* scope stack management */
void pushScope(Scope * scope);
Scope * popScope();

/* utility functions */
int isNumeric(char * str, int len);

/* expression evaluation */
AssemblyCode * evaluateExpression(TreeNode *, Scope *);

/* compilation */
AssemblyCode * compileAST(TreeNode *, Scope *, int);

#endif