/*
 * File:   SymbolLookup.cpp
 * Author: hoylemd
 *
 * Definition file for SymbolLookup class
 *
 * Created on January 20, 2012, 7:57 PM
 */

#include "SymbolLookup.hpp"

/**
 * Constructor for a lookup.
 * Also executes the lookup.
 * @param
 * @param is
 */
SymbolLookup::SymbolLookup(string * symbol, Scope * initialScope)
{
    /* Save parameters */
    this->symbol = new string(*symbol);
    this->initialScope = initialScope;

    /* initialize fields */
    this->scopeUps = 0;
    this->offset = 0;
    this->scope = NULL;

    /* look up the symbol */
        /* loop trace through the scopes to try to find the variable */
    this->scope = this->initialScope;

    while(!this->offset)
    {
        /* look up the identifier in the symbol table */
        this->offset = lookupOffset(this->symbol, this->scope->symbols);

        /* if it's not in the current scope */
        if (!this->offset)
        {
            /* move up to the parent scope, if any */
            if (this->scope->parent)
            {
                this->scope = this->scope->parent;
                this->scopeUps++;
            }
        }
    }
}

/**
 * deleter
 * @return
 */
SymbolLookup::~SymbolLookup()
{
    this->scope = NULL;
    this->initialScope = NULL;
    delete this->symbol;
    this->symbol = NULL;
}

/* field accessors */
int SymbolLookup::getScopeUps()
{
    return this->scopeUps;
}
int SymbolLookup::getOffset()
{
    return this->offset;
}
Scope * SymbolLookup::getScope()
{
    return this->scope;
}
Scope * SymbolLookup::getInitialScope()
{
    return this->initialScope;
}
string * SymbolLookup::getSymbol()
{
    return this->symbol;
}