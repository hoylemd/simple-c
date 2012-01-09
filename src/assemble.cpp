/**
 * library to generate assembly code from an Abstract syntax tree and Scope tree
 */

#include "assemble.h"

/**
* Function to push a scope to the stack
*/
void pushScope(Scope * scope)
{
    scopeStack[scopeCount] = scope;
    scopeCount++;
}

/**
* Function to pop a scope off the stack
*/
Scope * popScope()
{
    scopeCount--;
    return scopeStack[scopeCount];
}

/**
* Function to generate expression evaluation assembly code from an AST node and scope
*/
AssemblyCode * evaluateExpression(TreeNode * expr, Scope * scope)
{
    /**
    * TODO: functionalize this. it's huge. (your mom joke here)
    */
    
    /* initialize variables and pointers */
    TreeNode * currentNode = NULL, * subNode = NULL, *arg = NULL;
    InstructionList * iList = NULL;
    AssemblyChunk * aChunk = NULL;
    AssemblyCode * aCode = NULL, *bCode = NULL, *cCode = NULL;
    Instruction * inst;
    int op1Offset = 0, op2Offset = 0, scopeUps = 0, i = 0;
    Scope * cScope = NULL;
    int argNum = 0, jumpLoc = 0, index = 0;
    string * comment = NULL;
    
    /* ensure the expression and scope exist */
    if (expr && scope)
    {
        /* handle terminals */
        if (expr->terminal)
        {
            /* select the terminal type */
            switch(expr->type.tType)
            {
                
                /* handle identifiers */
                case id:
                {
                    /* set up environment */
                    op1Offset = 0;
                    scopeUps = 0;
                    cScope = scope;
                    
                    /* loop trace through the scopes to try to find the variable */
                    while(!op1Offset)
                    {
                        /* look up the identifier in the symbol table */
                        op1Offset = lookupOffset(expr->tokenValue, cScope->symbols);
                        
                        /* if it's not in the current scope */
                        if (!op1Offset)
                        {
                            /* move up to the parent scope, if any */
                            if (cScope->parent)
                            {
                                cScope = cScope->parent;
                                scopeUps++;
                            }
                            
                            /* if there is no more parent, then the symbol is not in scope */
                            /* this should not ever happen, but complain to death if it does */
                            else
                            {
                                fprintf(error, "Symbol not found. How was this not caught?!\n");
                                exit(0);
                            }
                        }
                    }
                    
                    /* generate a new code section, chunk, and instruction list for this expression */
                    aCode = newAssemblyCode();
                    aChunk = newAssemblyChunk();
                    iList = newInstructionList();
                    
                    /* generate an instruction to load the scope pointer, and add it to the list */
                    inst = newInstruction(RegisterMemoryInstruction, NoneR, LoadAddress,
                                          OP1, 0, FP,
                                          new string("set the scope pointer"));
                    iList = appendInstruction( iList, inst);

                    /* if the scope this identifier is at is not global */
                    if (cScope->parent) /* all non-global scopes have global as a parent */
                    {
                        /* add an instruction for each layer of scope to be raised */
                        for(i=0; i< scopeUps; i++)
                        {
                            inst = newInstruction(RegisterMemoryInstruction, NoneR, Load,
                                                OP1, 0, OP1,
                                                new string("raise scope level"));
                            iList = appendInstruction( iList, inst);
                        }
                    }

                    /* handle global scope */
                    else
                    {
                        /* add the global scope instruction */
                        inst = newInstruction(RegisterMemoryInstruction, NoneR, LoadAddress,
                                            OP1, 0, GP,
                                            new string("set scope to global"));
                        iList = appendInstruction( iList, inst );
                    }

                    /* generate a new comment for the assembly code */
                    if(comment) delete comment;
                    comment = new string ("load variable ");
                    comment->append( *expr->tokenValue);

                    /* generate an instruction to load the identifier value into memory */
                    inst = newInstruction(RegisterMemoryInstruction, NoneR, Load,
                                        RES, op1Offset, OP1, comment);
                    comment = NULL;
                    iList = appendInstruction( iList, inst);

                    /* put the instruction list in the chunk */
                    aChunk->iList = iList;

                    /* add the chunk to the code section */
                    appendChunk(aCode, aChunk);

                    /* return the code section */
                    return aCode;
                    break;
                }
                
                /* handle literal numbers */
                case number:
                {
                    
                    /* set up new structures */
                    aCode = newAssemblyCode();
                    aChunk = newAssemblyChunk();
                    iList = newInstructionList();
                    
                    /* generate a new comment */
                    if(comment) delete comment;
                    comment = new string("load constant ");
                    comment->append(*expr->tokenValue);
                    
                    /* generate an instruction to load the literal into a register */
                    inst = newInstruction(RegisterMemoryInstruction, NoneR, LoadConstant,
                                          RES, atoi(expr->tokenValue->data()), 0, comment);
                    comment = NULL;
                    iList = appendInstruction( iList, inst);

                    /* add the instruction to the chunk and code section */
                    aChunk->iList = iList;
                    appendChunk(aCode, aChunk);

                    /* retrn the code section */
                    return aCode;
                    break;
                }
            }
        }
        
        /* handle nonterminals */
        else
        {
            /* switch by expression type */
            switch(expr->type.rule)
            {
                
                /* handle expressions */
                case expression: /* this means an assignment expression, generally */
                {
                    /* if this is an assignment expression */
                    /* generate evaluation code for the right side */
                    aCode = evaluateExpression(expr->child[2], scope);
                    
                    /* set up environment */
                    op1Offset = 0;
                    scopeUps = 0;
                    cScope = scope;
                    
                    /* find the offset and scope */
                    while(!op1Offset)
                    {
                        /* look up the symbol at this scope */
                        op1Offset = lookupOffset(expr->child[0]->tokenValue, cScope->symbols);
                        
                        /* if not found, go up a level in scope */
                        if (!op1Offset)
                            if (cScope->parent)
                            {
                                cScope = cScope->parent;
                                scopeUps++;
                            }
                            else
                            {
                                /* handle the symbol not being found. */
                                /* this should never happen */
                                fprintf(error, "Symbol [%s] not found. How was this not caught?!\n",
                                        expr->child[0]->tokenValue->data());
                                        exit(0);
                            }
                    }
                    
                    /* create chunk and instruction list structures */
                    aChunk = newAssemblyChunk();
                    iList = newInstructionList();
                    
                    /* add instruction to load the scope pointer */
                    inst = newInstruction(RegisterMemoryInstruction, NoneR, LoadAddress,
                                          OP1, 0, FP,
                                          new string("set the scope pointer"));
                    iList = appendInstruction( iList, inst);

                    /* add instructions to find the right scope level */
                    if (cScope->parent)
                    {
                        for(i=0; i< scopeUps; i++)
                        {
                            inst = newInstruction(RegisterMemoryInstruction, NoneR, Load,
                                                OP1, 0, OP1,
                                                new string("raise scope level"));
                            iList = appendInstruction( iList, inst);
                        }
                    }

                    /* handle global scope */
                    else
                    {
                        inst = newInstruction(RegisterMemoryInstruction, NoneR, LoadAddress,
                                            OP1, 0, GP,
                                            new string("set scope to global"));
                        iList = appendInstruction( iList, inst );
                    }

                    /* generate the assignment code */
                    aChunk->preamble = new string("* Assigning value to \'");
                    aChunk->preamble->append(*expr->child[0]->tokenValue).append("\'");
                    inst = newInstruction(RegisterMemoryInstruction, NoneR, Store,
                                        RES, op1Offset, OP1,
                                        new string("assign value to local variable"));
                    appendInstruction(iList, inst);

                    /* add the instructions to the chunk and code section */
                    aChunk->iList = iList;
                    appendChunk(aCode, aChunk);

                    /* return the new code section */
                    return aCode;
                    break;
                }
                
                /* handle simple expressions */
                case simple_expression:
                {
                    /* generate code for the left operand */
                    bCode = evaluateExpression(expr->child[0], scope);
                    
                    /* add it to this code section's list */
                    if (!aCode) aCode = bCode;
                    else
                    {
                        cCode = appendCode(aCode, bCode);
                        aCode = disconnectCode(aCode);
                        bCode = disconnectCode(bCode);
                        aCode = cCode;
                    }
                    
                    /* generate code to store the left operand in the left temporary variable */
                    aChunk = newAssemblyChunk();
                    iList = newInstructionList();
                    inst = newInstruction(RegisterMemoryInstruction, NoneR, Store,
                                          RES, scope->symbols->nextOffset, FP,
                                          new string("Storing left operand in left temp"));
                    comment = NULL;
                    iList = appendInstruction( iList, inst);
                    aChunk->iList = iList;
                    appendChunk(aCode, aChunk);

                    /* generate code for the right operand */
                    bCode = evaluateExpression(expr->child[2], scope);

                    /* add it to this code section's list */
                    cCode = appendCode(aCode, bCode);
                    aCode = disconnectCode(aCode);
                    bCode = disconnectCode(bCode);
                    aCode = cCode;

                    /* generate code to store the right operand in the right temporary variable */
                    aChunk = newAssemblyChunk();
                    iList = newInstructionList();
                    inst = newInstruction(RegisterMemoryInstruction, NoneR, Store,
                                        RES, scope->symbols->nextOffset-1, FP,
                                        new string("Storing right operand in right temp"));
                    iList = appendInstruction( iList, inst);
                    aChunk->iList = iList;
                    appendChunk(aCode, aChunk);

                    /* generate the evaluation code section */
                    bCode = newAssemblyCode();
                    aChunk = newAssemblyChunk();
                    aChunk->preamble = new string("* Evaluating \'");
                    aChunk->preamble->append(*expr->child[0]->tokenValue).append("\' ");
                    aChunk->preamble->append(*expr->child[1]->tokenValue).append(" \'");
                    aChunk->preamble->append(*expr->child[2]->tokenValue).append("\'");
                    iList = newInstructionList();

                    /* add left operand loading code */
                    inst = newInstruction(RegisterMemoryInstruction, NoneR, Load,
                                            OP1, scope->symbols->nextOffset, FP,
                                            new string("Load left operand"));
                    iList = appendInstruction(iList, inst);

                    /* add left operand loading code */
                    inst =  newInstruction(RegisterMemoryInstruction, NoneR, Load,
                                            OP2, scope->symbols->nextOffset-1, FP,
                                            new string("Load right operand"));
                    iList = appendInstruction(iList, inst);

                    /* add comparison code */
                    inst = newInstruction(RegisterInstruction, Subtract, NoneRM,
                                        OP1, OP1, OP2,
                                        new string("calculate difference"));
                    iList = appendInstruction(iList, inst);

                    /* add code to load null into a register for comparison */
                    inst =  newInstruction(RegisterMemoryInstruction, NoneR, LoadConstant,
                                            RES, 0, 0,
                                            new string("set null result"));
                    iList = appendInstruction(iList, inst);

                    /* generate code based on the type of operation */
                    switch(expr->child[1]->type.tType)
                    {
                        /* equivalence */
                        case cnd_equal:
                        {
                            inst = newInstruction(RegisterMemoryInstruction, NoneR, JumpNotEqual,
                                                OP1, 1, PC,
                                                new string("skip next line if ops not equal"));
                            iList = appendInstruction(iList, inst);
                            break;
                        }

                        /* nonequivalence */
                        case cnd_notEqual:
                        {
                            inst = newInstruction(RegisterMemoryInstruction, NoneR, JumpEqual,
                                                OP1, 1, PC,
                                                new string("skip next line if ops equal"));
                            iList = appendInstruction(iList,inst);
                            break;
                        }

                        /* less than or equivalence */
                        case cnd_lessOrEqual:
                        {
                            inst = newInstruction(RegisterMemoryInstruction, NoneR, JumpGreaterThan,
                                                OP1, 1, PC,
                                                new string("skip next line if ops greater than"));
                            iList = appendInstruction(iList, inst);
                            break;
                        }

                        /* greater than or equivalence */
                        case cnd_moreOrEqual:
                        {
                            inst = newInstruction(RegisterMemoryInstruction, NoneR, JumpLessThan,
                                                OP1, 1, PC,
                                                new string("skip next line if ops less than"));
                            iList = appendInstruction(iList, inst);
                            break;
                        }

                        /* less than */
                        case cnd_less:
                        {
                            inst = newInstruction(RegisterMemoryInstruction, NoneR, JumpGreaterEqual,
                                                OP1, 1, PC,
                                                new string("skip next line if ops greater than or equal"));
                            iList = appendInstruction(iList, inst);
                            break;
                        }

                        /* greater than */
                        case cnd_more:
                        {
                            inst = newInstruction(RegisterMemoryInstruction, NoneR, JumpLessEqual,
                                                OP1, 1, PC,
                                                new string("skip next line if ops less than or equal"));
                            iList = appendInstruction(iList, inst);
                            break;
                        }
                    }

                    /* add the return true instruction */
                    inst = newInstruction(RegisterMemoryInstruction, NoneR, LoadConstant,
                                        RES, 1, 0,
                                        new string("set result to true"));
                    iList = appendInstruction(iList, inst);

                    /* add the list and chunk to the code section */
                    aChunk->iList = iList;
                    appendChunk(bCode, aChunk);

                    /* add the new section to the existing section */
                    if (!aCode) aCode = bCode;
                    else
                    {
                        cCode = appendCode(aCode, bCode);
                        aCode = disconnectCode(aCode);
                        bCode = disconnectCode(bCode);
                        aCode = cCode;
                    }

                    /* return the generated code section */
                    return aCode;
                    break;
                }
                
                /* handle additive expressions */
                case additive_expression:
                {
                    /* generate code for the left operand */
                    bCode = evaluateExpression(expr->child[0], scope);
                    
                    /* add it to this code section's list */
                    if (!aCode) aCode = bCode;
                    else
                    {
                        cCode = appendCode(aCode, bCode);
                        aCode = disconnectCode(aCode);
                        bCode = disconnectCode(bCode);
                        aCode = cCode;
                    }
                    /* add code to store it in the left temporary variable */
                    aChunk = newAssemblyChunk();
                    iList = newInstructionList();
                    inst = newInstruction(RegisterMemoryInstruction, NoneR, Store,
                                          RES, scope->symbols->nextOffset, FP,
                                          new string("Storing left operand in left temp"));
                    iList = appendInstruction( iList, inst);
                    aChunk->iList = iList;
                    appendChunk(aCode, aChunk);

                    /* generate code for the right operand */
                    bCode = evaluateExpression(expr->child[2], scope);
                    /* add it to this code's list */
                    cCode = appendCode(aCode, bCode);
                    aCode = disconnectCode(aCode);
                    aCode = disconnectCode(bCode);
                    aCode = cCode;

                    /* add code to store it in the right temporary variable */
                    aChunk = newAssemblyChunk();
                    iList = newInstructionList();
                    inst = newInstruction(RegisterMemoryInstruction, NoneR, Store,
                                        RES, scope->symbols->nextOffset-1, FP,
                                        new string("Storing right operand in right temp"));
                    iList = appendInstruction( iList, inst);
                    aChunk->iList = iList;
                    appendChunk(aCode, aChunk);

                    /* generate the evaluation code section */
                    bCode = newAssemblyCode();
                    aChunk = newAssemblyChunk();
                    aChunk->preamble = new string("* Evaluating \'");
                    aChunk->preamble->append(*expr->child[0]->tokenValue).append("\' ");
                    aChunk->preamble->append(*expr->child[1]->tokenValue).append(" \'");
                    aChunk->preamble->append(*expr->child[2]->tokenValue).append("\'");

                    /* add code to load left operand */
                    iList = newInstructionList();
                    inst = newInstruction(RegisterMemoryInstruction, NoneR, Load,
                                            OP1, scope->symbols->nextOffset, FP,
                                            new string("Load left operator"));
                    iList = appendInstruction(iList, inst);

                    /* add code to load right operand */
                    inst = newInstruction(RegisterMemoryInstruction, NoneR, Load,
                                        OP2, scope->symbols->nextOffset-1, FP,
                                        new string("Load right operator"));
                    iList = appendInstruction(iList, inst);

                    /* handle addition operation */
                    if (expr->child[1]->type.tType == mth_plus)
                    {
                        inst = newInstruction(RegisterInstruction, Add, NoneRM,
                                                RES, OP1, OP2, new string("Evaluate addition"));
                        iList = appendInstruction(iList, inst);
                    }

                    /* handle subtraction operation */
                    else
                    {
                        inst =  newInstruction(RegisterInstruction, Subtract, NoneRM,
                                                RES, OP1, OP2, new string("Evaluate subtraction"));
                        iList = appendInstruction(iList, inst);
                    }

                    /* package the evaluation code into a code section */
                    aChunk->iList = iList;
                    appendChunk(bCode, aChunk);

                    /* append the evaluation code to the existing code */
                    if (!aCode) aCode = bCode;
                    else
                    {
                        cCode = appendCode(aCode, bCode);
                        aCode = disconnectCode(aCode);
                        bCode = disconnectCode(bCode);
                        aCode = cCode;
                    }

                    /* return the generated code section */
                    return aCode;
                    break;
                }
                
                /* handle multiplicative statements */
                case term:
                {
                    /* generate code for the left operand */
                    bCode = evaluateExpression(expr->child[0], scope);
                    
                    /* add it to this code section */
                    if (!aCode) aCode = bCode;
                    else
                    {
                        cCode = appendCode(aCode, bCode);
                        aCode = disconnectCode(aCode);
                        bCode = disconnectCode(bCode);
                        aCode = cCode;
                    }
                    
                    /* generate code to store it in the left temporary variable */
                    aChunk = newAssemblyChunk();
                    iList = newInstructionList();
                    inst = newInstruction(RegisterMemoryInstruction, NoneR, Store,
                                          RES, scope->symbols->nextOffset, FP,
                                          new string("Storing left operand in left temp"));
                    iList = appendInstruction( iList, inst);
                    aChunk->iList = iList;
                    appendChunk(aCode, aChunk);

                    /* generate code for the right operand */
                    bCode = evaluateExpression(expr->child[2], scope);

                    /* add it to this code section */
                    cCode = appendCode(aCode, bCode);
                    aCode = disconnectCode(aCode);
                    bCode = disconnectCode(bCode);
                    aCode = cCode;

                    /* generate code to store it in the right temporary variable */
                    aChunk = newAssemblyChunk();
                    iList = newInstructionList();
                    inst = newInstruction(RegisterMemoryInstruction, NoneR, Store,
                                        RES, scope->symbols->nextOffset-1, FP,
                                        new string("Storing right operand in right temp"));
                    iList = appendInstruction( iList, inst);
                    aChunk->iList = iList;
                    appendChunk(aCode, aChunk);

                    /* generate the evaluation code section */
                    bCode = newAssemblyCode();
                    aChunk = newAssemblyChunk();
                    aChunk->preamble = new string("* Evaluating \'");
                    aChunk->preamble->append(*expr->child[0]->tokenValue).append("\' ");
                    aChunk->preamble->append(*expr->child[1]->tokenValue).append(" \'");
                    aChunk->preamble->append(*expr->child[2]->tokenValue).append("\'");

                    /* generate code to load the left operand */
                    iList = newInstructionList();
                    inst = newInstruction(RegisterMemoryInstruction, NoneR, Load,
                                            OP1, scope->symbols->nextOffset, FP,
                                            new string("Load left operator"));
                    iList = appendInstruction(iList, inst);

                    /* generat code to load the right operand */

                    inst = newInstruction(RegisterMemoryInstruction, NoneR, Load,
                                        OP2, scope->symbols->nextOffset-1, FP,
                                        new string("Load right operator"));
                    iList = appendInstruction(iList, inst);

                    /* handle multiplication */
                    if (expr->child[1]->type.tType == mth_multiply)
                    {
                        inst = newInstruction(RegisterInstruction, Multiply, NoneRM,
                                                RES, OP1, OP2, new string("Evaluate multiplication"));
                        iList = appendInstruction(iList, inst);
                    }

                    /* handle division */
                    else
                    {
                        inst = newInstruction(RegisterInstruction, Divide, NoneRM,
                                                RES, OP1, OP2, new string("Evaluate division"));
                        iList = appendInstruction(iList, inst);
                    }

                    /* add the code to the section */
                    aChunk->iList = iList;
                    appendChunk(bCode, aChunk);

                    /* append the evaluation code to the existing section */
                    if (!aCode) aCode = bCode;
                    else
                    {
                        cCode = appendCode(aCode, bCode);
                        aCode = disconnectCode(aCode);
                        bCode = disconnectCode(bCode);
                        aCode = cCode;
                    }

                    /* return the generated code section */
                    return aCode;
                    break;
                }
                
                /* handle parenthesized expressions */
                case factor:
                {
                    /* generate code for the child expression */
                    aCode = evaluateExpression(expr->child[1], scope);
                    
                    /* return this code */
                    return aCode;
                    break;
                }
                
                /* handle function calls */
                case call:
                {
                    
                    /* set up code containers */
                    aCode = newAssemblyCode();
                    aChunk = newAssemblyChunk();
                    aChunk->preamble = new string("* Call to function ");
                    aChunk->preamble->append(*expr->child[0]->tokenValue);
                    appendChunk(aCode, aChunk);
                    
                    /* store arguments */
                    /* get the arguments node */
                    subNode = expr->child[2];
                    
                    /* parse and encode them */
                    arg = subNode->child[0];
                    argNum = 0;
                    while(arg)
                    {
                        /* skip commas */
                        if (arg->type.tType != comma)
                        {
                            /* generate code to evaluate the argument */
                            bCode = evaluateExpression(arg, scope);
                            cCode = appendCode(aCode, bCode);
                            aCode = disconnectCode(aCode);
                            bCode = disconnectCode(bCode);
                            aCode = cCode;
                            
                            /* generate code to store it */
                            aChunk = newAssemblyChunk();
                            iList = newInstructionList();
                            inst = newInstruction(RegisterMemoryInstruction, NoneR, Store, RES,
                                                  scope->symbols->nextOffset-(4+argNum),
                                                  FP, new string("store argument"));
                            iList = appendInstruction(iList, inst);
                            aChunk->iList = iList;
                            appendChunk(aCode, aChunk);

                            /* increment the argument counter */
                            argNum++;
                        }
                        
                        /* move to the next argument */
                        arg = arg->sibling;
                    }
                    
                    /* generate code to store the frame location */
                    aChunk = newAssemblyChunk();
                    iList = newInstructionList();
                    inst = newInstruction(RegisterMemoryInstruction, NoneR, Store,
                                          FP, scope->symbols->nextOffset-2, FP, new string("push ofp"));
                    iList = appendInstruction(iList, inst);

                    /* move FP to frame's location: 0(FP) for new function instance */
                    inst = newInstruction(RegisterMemoryInstruction, NoneR, LoadAddress,
                                        FP, scope->symbols->nextOffset-2, FP, new string("push frame"));
                    iList = appendInstruction(iList, inst);

                    /* load AC with return instruction (1(PC)) */
                    inst = newInstruction(RegisterMemoryInstruction, NoneR, LoadAddress,
                                            AC, 1, PC, new string("load ac with ret ptr"));
                    iList = appendInstruction(iList, inst);

                    /* load pc with function position */
                    /* get the jump target */
                    index = indexOfSymbol(expr->child[0]->tokenValue, global->symbols);
                    jumpLoc = global->symbols->lineNo[index];
                    inst = newInstruction(RegisterMemoryInstruction, NoneR, LoadConstant,
                                        PC, jumpLoc, 0, new string("jump to function body"));
                    iList = appendInstruction(iList, inst);

                    /* retrieve frame */
                    inst = newInstruction(RegisterMemoryInstruction, NoneR, Load,
                                            FP, 0, FP, new string("pop frame"));
                    iList = appendInstruction(iList, inst);

                    /* append the call code to the existing code */
                    aChunk->iList = iList;
                    appendChunk(aCode, aChunk);

                    /* return the generated code */
                    return aCode;
                    break;
                }
            }
        }
    }
    
    /* complain if the expression is null */
    else if (scope)
    {
        fprintf(error,"cannot evaluate a null expression\n");
    }
    
    /* complain if the scope is null */
    else if (expression)
    {
        fprintf(error, "cannot evaluate an expression without a scope\n");
    }
    
    /* complain if both are null */
    else
    {
        fprintf(error, "cannot evaluate a null expression with a null scope\n");
    }
    
    /* return a null code section if an argument was invalid */
    return aCode;
}

