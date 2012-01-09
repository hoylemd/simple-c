/**
 * Yacc grammar and parsing file
 */

%{
    /* includes */
    #include "common.h"

    /* definitions */
    #define DEBUG 0
    #define YYSTYPE nodeOrString
    #define MAXCALLSTACK 16

    /* prototype pointer */
    Prototype * currentPrototype = NULL;
    
    /* call stack */
    Prototype * currentCall = NULL;
    Prototype * callStack[MAXCALLSTACK];
    int callStackLayer = 0;
    
    /* flags for function scope reminders */
    int funScope = 0;
    int funScopeRemember = 0;
    
    /* Yacc grammar type */
    typedef union{
        TreeNode * node;
        string * String;
    } nodeOrString;

    /**
     * Error handling routine
     */
    int yyerror(const char *str)
    {
        /* print out the error */
        fprintf(stderr,"error: %s on line %d\n", str, lineno);

        /* set the flag */
        errorEncountered = 1;
    
        return 0;
    }


    /**
     * Driver routine
     */
    static int yylex(void)
    {
        return getToken();
    }

    /**
     * Function to check if an identifier has been declared
     */
    int assertDeclared(string * id, Scope * scope)
    {
        /* check the symbol exists at this scope */
        if (!matchSymbol(id, scope))
        {
            /* print out the error */
            fprintf(stderr, "Undefined identifier '%s' at line %d\n", id->data(), lineno);

            /* set the flag and return an error */
            errorEncountered = 1;
            return 1;
        }
        return 0;
    }

    /**
     * Function to add a new symbol to the table.
     * Checks for redeclaration.
     */
    void checkSymbol(TreeNode * typeSpec, string * iden, Scope * scope, Prototype * proto, int size)
    {

        /* check for redefinition*/
        if (matchSymbol(iden, currentScope))
        {
            /* print error message */
            fprintf(stderr, "Redeclared identifier '%s' at line %d\n", iden->data(), lineno);

            /* set the flag */
            errorEncountered = 1;
        }

        /* if the symbol is not defined, add it */
        else
        {
            /* check for array size */
            if (size < 0)
            {
                /* print error message */
                fprintf(stderr, "invalid array size specified at line %d\n", lineno);

                /* set flag */
                errorEncountered = 1;

                /* set size to zero */
                /* this prevents redundant undeclared identifier variables */
                size = 0;
            }

            /* add the new identifier to the table */
            addSymbol(typeSpec, iden, scope, proto, size);
        }
    }
    
    /**
     * Function to get the type of an identifier by it's name
     */
    VarKind getIdentifierType(string * iden, Scope * scope)
    {
        /* initialize variables */
        int i = 0;

        /* ensure the given scope exists */
        if (scope)
        {
            /* iterate through the symbol list */
            for(i = 0; i < scope->symbols->numSymbols; i++)
            {
                /* if a matching symbol is found, return it */
                if (!iden->compare(*scope->symbols->identifiers[i]))
                {
                    return scope->symbols->types[i];
                }
            }
    
            /* if it's not found at this scope, try one level above */
            return getIdentifierType(iden, scope->parent);
        }

        /* if nothing found, return errorType */
        return errorType;
    }
    
    /**
     * Function to get a function prototype by it's name
     */
    Prototype * getPrototypeByName(string * iden, Scope * scope)
    {
        /* initialize variables */
        int i = 0;
    
        /* ensure the given scope exists */
        if (scope)
        {
            /* iterate through the symbol list */
            for(i=0; i < scope->symbols->numSymbols; i++)
            {
                /* if a matching symbol is found, return it's prototype */
                if (!iden->compare(*scope->symbols->identifiers[i]))
                    return scope->symbols->functions[i];
            }
    
            /* if it's not found at this scope, try one level above */
            return getPrototypeByName(iden, scope->parent);
    
        }
    
        /* if nothing's found return null */
        return NULL;
    }
    
    /**
     * Function to push a call to the stack
     */
    void pushCall(Prototype * call)
    {
        /* if the stack isn't too big */
        if (callStackLayer < (MAXCALLSTACK -1))
        {
            /* push it */
            callStack[callStackLayer] = call;
            callStackLayer++;
        }

        /* handle stack overflow */
        else
        {
            /* panic if the stack is too big */
            fprintf(stderr, "Too many calls - stack overflow\n");
            exit (0);
        }
    }
    
    /**
     * Function to pop a call from the stack.
     */
    Prototype * popCall()
    {
        /* initialize pointer */
        Prototype * p = NULL;
    
        /* ensure calls are on the stack */
        if (callStackLayer > 0)
        {
            /* get the top call */
            callStackLayer--;
            p = callStack[callStackLayer];
        }
        else
        {
            /* if there's nothing on the stack, return that */
            return NULL;
        }

    }
    
    /**
     * Function to move context down into a function call
     */
    Prototype * contextDown(string * func, Scope * scope, Prototype * currCall)
    {
        /* push the current function to the stack, if any */
        if (currCall)
            pushCall(currentCall);

        /* return the call's prototype */
        return getPrototypeByName(func, scope);
    }

    /*
     * Function to move context up out of a function call
     */
    Prototype * contextUp()
    {
        /* wrap around the pop function */
        return popCall();
    }
    
    
    /*
     * Function to type-check call arguments
     */
    int argumentsCheck(Prototype * call, TreeNode * arguments)
    {
        /* initialize variables */
        int i = 0;
        TreeNode * currentArg = NULL;
        VarKind currentParam = voidType;

        /* complain if no call was provided */
        if (!call)
        {
            /* print error message */
            fprintf(error, "Cannot check arguments for no function.\n");

            /* raise error flag and return 0 */
            errorEncountered = 1;
            return 0;
        }

        /* handle case where no arguments given */
        if(!arguments)
        {
            if (call->numParams == 0)
                /* if the function wasn't expecting any its ok */
                return 0;

            /* if it was expecting arguments*/
            else
            {
                /* print error message */
                fprintf(error, "No arguments specified for function %s on line %d Expected: %d\n",
                        call->name->data(), lineno, call->numParams);

                /* raise error flag and return */
                errorEncountered = 1;
                return 0;
            }
        }

        /* grab the arguments tree */
        else
            currentArg = arguments->child[0];
    
        /* step through the arguments list */
        while (currentArg)
        {
            /* advance past the commas */
            if (currentArg->type.tType == comma)
                currentArg = currentArg->sibling;
            else
            {
                /* if there should be another argument */
                if (i < call->numParams)
                {
                    /* get the next param type*/
                    currentParam = call->params[0];
    
                    /* check them */
                    if (currentParam != currentArg->varType)
                    {
                        /* print error */
                        fprintf(error, "Type mismatch in argument %d on line %d\n", i, lineno);

                        /* raise flag */
                        errorEncountered = 1;
                    }
    
                    /* advance */
                    currentArg = currentArg->sibling;
                    i++;
                }
                else
                {
                    /* if there are too many arguments */
                    while (currentArg)
                    {
                        /* count up the rest */
                        currentArg = currentArg->sibling;
                        i++;
                    }
    
                    /* report error */
                    fprintf(error, "too many arguments(%d) for function %s on line %d. Expected: %d\n",
                            i, call->name->data(), lineno, call->numParams);

                    /* raise flag */
                    errorEncountered = 1;
                    break;
                }
            }
        }
    
        /* if not enough arguments have been given */
        if (i < call->numParams)
        {
            /* print error */
            fprintf(error, "not enough arguments(%d) for function %s on line %d. Expected: %d\n",
                    i, call->name->data(), lineno, call->numParams);

            /* raise flag */
            errorEncountered = 1;
        }
    }
%}

