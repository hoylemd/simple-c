/**
 * header for scopes
 */

#ifndef _SCOPE_H_
#define _SCOPE_H_

#include "SymbolTable.h"

#define MAXSUBSCOPES 32

/**
* Structure for a scope
*/
typedef struct scopeStruct
{
    /* the symbol table for this scope */
    SymbolTable * symbols;

    /* the name of this scope */
    string * name;

    /* pointers to the parent and child scopes */
    struct scopeStruct * parent;
    struct scopeStruct * subScopes[MAXSUBSCOPES];
    int numSubScopes;

    /* count of how many scopes lie between this one and the function */
    /* used for return statements inside nexted scopes */
    int returnScopes;
} Scope;

/* Scope functions */
/* Constructor/Destructor */
Scope * newScope(string *, Scope *);
Scope * destroyScope(Scope *);
Scope * getScopeByName(string *, Scope *);

/* Manipulators */
void addSubScope(Scope *, Scope *);
void addSymbol(TreeNode *, string *, Scope *, Prototype *, int);
int matchSymbol(string *, Scope *);

#endif
