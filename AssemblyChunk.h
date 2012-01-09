/**
 * header for Assembly Chunks
 */

#ifndef _ASSEMBLYCHUNK_H_
#define _ASSEMBLYCHUNK_H_

/* local includes */
#include "InstructionList.h"

/* structure for chunks of assembly code */
typedef struct assemblyChunk
{
    /* preamble comment */
    string * preamble;
    
    /* list of instructions */
    InstructionList * iList;
    
    /* postamble comment */
    string * postamble;
    
    /* list pointers */
    struct assemblyChunk * next;
    struct assemblyChunk * prev;
} AssemblyChunk;

/* Function prototypes */

/* Constructor */
AssemblyChunk *newAssemblyChunk();

/* Destructor */

/* Mutator */

/* Printer */
void printChunk(FILE *, AssemblyChunk *);

#endif