/* bookkeeping tokens */
/* %token BAD */

/* keyword tokens */
%token IF ELSE INT VOID RETURN WHILE

/* value tokens */
%token ID NUMBER 

/* math operator tokens */
%token PLUS MINUS MULTIPLY DIVIDE

/* comparison operator tokens */ 
%token EQUAL NOTEQUAL LESSOREQUAL MOREOREQUAL LESS MORE ASSIGN

/* punctuation tokens */
%token OPENBRACE CLOSEBRACE OPENSQUARE CLOSESQUARE OPENPAREN CLOSEPAREN ENDOFLINE COMMA

/* expect one shift/reduce (floating else) */
%expect 2
%%

/* root rule */
program:
    declaration_list
    {
        /* initialize pointer */
        TreeNode * temp = NULL;

        /* debugging message */
        if(DEBUG) fprintf(error,"program\n");

        /* spawn the node */
        $$.node = newRuleNode(declaration_list);
    
        /* iterate through $1's siblings (the list of declarations)
            and add them all to this node as children*/
        temp = $1.node;
        while(temp)
        {
            addChild($$.node, temp);
            temp = temp->sibling;
        }
    
        /* print the global symbol list, If required */
        if (PrintTableFlag)
        {
            fprintf(stderr, "Global Symbol List:\n");
            printSymbols(global->symbols);
        }
    
        /* print the completed tree out, if required. */
        if (PrintTreeFlag)
            printTree($$.node,0);
    
        /* save the AST */
        AST = $$.node;
    }
    ;
    
/* lister for declarations */
/* returns a pointer to the first declaration in a sibling-linked 
    list of declarations */
