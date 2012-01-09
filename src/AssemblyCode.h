/**
 *  header for AssemblyCode
 */

#ifndef _ASSEMBLYCODE_H_
#define _ASSEMBLYCODE_H_

/* local includes */
#include "globals.h"
#include "AssemblyChunk.h"

typedef struct assemblyCode
{
    /* chunk list pointers */
    AssemblyChunk * first;
    AssemblyChunk * last;
    
    /* counter for chunks */
    int chunkCount;
} AssemblyCode;

/* Constructor */
AssemblyCode * newAssemblyCode();

/* Destructor */
AssemblyCode * disconnectCode(AssemblyCode *);

/* Mutators */
AssemblyCode * appendChunk(AssemblyCode *, AssemblyChunk *);
AssemblyCode * appendCode(AssemblyCode *, AssemblyCode *);

/* Utility */
int instructionsInAssembly(AssemblyCode *);

/* Printer */
void printCode(FILE *, AssemblyCode *);

#endif
