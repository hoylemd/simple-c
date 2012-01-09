/**
 * header for instructions
 */

#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_

/* includes */
#include "globals.h"
#include <stdlib.h>
#include <string>

using namespace std;

/* enumerations */

/* enum for register-only opcodes */
typedef enum
{
    NoneR,
    Halt, Input, Output,
    Add, Subtract, Multiply, Divide
} rOpCode;

/* enum for the instruction type */
typedef enum
{
    RegisterInstruction,
    RegisterMemoryInstruction
} InstructionType;

/* enum for register / memory opcodes */
typedef enum
{
    NoneRM,
    Load, LoadAddress, LoadConstant,
    Store,
    JumpLessThan, JumpLessEqual, JumpGreaterThan, JumpGreaterEqual, JumpEqual, JumpNotEqual
} rmOpCode;

/* unions */

/* union for all types of opcodes */
typedef union
{
    rOpCode rCode;
    rmOpCode rmCode;
} opCode;

/* structure to build an instruction */
typedef struct instr
{
    /* operation type and code */
    InstructionType type;
    opCode oc;
    
    /* operands */
    int o1;
    int o2;
    int o3;
    
    /* list pointers */
    struct instr * prev;
    struct instr * next;
    
    /* line number */
    int num;
    
    /* label/comment */
    string * label;
} Instruction;

/* function prototypes */

/* Constructor */
Instruction * newInstruction(InstructionType, rOpCode, rmOpCode,
                             int, int, int,
                             string *);

/* Destructor */
Instruction * destroyInstruction(Instruction * );

/* printer */
void printInstruction(FILE *, Instruction *);

/* op code generators */
string * ROpCodeString(rOpCode);
string * RmOpCodeString(rmOpCode);

#endif