declaration_list:
    /* handle lists */
    declaration_list declaration
    {
        /* initialize pointer */
        TreeNode * temp = NULL;

        /* debug message */
        if (DEBUG) fprintf(error,"declaration list+\n");

        /* handle both parts */
        if ($1.node && $2.node)
        {
            /* find the last declaration in the $1 list */
            temp = $1.node;
            while(temp->sibling) temp = temp->sibling;

            /* append the new one ($2) to the list */
            temp->sibling = $2.node;

            /* return a pointer to the first one */
            $$.node = $1.node;
        }

        /* handle only the list */
        else if ($1.node)
                $$ = $1;

        /* handle single declaration */
        else if ($2.node)
                $$ = $2;
    }
    |
    /* handle individual declarations */
    declaration
    {
        /* debug message */
        if (DEBUG) fprintf(error,"declaration list\n");

        /* just return the pointer */
        $$ = $1;
    }
    ;
    
/* abstraction of variable and function declarations */
declaration:
    variable_declaration
    {
        /* debug message */
        if (DEBUG) fprintf(error,"declaration - variable\n");

        /* return the declaration */
        $$ = $1;
    }
    |
    function_declaration
    {
        /* debug message */
        if (DEBUG) fprintf(error,"declaration - function\n");

        /* return the declaration */
        $$ = $1;
    }
    ;

/* function declarations */
function_declaration:
    type_specifier ID
    {
        /* initialize pointers */
        Scope * tempScope = NULL;

        /* create the scope and switch to it*/
        tempScope = newScope(new string(tokenString), currentScope);
        currentScope = tempScope;
        funScope = 1;
    
        /* grab the value of the id */
        $$.String = new string(tokenString);
    
    }
    OPENPAREN params
    {
        /* initialize temporary variables */
        VarKind type = errorType;

        /* get the return type */
        if ($1.node->type.tType = intToken)
            type = intType;
        else if ($1.node->type.tType = voidToken)
            type = voidType;

        /* create the prototype */
        currentPrototype = newPrototype($3.String, type, $5.node->child[0]);

        /* add this symbol to the table */
        checkSymbol($1.node, $3.String, currentScope->parent, currentPrototype, 1);
    
    }
    CLOSEPAREN compound_statement
    {
        /* debug message */
        if (DEBUG) fprintf(error,"function declaration\n");

        /* spawn the new node */
        $$.node = newRuleNode(function_declaration);

        /* add the parts */
        addChild($$.node,$1.node);
        addChild($$.node,newTerminalNode($3.String,id));
        addChild($$.node,newTerminalNode(new string("("),openParen));
        addChild($$.node,$5.node);
        addChild($$.node,newTerminalNode(new string(")"),closeParen));
        addChild($$.node,$8.node);
    
        /* print out the symbol table */
        if (PrintTableFlag)
        {
            fprintf(stderr, "Leaving scope '%s'\n", currentScope->name->data());
            printSymbols(currentScope->symbols);
        }
        /* return from this scope */
        currentScope = currentScope->parent;
        funScope = 0;
    
        currentPrototype = NULL;
    }
    ;

/* function parameters */
params:
    param_list
    {
        /* initialize pointer */
        TreeNode * temp = NULL;

        /* debug message */
        if (DEBUG) fprintf(error,"params\n");

        /* spawn the new node */
        $$.node = newRuleNode(params);

        /* add each param from the list */
        temp = $1.node;
        while(temp)
        {
            addChild($$.node, temp);
            temp = temp->sibling;
        }
    }
    |
    VOID
    {
        /* no parameters */
        /* debug message */
        if (DEBUG) fprintf(error,"void params\n");

        /* return a void token */
        $$.node = newTerminalNode(new string("void"),voidToken);
    }
    ;
    
/* sibling-linked parameter list */
param_list:
    param_list COMMA param
    {
        /* initialize pointer */
        TreeNode * temp = NULL;

        /* debug message */
        if (DEBUG) fprintf(error,"param_list, \n");

        /* find the last param in the list */
        temp = $1.node;
        while(temp->sibling) temp = temp->sibling;
    
        /* add a comma and the next param*/
        temp->sibling = newTerminalNode(new string(","),comma);
        temp = temp->sibling;
        temp->sibling = $3.node;

        /* return the first param */
        $$.node = $1.node;
    }
    |
    param
    {
        /* base case*/
        /* debug message */
        if (DEBUG) fprintf(error,"param_list\n");

        /* pass the param through */
        $$ = $1;
    }
    ;

