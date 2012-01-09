/* implementation file for Instruction */

#include "Instruction.h"

/**
* Function to generate an instruction structure from it's component data
*/
Instruction * newInstruction(InstructionType type, rOpCode rCode, rmOpCode rmCode,
                             int o1, int o2, int o3,
                             string * label)
{
    /* allocate memory for the struct */
    Instruction * nInst = (Instruction *) malloc(sizeof(Instruction));

    /* get the opcode */
    nInst->type = type;
    if (type == RegisterInstruction)
        nInst->oc.rCode = rCode;
    else if(type == RegisterMemoryInstruction)
        nInst->oc.rmCode = rmCode;

    /* store the operands */
    nInst->o1= o1;
    nInst->o2= o2;
    nInst->o3= o3;

    /* initialize list pointers */
    nInst->prev = NULL;
    nInst->next = NULL;

    /* initialize id number */
    nInst->num = 0;

    /* store the label */
    nInst->label = label;

    /* return the structure */
    return nInst;
}

/**
* Destructor for an instruction structure
* for redundant safety, the result of this function should be
* assigned to the target's pointer.
*/
Instruction * destroyInstruction(Instruction * i)
{
    /* Ensure it exists */
    if (i)
    {
        /* clear pointers */
        i->prev = NULL;
        i->next = NULL;
        
        /* delete objects */
        delete i->label;
        
        /* free the struct */
        free(i);
    }
    
    /* return a NULL pointer */
    return NULL;
}

/**
* Function to print an instruction to a file such that it can
* be interpreted by the VM.
*/
void printInstruction(FILE * target, Instruction * inst)
{
    
    /* initialize variables */
    int spaces = 0, i = 0;
    
    /* ensure we actually have an instruction */
    if (inst)
    {
        /* determine the number of spaces needed for line numbers */
        /* only correctly handles instructions up to # 999 */
        if (inst->num < 10) spaces = 2;
        else if (inst->num < 100) spaces = 1;
        else spaces = 0;
        
        /* space the line number correctly */
        for(i=0;i<spaces;i++)fprintf(target, " ");
        
        /* call the appropriate print routine */
        if (inst->type == RegisterMemoryInstruction)
            fprintf(target, "%d:%s  %d,%d(%d) \t%s\n", inst->num, RmOpCodeString(inst->oc.rmCode)->data(),
                    inst->o1, inst->o2, inst->o3, inst->label->data());
                    else
                        fprintf(target, "%d:%s  %d,%d,%d \t%s\n", inst->num, ROpCodeString(inst->oc.rCode)->data(),
                        inst->o1, inst->o2, inst->o3, inst->label->data());
    }
    else
    {
        /* print error message if the instruction was null */
        fprintf(error, "Cannot print null instruction\n");
    }
}

/**
* Function to get the string command for a register operation opcode
*/
string * ROpCodeString(rOpCode r)
{
    /* validate input */
    if (r)
    {
        /* select the string */
        switch (r)
        {
            case Halt:
                return new string("   HALT");
            case Input:
                return new string( "     IN");
            case Output:
                return new string( "    OUT");
            case Add:
                return new string( "    ADD");
            case Subtract:
                return new string( "    SUB");
            case Multiply:
                return new string( "    MUL");
            case Divide:
                return new string( "    DIV");
                
                /* return nothing on unrecognized opcode */
                default:
                    return new string("");
        }
    }
    
    /* return nothing if no opcode given */
    else
        return new string("");
}

/**
* Function to get the string command for a register/memory operation opcode
*/
string * RmOpCodeString(rmOpCode rm)
{
    /* validate input */
    if(rm)
    {
        
        /* select the appropriate string */
        switch (rm)
        {
            case Load:
                return new string( "     LD");
            case LoadAddress:
                return new string( "    LDA");
            case LoadConstant:
                return new string( "    LDC");
            case Store:
                return new string( "     ST");
            case JumpLessThan:
                return new string( "    JLT");
            case JumpLessEqual:
                return new string( "    JLE");
            case JumpGreaterThan:
                return new string( "    JGT");
            case JumpGreaterEqual:
                return new string( "    JGE");
            case JumpEqual:
                return new string( "    JEQ");
            case JumpNotEqual:
                return new string( "    JNE");
                
                /* return nothing on unrecognized opcode */
                default:
                    return new string("");
        }
    }
    
    /* return nothing if no opcode given */
    else
        return new string("");
}
