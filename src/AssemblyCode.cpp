/**
 * AssemblyCode implementation
 */

# include "AssemblyCode.h"

/**
* Function to create a new code section
*/
AssemblyCode * newAssemblyCode()
{
    /* allocate memory */
    AssemblyCode * aCode = (AssemblyCode *) malloc(sizeof(AssemblyCode));
    
    /* initialize members */
    aCode->chunkCount = 0;
    aCode->first = NULL;
    aCode->last = NULL;
    
    /* return the new structure */
    return aCode;
}

/**
* Function to disconnect a code section from it's chinks and delete it.
*/
AssemblyCode * disconnectCode(AssemblyCode * aCode)
{
    /* ensure we have a code section */
    if (aCode)
    {
        /* disconnect the chunk pointers */
        aCode->first = NULL;
        aCode->last = NULL;
        
        /* free the structure */
        free(aCode);
    }
    
    /* complain if no code section was passed */
    else
        fprintf(error, "Cannot disconnect null code\n");
    
    /* return NULL */
    return (AssemblyCode *) NULL;
}

/**
* Function to append a chunk of assembly code to an AssemblyCode
*/
AssemblyCode * appendChunk(AssemblyCode * aCode, AssemblyChunk *aChunk)
{
    /* initialize variables */
    int offset = 0;
    Instruction * inst = NULL;
    
    /* check we have both operands */
    if (aCode && aChunk)
    {
        /* get the line number offset */
        offset = instructionsInAssembly(aCode);
        
        /* if the code already has instructions, append the chunk */
        if(aCode->chunkCount)
        {
            aCode->last->next = aChunk;
            aChunk->prev = aCode->last;
            aCode->last = aChunk;
        }
        
        /* otherwise, just add it to the code */
        else
        {
            aCode->first = aChunk;
            aCode->last = aChunk;
        }
        aCode->chunkCount++;
        
        /* shift the instruction numbers */
        if (aChunk->iList)
            inst = aChunk->iList->first;
        while(inst)
        {
            inst->num = inst->num + offset;
            inst = inst->next;
        }
    }
}

/**
* Function to append a code section to another and make a new code section from it.
*/
AssemblyCode * appendCode(AssemblyCode * header, AssemblyCode * body)
{
    /* initialize variables and pointers */
    int offset = 0;
    AssemblyCode * aCode = NULL;
    AssemblyChunk * aChunk = NULL;
    Instruction * inst = NULL;
    
    /* ensure we have both operands */
    if (header && body)
    {
        /* offset the body's instruction numbers */
        offset = instructionsInAssembly(header);
        
        /*connect the new code section to the operand's lists*/
        aCode = newAssemblyCode();
        aCode->first = header->first;
        aCode->last = body->last;
        
        /* connect the lists */
        header->last->next = body->first;
        body->first->prev = header->last;
        
        /* count up the chunks */
        aCode->chunkCount = header->chunkCount + body->chunkCount;
        
        /* get the first chunk of the new section */
        aChunk = body->first;
        
        /* iterate through the chunk list */
        while(aChunk)
        {
            inst = NULL;
            
            /* get the first instruction of this chunk */
            if (aChunk->iList)
                inst = aChunk->iList->first;
            
            /* apply the offset to the line numbers */
            while(inst)
            {
                inst->num = inst->num + offset;
                inst = inst->next;
            }
            
            aChunk = aChunk->next;
        }
        
        
        /* return the new Code section */
        return aCode;
    }
    
    /* complain if we are missing a header */
    else if (body)
    {
        fprintf(error, "Cannot append a code section to a null code section\n");
        return NULL;
    }
    
    /* complain if we are missing a body */
    {
        fprintf(error, "Cannot append a null code section.\n");
        return NULL;
    }
}

/**
* Function to count the number of instructions in a code section
*/
int instructionsInAssembly(AssemblyCode * aCode)
{
    /* initialize variables */
    int sum = 0;
    AssemblyChunk * aChunk = NULL;
    
    /* ensure we have a code section */
    if(aCode)
    {
        /* get the first code chunk */
        aChunk = aCode->first;
        
        /* iterate through the chunk list */
        while(aChunk)
        {
            /* if this chunk has instructions, add their count to the sum */
            if (aChunk->iList)
                sum+= aChunk->iList->numInstr;
            
            /* grab the next chunk */
            aChunk = aChunk->next;
        }
    }
    
    /* complain if no code section passed */
    else
    {
        fprintf(error, "Cannot count instructions in a null code section.\n");
        /* aChunk = aCode->first; */
    }
    
    /* return the sum.*/
    return sum;
}

/**
* Function to print a Code section to a stream
*/
void printCode(FILE * target, AssemblyCode * aCode)
{
    /* initialize variables */
    AssemblyChunk * aChunk = NULL;
    
    /* ensure we have a code section and a stream */
    if(aCode && target)
    {
        /* get the first code chunk */
        aChunk = aCode->first;
        
        /* iterate through the list */
        while(aChunk)
        {
            /* print all chunks out */
            printChunk(target, aChunk);
            aChunk = aChunk->next;
        }
    }
    
    /* complain if a null code section is passed */
    else if (target)
        fprintf(error, "Cannot print null code.\n");
    
    /* complain if a null stream is passed */
    else if (target)
        fprintf(error, "Cannot print to a null stream.\n");
}