/* function parameter */
param:
    type_specifier ID
    {
        /* debug message */
        if (DEBUG) fprintf(error,"simple param\n");

        /* spawn the new node */
        $$.node = newRuleNode(param);

        /* add the terminals */
        addChild($$.node, $1.node);
        addChild($$.node, newTerminalNode(new string(tokenString),id));
    
        /* store the type */
        if ($1.node->type.tType == intToken)
            $$.node->varType = intType;
        else
            $$.node->varType = voidType;
    
        /* add this symbol to the table */
        checkSymbol($1.node,new string(tokenString), currentScope, NULL, 1);
    }
    |
    type_specifier ID
    {
        /* array parameter */
        /* grab the id value */
        $$.String = new string(tokenString);
    }
    OPENSQUARE CLOSESQUARE
    {
        /* debug message */
        if (DEBUG) fprintf(error,"array param\n");

        /* spawn the new node */
        $$.node =newRuleNode(param);

        /* add the terminals */
        addChild($$.node, $1.node);
        addChild($$.node, newTerminalNode($3.String,id));
        addChild($$.node, newTerminalNode(new string("["),openSquare));
        addChild($$.node, newTerminalNode(new string("]"),closeSquare));
    
        /* store the type */
        if ($1.node->type.tType == intToken)
            $$.node->varType = intType;
        else
            $$.node->varType = voidType;
    
        /* add this symbol to the table */
        checkSymbol($1.node,$3.String, currentScope, NULL, 0);
    }
    ;

/* function body or Curly brace delimited code block */
compound_statement:
    OPENBRACE
    {
        /* initialize pointers */
        Scope * tempScope = NULL;

        if(!funScope)
        {
            /* if we're not in a function declaration */
            /* make a new scope and switch to it */
            tempScope = newScope(NULL, currentScope);
            currentScope = tempScope;
        } else {
            /* if we're in a function declaration */
            /* clear the flag, remember it, and don't make a new scope */
            funScope = 0;
            funScopeRemember = 1;
        }
        if (PrintTableFlag)
        {
            fprintf(stderr, "Entering scope '%s'\n", currentScope->name->data());
        }
    }
    local_declarations
    {
        /* initialize pointer */
        TreeNode * temp = NULL;

        /* spawn the local declarations node */
        $$.node = newRuleNode(local_declarations);

        /* iterate through the declarations list and add them
            to the node */
        temp = $3.node;
        while(temp)
        {
            addChild($$.node, temp);
            temp = temp->sibling;
        }
    }
    statement_list
    {
        /* initialize pointer */
        TreeNode * temp = NULL;

        /* spawn the statements node */
        $$.node = newRuleNode(statement_list);

        /* iterate through the statements list and add them to
            the node */
        temp = $5.node;
        while(temp)
        {
            addChild($$.node, temp);
            temp = temp->sibling;
        }
    }
    CLOSEBRACE
    {
        /* debug message */
        if (DEBUG) fprintf(error,"compound statement\n");
    
    
        /* spawn the top node */
        $$.node = newRuleNode(compound_statement);

        /* add terminals and lists */
        addChild($$.node, newTerminalNode(new string("{"),openBrace));
        addChild($$.node, $4.node);
        addChild($$.node, $6.node);
        addChild($$.node, newTerminalNode(new string("}"),closeBrace));
    
        /* return from this scope */
        if (!funScopeRemember)
        {
            /* print out the symbol table */
            if (PrintTableFlag)
            {
                fprintf(stderr, "Leaving scope '%s'\n", currentScope->name->data());
                printSymbols(currentScope->symbols);
            }
    
            currentScope = currentScope->parent;
        }
    
        /* forget about the scope changes */
        funScopeRemember = 0;
    }
    ;

/* local variable declarations */
local_declarations:
    local_declarations variable_declaration
    {
        /* initialize pointer */
        TreeNode * temp = NULL;

        /* debug message */
        if (DEBUG) fprintf(error,"Local declarations\n");

        /* grab the existsing declarations list, if any */
        temp = $1.node;
        if(temp)
        {
            /* find the end of the list */
            while (temp->sibling) temp = temp->sibling;

            /* append the new declaration */
            temp->sibling = $2.node;

            /* return the head of the list */
            $$.node = $1.node;
        }
        else
        {
            /* return the declaration */
            $$.node = $2.node;
        }
    }
    |
    {
        /* debug message */
        if (DEBUG) fprintf(error, "empty local declarations list\n");
    
        /* return NULL, signifying no declaration */
        $$.node = NULL;
    }
    ;
    
