/**
* implementation for Scope
*/

#include "Scope.h"

/**
* Function to add a scope as a subscope of another
*/
void addSubScope(Scope * parent, Scope * child)
{
    /* ensure both exist and are not the same */
    if ((parent && child) && parent != child)
    {
        /* if the parent has room */
        if (parent->numSubScopes < MAXSUBSCOPES)
        {
            /* add the subscope*/
            parent->subScopes[parent->numSubScopes] = child;
            parent->numSubScopes++;
            
            /* set the parent pointer */
            child->parent = parent;
        }
        else
        {
            /* panic if the parent doesnt have room */
            fprintf(stderr, "Too many subscopes!\n");
            exit(0);
        }
    }
    else
    {
        /* panic if invalid scopes are provided */
        fprintf(error, "Attempt to add an invalid subscope.\n");
        exit(0);
    }
}

/**
* Scope constructor
*/
Scope * newScope(string * name, Scope * parent)
{
    /* initialize variables */
    int i = 0;
    char pseudoName[MAXTOKENLEN];
    Scope * s = NULL;
    
    /* allocate memory */
    s = (Scope *) malloc(sizeof(Scope));
    
    /* if a name is provided, this is a function call. */
    if (name)
    {
        /* set the name */
        s->name = name;
        
        /* set the return scope level */
        s->returnScopes = 0;
    }
    
    /* handle compound statements */
    else
    {
        /* grab the parent's name */
        s->name = new string(*parent->name);
        
        /* add a level of return scoping */
        s->returnScopes = parent->returnScopes + 1;
        
        /* append a + to the name */
        s->name->append("+");
    }
    
    /* initialize the subscopes */
    for (i=0; i < MAXSUBSCOPES; i++)
        s->subScopes[i] = NULL;
    s->numSubScopes = 0;
    
    /* make a new symbol table */
    s->symbols = newSymbolTable();
    
    /* set or initialize the parent pointer */
    if (parent)
        addSubScope(parent, s);
    else
        s->parent = NULL;
    
    /* return pointer to the new scope */
    return s;
}

/**
* Scope destructor
*/
Scope * destroyScope(Scope * s)
{
    /* initialize variable */
    int i = 0;
    
    /* if the scope exists */
    if (s)
    {
        /* destroy subscopes if any */
        if (s->numSubScopes)
            for(i=0; i < s->numSubScopes; i++)
                s->subScopes[i] = destroyScope(s->subScopes[i]);
            
            /* clear parent pointer */
            s->parent = NULL;
        
        /* destroy the symbol table */
        destroySymbolTable(s->symbols);
        
        /* delete objects */
        delete s->name;
        s->name = NULL;
        
        /* free the memory */
        free(s);
    }
    
    return NULL;
}

/**
* Function to get a scope by it's name
*/
Scope * getScopeByName(string * name, Scope * scope)
{
    /* initialize variable */
    int i = 0;
    
    /* iterate through the scopes*/
    for(i=0; i < scope->numSubScopes; i++)
    {
        /* return the scope if it's name is the one we're looking for */
        if (!name->compare(*scope->subScopes[i]->name))
        {
            return scope->subScopes[i];
        }
    }
    
    /* if not found, return null */
    return NULL;
}

/**
* Function to add a symbol to a scope
*/
void addSymbol(TreeNode * typeSpec, string * name, Scope * scope, Prototype * proto, int size)
{
    /* get the number of symbols already in the table */
    int i = scope->symbols->numSymbols;
    
    /* add this symbol to the table */
    scope->symbols->identifiers[i] = name;
    
    /* save the type */
    if (typeSpec->type.tType == intToken)
        scope->symbols->types[i] = intType;
    else
        scope->symbols->types[i] = voidType;
    
    /* save the prototype, if any */
    if (proto)
    {
        scope->symbols->functions[i] = proto;
        scope->symbols->offsets[i] = 0;
    }
    else
    {
        scope->symbols->functions[i] = NULL;
        scope->symbols->offsets[i] = scope->symbols->nextOffset;
        scope->symbols->nextOffset--;
    }
    
    /* save the array size */
    scope->symbols->arraySizes[i] = size;
    
    /* increment the counter */
    scope->symbols->numSymbols++;
}

/**
* Function to match a symbol within a scope
*/
int matchSymbol(string * sym, Scope * scope)
{
    /* initialize variable */
    int i = 0;
    
    /* if this scope exists */
    if (scope)
    {
        /* return true if the symbol is in this scope's table */
        for (i=0; i< scope->symbols->numSymbols; i++)
            if (!sym->compare(*scope->symbols->identifiers[i])) return 1;
            
            /* if not, try one level higher */
            if (matchSymbol(sym, scope->parent)) return 1;
    }
    
    /* return false if we've exhausted all scopes */
    return 0;
}
