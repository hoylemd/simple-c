/**
* header for InstructionList
*/

#ifndef _INSTRUCTIONLIST_H_
#define _INSTRUCTIONLIST_H_

#include "Instruction.h"
#include "globals.h"

/* Instruction List structure */
typedef struct instrList
{
    /* count of the contained instructions */
    int numInstr;
    
    /* list pointers */
    Instruction * first;
    Instruction * last;
} InstructionList;

/* function prototypes */

/* Constructor */
InstructionList * newInstructionList();

/* Destructor */
InstructionList * destroyInstructionList(InstructionList *);

/* Mutators */
InstructionList * appendInstruction(InstructionList *, Instruction * );
InstructionList * insertInstruction(InstructionList *, Instruction *, int);

/* Printer */
void printInstructionList(FILE *, InstructionList *);

#endif