/* variable declaration */
variable_declaration:
    type_specifier ID
    {
        /* grab the id value */
        $$.String = new string(tokenString);
    }
    ENDOFLINE
    {
        /* debug message */
        if (DEBUG) fprintf(error,"variable declaration\n");

        /* spawn the new node */
        $$.node = newRuleNode(variable_declaration);

        /* add the terminals */
        addChild($$.node, $1.node);
        addChild($$.node, newTerminalNode($3.String, id));
        addChild($$.node, newTerminalNode(new string(";"), endOfLine));
    
        /* add this symbol to the table */
        checkSymbol($1.node,$3.String, currentScope, NULL, 1);
    }
    |
    type_specifier ID
    {
        /* array declarations */
        /* grab the id value */
        $$.String = new string(tokenString);
    }
    OPENSQUARE NUMBER
    {
        /* grab the size value */
        $$.String = new string(tokenString);
    }
    CLOSESQUARE ENDOFLINE
    {
        /* debug message */
        if (DEBUG) fprintf(error,"array variable declaration\n");
    
        /* spawn the new node */
        $$.node = newRuleNode(variable_declaration);

        /* add the terminals */
        addChild($$.node, $1.node);
        addChild($$.node, newTerminalNode($3.String, id));
        addChild($$.node, newTerminalNode(new string("["),openSquare));
        addChild($$.node, newTerminalNode($6.String, number));
        addChild($$.node, newTerminalNode(new string("]"),closeSquare));
        addChild($$.node, newTerminalNode(new string(";"),endOfLine));
    
        /* add this symbol to the table */
        checkSymbol($1.node,$3.String, currentScope, NULL, atoi($6.String->data()));
    }
    |
    error
    {
        $$.node = NULL;
    }
    ;

/* type specifier */
type_specifier:
    INT
    {
        /* debug message */
        if (DEBUG) fprintf(error,"int type specifier\n");

        /* generate and return the terminal */
        $$.node = newTerminalNode(new string("int"),intToken);
    }
    |
    VOID
    {
        /* debug message */
        if (DEBUG) fprintf(error,"void type specifier\n");

        /* generate and return the terminal */
        $$.node = newTerminalNode(new string("void"),voidToken);
    }
    ;

/* sibling-linked list of statements */
statement_list:
    statement_list statement
    {
        /* initialize pointer */
        TreeNode * temp = NULL;

        /* debug message */
        if (DEBUG) fprintf(error,"statement list\n");

        /* grab the existsing list, if any */
        temp = $1.node;
        if(temp)
        {
            /* find the last statement */
            while(temp->sibling) temp = temp->sibling;

            /* append the new statement */
            temp->sibling = $2.node;

            /* return the head of the list */
            $$.node = $1.node;
        }
        else
        {
            /* if no list, return the singular statement.*/
            /* this is the base case */
            $$ = $2;
        }
    }
    |
    {
        /* return NULL signifying no statements */
        $$.node = NULL;
    }
    ;

/* statement abstraction */
statement:
    expression_statement
    {
        /* debug message */
        if (DEBUG) fprintf(error,"expression statment\n");

        /* pass the statement upwards */
        $$ = $1;
    }
    |
    compound_statement
    {
        /* debug message */
        if (DEBUG) fprintf(error,"compound statment\n");

        /* pass the statement upwards */
        $$ = $1;
    }
    |
    selection_statement
    {
        /* debug message */
        if (DEBUG) fprintf(error,"selection statement\n");

        /* pass the statement upwards */
        $$ = $1;
    }
    |
    iteration_statement
    {
        /* debug message */
        if (DEBUG) fprintf(error,"iteration statement\n");

        /* pass the statement upwards */
        $$ = $1;
    }
    |
    return_statement
    {
        /* debug message */
        if (DEBUG) fprintf(error,"return statement\n");

        /* pass the statement upwards */
        $$ = $1;
    }
    ;
    
/* selection (if) statement */
selection_statement:
    IF OPENPAREN expression CLOSEPAREN statement ELSE statement
    {
        /* debug message */
        if (DEBUG) fprintf(error,"if else statement\n");

        /* spawn the new node */
        $$.node = newRuleNode(selection_statement);

        /* add terminals and trees */
        addChild($$.node, newTerminalNode(new string("if"), ifToken));
        addChild($$.node, newTerminalNode(new string("("),openParen));
        addChild($$.node, $3.node);
        addChild($$.node, newTerminalNode(new string(")"),closeParen));
        addChild($$.node, $5.node);
        addChild($$.node, newTerminalNode(new string("else"),elseToken));
        addChild($$.node, $7.node);
    
        /* type the node */
        if($3.node->varType != intType)
        {
            fprintf(error, "attempt to use a void as a condition on line %d\n", lineno);
    
            errorEncountered = 1;
    
        }
    }
    |
    IF OPENPAREN expression CLOSEPAREN statement
    {
        /* debug message */
        if (DEBUG) fprintf(error,"if statement\n");

        /* spawn the new node */
        $$.node = newRuleNode(selection_statement);

        /* add terminals and trees */
        addChild($$.node, newTerminalNode(new string("if"), ifToken));
        addChild($$.node, newTerminalNode(new string("("),openParen));
        addChild($$.node, $3.node);
        addChild($$.node, newTerminalNode(new string(")"),closeParen));
        addChild($$.node, $5.node);
    
        /* type the node */
        if($3.node->varType != intType)
        {
            fprintf(error, "attempt to use a void as a condition on line %d\n", lineno);
    
            errorEncountered = 1;
        }
    }
    ;

