/**
 * Implementation of TreeNode class
 */

#include "TreeNode.h"

/**
* Tree Node Constructor
*/
TreeNode * newTreeNode(string * token, TerminalType tt, Rule r)
{
    /* intitialize variables */
    int i = 0;
    
    /* allocate memory */
    TreeNode * ntn = (TreeNode *) malloc(sizeof(TreeNode));
    
    /* initialize node pointers */
    ntn->childCount = 0;
    for(i=0; i<MAXCHILDREN; i++)
        ntn->child[i] = NULL;
    ntn->sibling = NULL;
    
    /* initialize line number */
    ntn->line = 0;

    ntn->varType = errorType;
    
    /* if a token is specified */
    if(token && token->size())
    {
        /* set it to a terminal type */
        ntn->terminal = TRUE;
        ntn->type.tType = tt;
        
        /* save the token */
        ntn->tokenValue = token;
    }
    
    /* if no token specified */
    else
    {
        /* save as a rule type */
        ntn->terminal = FALSE;
        ntn->type.rule = r;
    }
    
    /* return a pointer to the new node */
    return ntn;
}

/**
* Rule node constructor
*/
TreeNode * newRuleNode(Rule r)
{
    /* wrap the tree node constructor for a rule */
    return newTreeNode(NULL,nada,r);
}

/**
* Terminal node constructor
*/
TreeNode * newTerminalNode(string * token, TerminalType tt)
{
    /* wrap the tree node constructor for a terminal */
    return newTreeNode(token,tt,none);
}

/**
* Tree Node Destructor
*/
TreeNode * freeTreeNode(TreeNode * ftn)
{
    /* initialize variables */
    int i = 0;
    
    /* ensure the given node exists */
    if (ftn)
    {
        /* clear the pointers */
        for(i=0;i<ftn->childCount;i++)
            ftn->child[i] = NULL;
        ftn->sibling = NULL;
        
        /* free the memory */
        free(ftn);
    }
    
    return NULL;
}

/**
* Function to add a node as a child of another
*/
void addChild(TreeNode * tree, TreeNode * child)
{
    /* ensure both exist and aren't the same one */
    if((tree && child) && tree !=child)
    {
        /* and if the parent has room */
        if(tree->childCount < MAXCHILDREN)
        {
            /* add it to the child list */
            tree->child[tree->childCount] = child;
            
            /* add it as a sibling to existing children, if any */
            if(tree->childCount > 0) tree->child[tree->childCount-1]->sibling = child;
            
            /* increment the counter */
            tree->childCount++;
        }
        else
        {
            /* panic if too many children assigned.*/
            /* A very human response */
            fprintf(error, "Too many children.\n");
            exit(0);
        }
    }
    
    /* panic if an invalid child provided */
    else
    {
        /* Again, very human */
        fprintf(error,"attempt to add an invalid child.\n");
        exit(0);
    }
}

