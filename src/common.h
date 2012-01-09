#ifndef _COMMON_H_
#define _COMMON_H_

#include "globals.h"
#include "Scope.h"
#include "TreeNode.h"

/* global scopes */
extern Scope * global;
extern Scope * currentScope;

/* pointer to the AST */
extern TreeNode * AST;

#endif