/* iteration (while loop) statement */
iteration_statement:
    WHILE OPENPAREN expression CLOSEPAREN statement
    {
        /* debug message */
        if (DEBUG) fprintf(error,"while loop\n");
    
        /* spawn new node */
        $$.node = newRuleNode(iteration_statement);
    
        /* add terminals and trees */
        addChild($$.node, newTerminalNode(new string("while"), whileToken));
        addChild($$.node, newTerminalNode(new string("("),openParen));
        addChild($$.node, $3.node);
        addChild($$.node, newTerminalNode(new string(")"),closeParen));
        addChild($$.node, $5.node);
    
        /* type the node */
        if($3.node->varType != intType)
        {
            fprintf(error, "attempt to use a void as a condition on line %d\n", lineno);
    
            errorEncountered = 1;
        }
    }
    ;

/* return statements */
return_statement:
    RETURN ENDOFLINE
    {
        /* debug message */
        if (DEBUG) fprintf(error,"empty return\n");
    
        /* spawn new node */
        $$.node = newRuleNode(return_statement);
    
        /* add terminals */
        addChild($$.node, newTerminalNode(new string("return"), returnToken));
        addChild($$.node, newTerminalNode(new string(";"), endOfLine));
    
        /* check if we're actually in a function */
        if (!currentPrototype)
        {
            fprintf(error, "Dangling return statement on line %d\n", lineno);
    
            errorEncountered = 1;
        }
    }
    |
    RETURN expression ENDOFLINE
    {
        /* debug message */
        if (DEBUG) fprintf(error,"return\n");
    
        /* spawn new node */
        $$.node = newRuleNode(return_statement);
    
        /* add terminals and trees */
        addChild($$.node, newTerminalNode(new string("return"), returnToken));
        addChild($$.node, $2.node);
        addChild($$.node, newTerminalNode(new string(";"),endOfLine));
    
        /* check if we're actually in a function */
        if (!currentPrototype)
        {
            fprintf(error, "Dangling return statement on line %d\n", lineno);
    
            errorEncountered = 1;
        }
        else
        {
            /* type check the expression */
            if ($2.node->varType != currentPrototype->type)
            {
                fprintf(error, "Mismatched return type (got:%d, needed: %d) on line %d\n",
                        $2.node->varType, currentPrototype->type,  lineno);
    
                errorEncountered = 1;
            }
        }
    

    }
    ;

/* expression statement */
expression_statement:
    expression ENDOFLINE
    {
        /* debug message */
        if (DEBUG) fprintf(error,"expression statement;\n");
    
        /* spawn new node */
        $$.node = newRuleNode(expression_statement);
    
        /* add terminal and tree */
        if ($1.node)
            addChild($$.node, $1.node);
        addChild($$.node, newTerminalNode(new string(";"),endOfLine));
    }
    |
    ENDOFLINE
    {
        /* debug message */
        if (DEBUG) fprintf(error,"expression statement SEMICOLON\n");
    
        /* return terminal */
        $$.node = newTerminalNode(new string(";"),endOfLine);
    }
    ;

/* expression */
expression:
    var ASSIGN expression
    {
        /* debug message */
        if (DEBUG) fprintf(error, "assignment expression\n");
    
        /* spawn new node */
        $$.node = newRuleNode(expression);
    
        /* add terminal and trees */
        addChild($$.node,$1.node);
        addChild($$.node,newTerminalNode(new string("="),assign));
        addChild($$.node,$3.node);
    
        /* type check it */
        if ($1.node->varType != $3.node->varType)
        {
            fprintf(error, "Mismatched types in assignment on line %d\n", lineno);
    
            errorEncountered = 1;
        }
        /* type this node */
        $$.node->varType = intType;
    }
    |
    simple_expression
    {
        /* debug message */
        if (DEBUG) fprintf(error, "simple expression\n");
    
        /* pass upwards the expression */
        $$ = $1;
    }
    ;

