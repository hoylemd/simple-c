/**
 * Header for Abstract Syntax Tree nodes
 */

#ifndef _TREENODE_H_
#define _TREENODE_H_

#include <stdio.h>
#include <stdlib.h>

#include "globals.h"
#include "string.h"
#include "string"

#define MAXCHILDREN 64

using namespace std;

/* enumeration for rule node types */
typedef enum rule{
    none,
    program,
    declaration_list,
    declaration,
    function_declaration,
    params,
    param,
    compound_statement,
    local_declarations,
    variable_declaration,
    type_specifier,
    statement_list,
    statement,
    selection_statement,
    iteration_statement,
    return_statement,
    expression_statement,
    expression,
    simple_expression,
    relop,
    additive_expression,
    addop,
    term,
    mulop,
    factor,
    var,
    call,
    args
} Rule;

/* enumeration for terminal types */
typedef enum terminalType{
    nada,
    endfile,
    ifToken,
    elseToken,
    intToken,
    voidToken,
    returnToken,
    whileToken,
    id,
    number,
    mth_plus,
    mth_minus,
    mth_multiply,
    mth_divide,
    cnd_equal,
    cnd_notEqual,
    cnd_lessOrEqual,
    cnd_moreOrEqual,
    cnd_less,
    cnd_more,
    assign,
    endOfLine,
    comma,
    openBrace,
    closeBrace,
    openSquare,
    closeSquare,
    openParen,
    closeParen,
    bad
} TerminalType;

/* union for node types */
typedef union nodeType
{
    Rule rule;
    TerminalType tType;
} NodeType;

/* enumeration for variable types */
typedef enum varKind{
    intType, voidType, errorType
} VarKind;

/**
* structure for tree nodes
*/
typedef struct treeNode
{
    /* child and sibling pointers */
    struct treeNode * child[MAXCHILDREN];
    int childCount;
    struct treeNode * sibling;
    
    /* line number */
    int line;
    
    /* terminal flag */
    int terminal;
    
    /* node and evaluation types */
    NodeType type;
    VarKind varType;
    
    /* the token value */
    string * tokenValue;
} TreeNode;

/* function prototypes */

/** Constructors */
TreeNode * newTreeNode(string *, TerminalType, Rule);
TreeNode * newRuleNode(Rule );
TreeNode * newTerminalNode(string *, TerminalType);

/** Destructor */
TreeNode * freeTreeNode(TreeNode *);

/** Child Adder */
void addChild(TreeNode *, TreeNode *);

/** Printers */
void printNode(TreeNode *);
void printTree(TreeNode *, int);

#endif