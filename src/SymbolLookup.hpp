/*
 * File:   SymbolLookup.hpp
 * Author: hoylemd
 *
 * Declaration header for SymbolLookup class
 *
 * Created on January 20, 2012, 7:53 PM
 */

#ifndef SYMBOLLOOKUP_HPP
#define	SYMBOLLOOKUP_HPP

#include <string>

#include "Scope.h"

using namespace std;

/**
 * Symbol lookup class
 * Holds data representing the results of a symbol lookup
 */
class SymbolLookup{
private:
    /* fields */
    int scopeUps;
    int offset;
    Scope * scope;
    Scope * initialScope;
    string * symbol;


public:
    /* constructors */
    SymbolLookup( string *, Scope *);

    /* deleter*/
    ~SymbolLookup();

    /* field accessors */
    int getScopeUps();
    int getOffset();
    Scope * getScope();
    Scope * getInitialScope();
    string * getSymbol();
};

#endif	/* SYMBOLLOOKUP_HPP */