/* non-assignment expression */
simple_expression:
    additive_expression relop additive_expression
    {
        /* debug message */
        if (DEBUG) fprintf(error,"additive comparison\n");
    
        /* spawn a new node */
        $$.node = newRuleNode(simple_expression);
    
        /* add the trees */
        addChild($$.node,$1.node);
        addChild($$.node,$2.node);
        addChild($$.node,$3.node);
    
        /* type the node */
        if ($1.node->varType != intType != $3.node->varType)
        {
            fprintf(error, "Type mismatch on line %d. Attemped Assignment using voids.\n", lineno);
    
            errorEncountered = 1;
        }
        $$.node->varType = intType;
    }
    |
    additive_expression
    {
        /* debug message */
        if (DEBUG) fprintf(error,"additive expression\n");
    
        /* pass the expression upwards */
        $$ = $1;
    }
    ;

/* comparison operator */
/* all subrules just pass the terminal upwards */
relop:
    EQUAL
    {
        $$.node = newTerminalNode(new string("=="), cnd_equal);
    }
    |
    NOTEQUAL
    {
        $$.node = newTerminalNode(new string("!="), cnd_notEqual);
    }
    |
    LESSOREQUAL
    {
        $$.node = newTerminalNode(new string("<="), cnd_lessOrEqual);
    }
    |
    MOREOREQUAL
    {
        $$.node = newTerminalNode(new string(">="), cnd_moreOrEqual);
    }
    |
    LESS
    {
        $$.node = newTerminalNode(new string("<"), cnd_less);
    }
    |
    MORE
    {
        $$.node = newTerminalNode(new string(">"), cnd_more);
    }
    ;
    
/* addititve expression */
additive_expression:
    additive_expression addop term
    {
        /* debug message */
        if (DEBUG) fprintf(error,"additive complex\n");
    
        /* spawn new node */
        $$.node = newRuleNode(additive_expression);
    
        /* add trees */
        addChild($$.node,$1.node);
        addChild($$.node,$2.node);
        addChild($$.node,$3.node);
    
        /* type check it */
        if ($1.node->varType != intType != $3.node->varType)
        {
            fprintf(error, "Type mismatch on line %d. Attemped Arithmetic using voids.\n", lineno);
    
            errorEncountered = 1;
        }
        /* type the node */
        $$.node->varType = intType;
    }
    |
    term
    {
        /* debug message */
        if (DEBUG) fprintf(error,"additive term %ld\n", (long)$1.node);
    
        /* pass the term upwards */
        $$ = $1;
    }
    ;

/* additive operator */
addop:
    PLUS
    {
        /* debug message */
        if (DEBUG) fprintf(error,"addop plus\n");
    
        /* pass upwards the terminal */
        $$.node = newTerminalNode(new string("+"), mth_plus);
    }
    |
    MINUS
    {
        /* debug message */
        if (DEBUG) fprintf(error,"addop minus\n");
    
        /* pass upwards the terminal */
        $$.node = newTerminalNode(new string("-"), mth_minus);
    }
    ;
    
/* terms */
term:
    term mulop factor
    {
        /* debug message */
        if (DEBUG) fprintf(error,"multiplicative term\n");
    
        /* spawn a new node */
        $$.node = newRuleNode(term);
    
        /* add the trees */
        addChild($$.node,$1.node);
        addChild($$.node,$2.node);
        addChild($$.node,$3.node);
    
        /* type check it */
        if ($1.node->varType != intType != $3.node->varType)
        {
            fprintf(error, "Type mismatch on line %d. Attemped Arithmetic using voids.\n", lineno);
    
            errorEncountered = 1;
        }
        $$.node->varType = intType;
    }
    |
    factor
    {
        /* debug message */
        if (DEBUG) fprintf(error,"term %ld\n", (long)$1.node);
    
    
        /* pass upwards the factor */
        $$ = $1;
    }
    ;
    
/* multiplicative operator */
/* just passes upwards the terminals */
mulop:
    MULTIPLY
    {
        $$.node = newTerminalNode(new string("*"), mth_multiply);
    }
    |
    DIVIDE
    {
        $$.node = newTerminalNode(new string("/"), mth_divide);
    }
    ;
    
/* factors */
factor:
    OPENPAREN expression CLOSEPAREN
    {
        /* debug message */
        if (DEBUG) fprintf(error,"parenthesized factor\n");
    
        /* spawn the new node */
        $$.node = newRuleNode(factor);
    
        /* add the terminals and tree */
        addChild($$.node,newTerminalNode(new string("["),openParen));
        addChild($$.node,$2.node);
        addChild($$.node,newTerminalNode(new string("]"),closeParen));
    
        /* move up the type */
        $$.node->varType = $2.node->varType;
    }
    |
    var
    {
        /* debug message */
        if (DEBUG) fprintf(error,"variable factor\n");
    
        /* pass upwards the variable */
        $$ = $1;
    }
    |
    call
    {
        /* debug message */
        if (DEBUG) fprintf(error,"factor call\n");
    
        /* pass upwards the call */
        $$ = $1;
    }
    |
    NUMBER
    {
        /* debug message */
        if (DEBUG) fprintf(error,"factor number\n");
    
        /* pass upwards the number terminal */
        $$.node = newTerminalNode(new string(tokenString), number);
    
        /* set the type */
        $$.node->varType = intType;
    }
    ;

