/**
 * header for symbol tables
 */

#ifndef _SYMBOLTABLE_H_
#define _SYMBOLTABLE_H_

#define MAXSYMBOLS 1024

#include "globals.h"
#include "Prototype.h"

/**
* Structure for a symbol table
*/
typedef struct symbolTable
{
    /* the number of symbols (rows filled)*/
    int numSymbols;

    /* symbol token string column */
    string * identifiers[MAXSYMBOLS];

    /* symbol type column */
    VarKind types[MAXSYMBOLS];

    /* prototype column */
    Prototype * functions[MAXSYMBOLS];

    /* column for the array degree of the symbol */
    /* 0 means it's an array placeholder, 1 means it's a regular variable, 1+ means an array*/
    int arraySizes[MAXSYMBOLS];

    /* offset column*/
    int offsets[MAXSYMBOLS];

    /* line number column*/
    int lineNo[MAXSYMBOLS];

    /* the offset for the next symbol */
    int nextOffset;
} SymbolTable;

/* Symbol Table functions */
/* constructor/Destructor */
SymbolTable * newSymbolTable();
SymbolTable * destroySymbolTable(SymbolTable *);

/* manipulators */
int indexOfSymbol(string *, SymbolTable *);
int lookupOffset(string*, SymbolTable *);
/* printer */
void printSymbols(SymbolTable *);

#endif