/**
* Function to print out the contents of a node
* currently only does it if it's an id or number
*/
void printNode(TreeNode * tree)
{
    /* check if it's a terminal */
    if(tree->terminal)
    {
        /* switch based on terminal type */
        switch(tree->type.tType)
        {
            case nada:
                break;
            case endfile:
                break;
            case ifToken:
                break;
            case elseToken:
                break;
            case intToken:
                break;
            case voidToken:
                break;
            case returnToken:
                break;
            case whileToken:
                break;
            case id:
            {
                /* print the prefix for identifiers */
                fprintf(error, "id: ");

                /* print the type */
                fprintf(error, "(%d) ", tree->varType);
                break;
            }
            case number:
            {
                /* print the prefix for numeric literals */
                fprintf(error, "number: ");
                
                /* print the type */
                fprintf(error, "(%d) ", tree->varType);
                break;
            }
            case mth_plus:
                break;
            case mth_minus:
                break;
            case mth_multiply:
                break;
            case mth_divide:
                break;
            case cnd_equal:
                break;
            case cnd_notEqual:
                break;
            case cnd_lessOrEqual:
                break;
            case cnd_moreOrEqual:
                break;
            case cnd_less:
                break;
            case cnd_more:
                break;
            case assign:
                break;
            case endOfLine:
                break;
            case comma:
                break;
            case openBrace:
                break;
            case closeBrace:
                break;
            case openSquare:
                break;
            case closeSquare:
                break;
            case openParen:
                break;
            case closeParen:
                break;
            default:
                break;
        }
        /* print out the value */
        fprintf(error, "%s", tree->tokenValue->data());
    }
    
    /* handle nonterminals */
    else
    {
        /* for rules, check which one and print it out */
        switch(tree->type.rule)
        {
            case none:
                break;
            case program:
            {
                fprintf(error, "program");
                break;
            }
            case declaration:
            {
                fprintf(error, "declaration");
                break;
            }
            case declaration_list:
            {
                fprintf(error, "declaration_list");
                break;
            }
            case function_declaration:
            {
                fprintf(error, "function_declaration");
                break;
            }
            case params:
            {
                fprintf(error, "params");
                break;
            }
            case param:
            {
                fprintf(error, "param");
                break;
            }
            case compound_statement:
            {
                fprintf(error, "compound_statement");
                break;
            }
            case local_declarations:
            {
                fprintf(error, "local_declarations");
                break;
            }
            case variable_declaration:
            {
                fprintf(error, "variable_declaration");
                break;
            }
            case type_specifier:
            {
                fprintf(error, "type_specifier");
                break;
            }
            case statement_list:
            {
                fprintf(error, "statement_list");
                break;
            }
            case statement:
            {
                fprintf(error, "statement");
                break;
            }
            case selection_statement:
            {
                fprintf(error, "selection_statement");
                break;
            }
            case iteration_statement:
            {
                fprintf(error, "iteration_statement");
                break;
            }
            case return_statement:
            {
                fprintf(error, "return_statement");
                break;
            }
            case expression_statement:
            {
                fprintf(error, "expression_statement");
                break;
            }
            case expression:
            {
                fprintf(error, "expression");
                
                /* print the type */
                fprintf(error, " (%d)", tree->varType);
                break;
            }
            case simple_expression:
            {
                fprintf(error, "simple_expression");
                
                /* print the type */
                fprintf(error, " (%d)", tree->varType);
                break;
            }
            case relop:
            {
                fprintf(error, "relop");
                break;
            }
            case additive_expression:
            {
                fprintf(error, "additive_expression");
                
                /* print the type */
                fprintf(error, " (%d)", tree->varType);
                break;
            }
            case addop:
            {
                fprintf(error, "addop");
                break;
            }
            case term:
            {
                fprintf(error, "term");
                
                /* print the type */
                fprintf(error, " (%d)", tree->varType);
                break;
            }
            case mulop:
            {
                fprintf(error, "mulop");
                break;
            }
            case factor:
            {
                fprintf(error, "factor");
                
                /* print the type */
                fprintf(error, " (%d)", tree->varType);
                break;
            }
            case var:
            {
                fprintf(error, "var");
                
                /* print the type */
                fprintf(error, " (%d)", tree->varType);
                break;
            }
            case call:
            {
                fprintf(error, "call");
                
                /* print the type */
                fprintf(error, " (%d)", tree->varType);
                break;
            }
            case args:
            {
                fprintf(error, "args");
                break;
            }
            default:
            {
                fprintf(error, "wat? %d",tree->type.rule);
                break;
            }
        }
    }
}

/**
* Function to print out a tree
*/
void printTree(TreeNode * tree, int depth)
{
    /* initialize variables */
    int i = 0;
    
    /* indent */
    for(i=0; i<depth;i++)fprintf(error,"\t");
    
    /* print this node */
    printNode(tree);
    fprintf(error,"\n");
    
    /* print the children. */
    for(i=0; i<tree->childCount; i++)
        printTree(tree->child[i],depth+1);
}