/* variable */
var:
    ID
    {
        /* initialize temporary pointer */
        string * tempstring = NULL;

        /* debug message */
        if (DEBUG) fprintf(error,"id\n");
    
        /* pass upwards the terminal */
        $$.node = newTerminalNode(new string(tokenString), id);
    
        /* check if this id has been declared */
        tempstring = new string(tokenString);
        if (!assertDeclared(tempstring, currentScope))
        {
            /* add the type */
            $$.node->varType = getIdentifierType(tempstring, currentScope);
        }
        delete tempstring;
    
    }
    |
    ID
    {
        /* grab the id value */
        $$.String = new string(tokenString);
    }
    OPENSQUARE expression CLOSESQUARE
    {
        /* debug message */
        if (DEBUG) fprintf(error,"id[]\n");
    
        /* spawn the new node */
        $$.node = newRuleNode(var);
    
        /* check if this id has been declared */
        if (!assertDeclared($2.String, currentScope))
        {
            /* add the type */
            $$.node->varType = getIdentifierType($2.String, currentScope);
        }
    
        /* add the terminals and the tree */
        addChild($$.node,newTerminalNode($2.String, id));
        addChild($$.node,newTerminalNode(new string("["),openSquare));
        if ($4.node) addChild($$.node,$4.node);
        else
        {
            fprintf(error, "invalid array size\n");
    
            errorEncountered = 1;
        }
        addChild($$.node,newTerminalNode(new string("]"),closeSquare));
    
        /* type check it */
        if ($4.node->varType != intType)
        {
            fprintf(error, "void used as array index on line %d\n", lineno);
    
            errorEncountered = 1;
        }
    }
    ;

/* function call */
call:
    ID {
        /* initialize temporary pointer */
        string * tempstring = NULL;

        /* grab the id value */
        $$.String = new string(tokenString);
    
        /* move into the call's context */
        tempstring = new string(tokenString);
        if(!(currentCall = contextDown(tempstring, currentScope, currentCall)))
            currentCall = contextUp();
        delete tempstring;
    }
    OPENPAREN args CLOSEPAREN
    {
        /* debug message */
        if (DEBUG) fprintf(error, "function call\n");
    
        /* spawn the new node */
        $$.node = newRuleNode(call);
    
        /* check if this id has been declared */
        if (!assertDeclared($2.String, currentScope))
        {
            /* add the type */
            $$.node->varType = getIdentifierType($2.String, currentScope);
        }
    
        /* add the terminals */
        addChild($$.node,newTerminalNode($2.String,id));
        addChild($$.node, newTerminalNode(new string("("),openParen));
    
        /* add the args tree */
        if($4.node) addChild($$.node, $4.node);
    
        /* typeCheck the args */
        argumentsCheck(currentCall, $4.node);
    
        /* add the closing paren terminal */
        addChild($$.node, newTerminalNode(new string(")"),closeParen));
    
        /* move out of this context */
        currentCall = contextUp();
    }
    |
    error
    {
        $$.node = NULL;
    }
    ;
    
/* argument list */
args:
    arg_list
    {
        /* initialize pointer */
        TreeNode * temp = NULL;

        /* debug message */
        if (DEBUG) fprintf(error,"argument list\n");
    
        /* spawn the new node */
        $$.node = newRuleNode(args);
    
        /* iterate through the argument list and add them all */
        temp = $1.node;
        while(temp)
        {
            addChild($$.node,temp);
            temp = temp->sibling;
        }
    }
    |
    {
        /* return null signifying no args */
        $$.node = NULL;
    }
    ;
    
arg_list:
    arg_list COMMA expression
    {
        /* initialize pointer */
        TreeNode * temp = NULL;

        /* debug message */
        if (DEBUG) fprintf(error,"+arg");
    
        /* find the last arg in the list */
        temp = $1.node;
        while(temp->sibling) temp = temp->sibling;
    
        /* add the comma and expression */
        temp->sibling = newTerminalNode(new string(","),comma);
        temp = temp->sibling;
        temp->sibling = $3.node;
    
        /* return a pointer to the first arg */
        $$.node = $1.node;
    }
    |
    expression
    {
        /* debug message */
        if (DEBUG) fprintf(error,"arg");
    
        /* pass upwards the expression */
        $$ = $1;
    }
    ;