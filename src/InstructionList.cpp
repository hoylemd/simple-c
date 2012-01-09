# include "InstructionList.h"

/**
* Function to create a new instruction list structure
*/
InstructionList * newInstructionList()
{
    /* Allocate memory */
    InstructionList * iList = (InstructionList *)malloc(sizeof(InstructionList));
    
    /* initialize fields */
    iList->numInstr = 0;
    iList->first = NULL;
    iList->last = NULL;
    
    /* return pointer */
    return iList;
}


/**
* Function to destruct an instruction list structure
* for redundant safety, the result of this function should be
* assigned to the target's pointer.
*/
InstructionList * destroyInstructionList(InstructionList * iList)
{
    /* initialize variables */
    Instruction * currentI = NULL, * nextI = NULL;
    
    /* ensure we have a list */
    if (iList)
    {
        /* set up pointer */
        nextI = iList->first;
        
        /* disconnect the first and last pointers */
        iList->first = NULL;
        iList->last = NULL;
        
        /* iterate through the list and... */
        while(nextI)
        {
            /* get the next member of the list */
            currentI = nextI;
            nextI = currentI->next;
            
            /* destruct the current member */
            destroyInstruction(currentI);
        }
        
        /* free the list */
        free(iList);
    }
    
    /* return a null pointer */
    return NULL;
}

/**
* Function to append an instruction to a list
*/
InstructionList * appendInstruction(InstructionList * list, Instruction * i)
{
    /* ensure we actually have both operands */
    if(list && i)
    {
        /* handle lists with members */
        if (list->first)
        {
            /* connect the instruction to the last member */
            list->last->next = i;
            i->prev = list->last;
            list->last = i;
        }
        
        /* handle empty lists */
        else
        {
            /* connect the list's pointers to the instruction */
            list->first = i;
            list->last = i;
        }
        
        /* set the line numbers and counters correctly */
        i->num = list->numInstr;
        list->numInstr++;
    }
    
    /* return the modified list */
    return list;
}

/**
* Function to insert an instruction at a specific line number
* if the line number provided is negative, seek from the end of the list.
* just like python!
*/
InstructionList * insertInstruction(InstructionList * list, Instruction * inst, int pos)
{
    /* initalize variables */
    Instruction * currentI = NULL;
    int i = 0;
    
    /* ensure we have all operands */
    if (list && inst)
    {
        /* ensure we have a reasonable position */
        if (abs(pos) < list->numInstr)
        {
            /* handle seeking directions */
            if (pos < 0) /* negative */
            {
                /* find the instruction at this position */
                currentI = list->last;
                for(i=0; i < abs(pos); i++)
                    currentI = currentI->prev;
            }
            else /* positive */
            {
                /* find the instruction at this position */
                currentI = list->first;
                for(i=0; i < pos; i++)
                    currentI = currentI->next;
            }
            
            /* insert the instruction, handling case of insert at the head case */
            if(currentI != list->first)
            {
                currentI->prev->next = inst;
                inst->prev = currentI->prev;
            }
            currentI->prev = inst;
            inst->next = currentI;
            
            /* update the line numbers */
            inst->num = currentI->num;
            list->numInstr++;
            while(currentI)
            {
                currentI->num++;
                currentI = currentI->next;
            }
        }
    }
}

/**
* Function to print an instruction list to a file such that it can
* be interpreted by the VM.
*/
void printInstructionList(FILE * target, InstructionList * iList)
{
    /* initialize variables */
    Instruction * currentI = NULL;
    
    /* ensure we have a list */
    if (iList)
    {
        /* get the first member */
        currentI = iList->first;
        
        /* iterate through the list and print each instruction */
        while(currentI)
        {
            printInstruction(target, currentI);
            currentI = currentI->next;
        }
    }
    else
        /* complain if we werent given a list */
        fprintf(error, "Cannot print null Instruction List\n");
}
