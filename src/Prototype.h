/**
 * prototype header
 */

#ifndef _PROTOTYPE_H_
#define _PROTOTYPE_H_

#include "globals.h"
#include "TreeNode.h"

/**
* Structure for a function prototype
*/
typedef struct funProto
{
    /* the name of the function */
    string * name;

    /* return type */
    VarKind type;

    /* parameter types */
    VarKind params[MAXCHILDREN];
    int numParams;
} Prototype;

/* Prototype functions */
/* Constructor */ 
Prototype * newPrototype(string *, VarKind, TreeNode *);

/* Destructor */
Prototype * destroyPrototype(Prototype *);

#endif