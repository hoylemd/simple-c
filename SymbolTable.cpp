/**
 * implementation for SymbolTable
 */

#include "SymbolTable.h"

/**
* Symbol Table Constructor
*/
SymbolTable * newSymbolTable()
{
    /* allocate memory */
    SymbolTable * s = (SymbolTable *) malloc(sizeof(SymbolTable));
    
    /* initialize it */
    s-> numSymbols = 0;
    s-> nextOffset = -2;
    
    /* return the new table */
    return s;
}

/**
* Symbol Table Destructor
*/
SymbolTable * destroySymbolTable(SymbolTable * s)
{
    /* initialize variables */
    int i =0;
    
    /* if it exists*/
    if (s)
    {
        /* iterate through the rows */
        for(i=0; i < s->numSymbols; i++)
        {
            /* destroy prototypes */
            s->functions[i] = destroyPrototype(s->functions[i]);
        }
        
        /* free memory*/
        free(s);
    }
    
    return NULL;
}

/**
* Function to print out the contents of a symbol table
*/
void printSymbols(SymbolTable* symbols)
{
    /* initialize variables and pointer */
    int i= 0, j = 0;
    Prototype * proto = NULL;
    
    /* iterate and print */
    for(i=0; i< symbols->numSymbols; i++)
    {
        /* print the type and identifier */
        if (symbols->types[i] == intType)
            fprintf(error, "\tint %s", symbols->identifiers[i]->data());
        else
            fprintf(error, "\tvoid %s", symbols->identifiers[i]->data());
        
        /* print out the array notation, if any */
        if (symbols->arraySizes[i] > 1)
            fprintf(error, "[%d]", symbols->arraySizes[i]);
        else if (symbols->arraySizes[i] == 0)
            fprintf(error, "[]");
        
        /* print out the prototype, if any */
        if (proto = symbols->functions[i])
        {
            fprintf(error, "(");
            
            /* print out the argument list, delimited by commas. */
            for(j= 0; j < proto->numParams; j++)
            {
                if (j) fprintf(error, ", ");
                
                if (proto->params[j] == intType)
                    fprintf(error, "int");
                else
                    fprintf(error, "void");
            }
            fprintf(error, ")");
        }
        
        /* go to the next line */
        fprintf(error, "\n");
    }
}

/**
* Function to find the index of a symbol
*/
int indexOfSymbol(string *sym, SymbolTable * table)
{
    /* initialize variable */
    int i = 0;
    
    /* endure the table exists */
    if (table)
    {
        /* iterate through the list to find it */
        for(i = 0; i < table->numSymbols; i++)
            if (!sym->compare(*table->identifiers[i])) return i;
    }
    
    /* if it wasn't found, return -1 error */
    return -1;
}


/**
* Function to look up a variable's offset by symbol
*/
int lookupOffset(string * sym, SymbolTable * table)
{
    /* initialize variables */
    int i = 0;
    
    /* if the symbol table and symbol exists */
    if (table && sym)
    {
        /* iterate through the table to find the offset */
        for(i=0; i < table->numSymbols; i++)
            if (!sym->compare(*table->identifiers[i]))
                return table->offsets[i];
    }
    
    /* return 0 on failure */
    return 0;
}