/**
* Function to compile an Abstract Syntax tree into assembly code
*/
AssemblyCode * compileAST(TreeNode * tree, Scope * scope, int functionOffset)
{
    
    /* initialize variables and pointers */
    TreeNode * currentNode = tree, *subNode = NULL;
    Scope * currentScope = NULL;
    AssemblyCode * aCode = NULL, * bCode = NULL, *cCode = NULL, *body = NULL;
    AssemblyChunk * aChunk = NULL, *bChunk = NULL;
    Instruction * inst = NULL;
    InstructionList * iList = NULL;
    int functionsCompound = 0, subScope = 0, op1Offset = 0;
    int memOffset = 0, nextLineNo = functionOffset, lines = 0;
    int index = 0, i = 0;
    string * comment = NULL;
    
    /* ensure we have a valid tree and scope */
    if (tree && scope)
    {
        /* handle the different types of nodes */
        switch (tree->type.rule)
        {
            
            /* handle declaration lists */
            case (declaration_list):
            {
                /* point at the first child */
                currentNode = tree->child[0];
                
                /* iterate through the child nodes */
                while(currentNode)
                {
                    /* handle function declarations */
                    if (currentNode->type.rule == function_declaration)
                    {
                        /* initialize temporary pointer */
                        stringstream converter;
                        
                        /* generate the function header */
                        aChunk = newAssemblyChunk();
                        aChunk->preamble = new string("* function ");
                        aChunk->preamble->append(*currentNode->child[1]->tokenValue);
                        iList = newInstructionList();
                        
                        /* generate code to store return pointers */
                        if(comment) delete comment;
                        comment =  new string("store return for ");
                        comment->append(*currentNode->child[1]->tokenValue).append(" ");
                        converter << nextLineNo;
                        comment->append(converter.str());
                        inst = newInstruction(RegisterMemoryInstruction, NoneR, Store,
                                              AC, -1, FP,
                                              comment );
                        comment = NULL;
                        iList = appendInstruction(iList, inst);
                        
                        /* save the starting line number for this function in the symbol table */
                        index = indexOfSymbol(currentNode->child[1]->tokenValue, scope->symbols);
                        scope->symbols->lineNo[index] = nextLineNo;
                        nextLineNo++;
                        
                        /* add the code to the existing code */
                        aChunk->iList = iList;
                        if (!body)
                            body = newAssemblyCode();
                        appendChunk(body, aChunk);
                        
                        /* generate code for the function body */
                        aCode = compileAST (currentNode->child[5], scope->subScopes[subScope], nextLineNo);
                        
                        /* update line number and scope trackers */
                        if (bCode)
                            nextLineNo += instructionsInAssembly(bCode);
                        subScope++;
                        
                        /* add the body code to the header code */
                        bCode = appendCode(body, aCode);
                        body = disconnectCode(body);
                        aCode = disconnectCode(aCode);
                        body = bCode;
                        
                        /* generate the function footer */
                        aChunk = newAssemblyChunk();
                        aChunk->preamble = new string("* return to caller");
                        iList = newInstructionList();
                        inst = newInstruction(RegisterMemoryInstruction, NoneR, Load, PC, -1, FP,
                                              new string("return to caller"));
                        iList = appendInstruction(iList, inst);
                        nextLineNo++;

                        /* add the footer to the function code */
                        aChunk->iList = iList;
                        appendChunk(body, aChunk);

                        /* update the line number tracker */
                        nextLineNo = instructionsInAssembly(body) + functionOffset;
                    }
                    
                    /* handle variable declaration */
                    else
                    {
                        /* coming soon! */
                    }
                    
                    /* move over to the next child */
                    currentNode = currentNode->sibling;
                }
                
                /* return this generated code */
                return body;
                break;
            }
            
            /* handle compound statements */
            case compound_statement:
            {
                /*
                fprintf(error, "compount statement!\n");
                printTree(currentNode, 0);*/
                
                /* get the first statement node */
                currentNode = tree->child[2]->child[0];
                
                /* iterate through the statements */
                while(currentNode)
                {
                    /* handle different statement types */
                    switch(currentNode->type.rule)
                    {
                        
                        /* handle selection statements */
                        case selection_statement:
                        {
                            
                            /* generate the evaluation header */
                            aChunk = newAssemblyChunk();
                            aChunk->preamble = new string("* Selection Statement. evaluate condition");
                            if (!body) body = newAssemblyCode();
                            appendChunk(body, aChunk);
                            
                            /* generate evaluation code */
                            aCode = evaluateExpression(currentNode->child[2], scope);
                            
                            /* add the evaluation code to the existing code */
                            bCode = appendCode(body, aCode);
                            body = disconnectCode(body);
                            aCode = disconnectCode(aCode);
                            body = bCode;
                            
                            /* handle compound statements in result block */
                            if (currentNode->child[4]->type.rule == compound_statement)
                            {
                                
                                /* set up compund statement header */
                                aChunk = newAssemblyChunk();
                                aChunk->preamble = new string("* pushing frame for compound statement");
                                iList = newInstructionList();
                                memOffset = scope->symbols->nextOffset-2;
                                inst = newInstruction(RegisterMemoryInstruction, NoneR, Store,
                                                      FP, memOffset, FP,
                                                      new string( "push ofp"));
                                iList = appendInstruction( iList, inst);
                                inst = newInstruction(RegisterMemoryInstruction, NoneR, LoadAddress,
                                                      FP, memOffset, FP,
                                                      new string( "push frame"));
                                iList = appendInstruction( iList, inst);
                                aChunk->iList = iList;
                                appendChunk(body, aChunk);

                                /* generate code for the compound statement */
                                aCode = compileAST(currentNode->child[4], scope->subScopes[subScope], 0);
                                subScope++;
                            }
                            
                            /* handle standalone statements */
                            else
                            {
                                /* I don't think this works... yet */
                                /* generate code for the statement */
                                aCode = compileAST(currentNode, scope, 0);
                            }
                            
                            /* count up the new lines of code */
                            lines = instructionsInAssembly(aCode);
                            
                            /* add code to branch around statement body if the condition is false */
                            aChunk = newAssemblyChunk();
                            iList = newInstructionList();
                            inst = newInstruction(RegisterMemoryInstruction, NoneR, JumpEqual,
                                                  RES, lines, PC,
                                                  new string( "jump around if body if condition false"));
                            iList = appendInstruction( iList, inst);
                            aChunk->iList = iList;
                            appendChunk(body, aChunk);

                            /* add the body code */
                            bCode = appendCode(body, aCode);
                            body = disconnectCode(body);
                            aCode = disconnectCode(aCode);
                            body = bCode;

                            /* generate code to pop the frame off the stack */
                            aChunk = newAssemblyChunk();
                            iList = newInstructionList();
                            inst = newInstruction(RegisterMemoryInstruction, NoneR, Load,
                                                  FP, 0, FP,
                                                  new string( "pop frame"));
                            iList = appendInstruction( iList, inst);
                            aChunk->iList = iList;
                            appendChunk(body, aChunk);

                            /* finish with this statement */
                            break;
                        }
                        
                        /* handle expression statements */
                        case expression_statement:
                        {
                            /* get the expression node */
                            subNode = currentNode->child[0];
                            
                            /* generate code to evaluate the expression */
                            aCode = evaluateExpression(subNode, scope);
                            
                            /* add this code to the existing code */
                            if (!body)
                                body = aCode;
                            else
                            {
                                bCode = appendCode(body, aCode);
                                body = disconnectCode(body);
                                aCode = disconnectCode(aCode);
                                body = bCode;
                            }
                            
                            /* finish with this statement */
                            break;
                        }
                        
                        /* handle return statements */
                        case return_statement:
                        {
                            /*fprintf(error, "return statement!\n");
                            printTree(currentNode, 0);*/
                            
                            /* get the node for the expression to return */
                            subNode = currentNode->child[1];
                            
                            /* check if there is an expression to evaluate */
                            if (!(subNode->terminal && subNode->type.tType == endOfLine))
                            {
                                /*fprintf(error, "evaluating return expression\n");
                                printTree(subNode, 0);*/
                                
                                /* generate code for the expression */
                                aCode = evaluateExpression(subNode, scope);
                                
                                /* fprintf(error, "evaluation code:\n");
                                printCode(error, aCode); */
                                
                                if (!body)
                                {
                                    body = aCode;
                                    aCode = NULL;
                                }
                                else
                                {
                                    bCode = appendCode(body, aCode);
                                    body = disconnectCode(body);
                                    aCode = disconnectCode(aCode);
                                    body = bCode;
                                }
                            }
                            else
                                aCode = NULL;
                            
                            /* set up code containers */
                            if (!aCode) aCode = newAssemblyCode();
                            aChunk = newAssemblyChunk();
                            iList = newInstructionList();
                            
                            /* add code to return to function top level scope */
                            for (i = 0; i < scope->returnScopes; i++)
                            {
                                inst = newInstruction( RegisterMemoryInstruction, NoneR, Load,
                                                       FP, 0, FP,
                                                       new string( "pop frame"));
                                iList = appendInstruction(iList, inst);
                            }
                            
                            /* add code to return to the caller*/
                            inst = newInstruction(RegisterMemoryInstruction, NoneR, Load,
                                                  PC, -1, FP,
                                                  new string( "return to caller"));
                            iList = appendInstruction(iList, inst);
                            aChunk->iList = iList;
                            appendChunk(aCode, aChunk);
                            if (!body)
                            {
                                body = aCode;
                                aCode = NULL;
                            }
                            else
                            {
                                bCode = appendCode(body, aCode);
                                body = disconnectCode(body);
                                aCode = disconnectCode(aCode);
                                body = bCode;
                                bCode = NULL;
                            }

                            /* finish with this statement */
                            break;
                        }
                    }
                    
                    /* advance to the next top-level node */
                    currentNode = currentNode->sibling;
                }
                
                /* return all of the code for this tree */
                return body;
                break;
            }
            
            /* complain for unknown node types */
            default :
            {
                fprintf(error, "Invalid node encountered while parsing AST.\n");
                
                /* print out the tree so it can be debugged */
                printTree(currentNode, 0);
                
                /* don't return any code for it */
                /* this will allow the compiler to attempt to keep going */
                return NULL;
            }
        }
    }
    
    /* complain if a null tree was passed */
    else if (scope)
    {
        fprintf(error, "Cannot compile a null tree.\n");
        return NULL;
    }
    
    /* complain if a null scope was passed */
    else if (tree)
    {
        fprintf(error, "Cannot compile with a null scope.\n");
        return NULL;
    }
    
    /* complain if both are null */
    else
    {
        fprintf(error, "Cannot compile a null tree with a null scope.\n");
        return NULL;
    }
}

/* utility functions */

/**
* Function to check if a string contains only numeric characters
* returns 1 if it is numeric, and 0 if not.
*/
int isNumeric(char * str, int len)
{
    /* initialize variables  and pointers */
    char * chr = NULL;
    int i = 0;
    
    /* grab the first character */
    chr = str;
    
    /* if the firct character is a - sign */
    if (*chr == '-')
    {
        /* advance on,*/
        chr++;
        i++;
    }
    
    /* iterate over each character */
    while(chr && i < len)
    {
        /* if it is not a digit, return false */
        if (!isdigit(*chr))
            return 0;
        
        /* move to the next character */
        chr++;
        i++;
    }
    
    /* if we have reached this point, the string is entirely numeric */
    return 1;
}