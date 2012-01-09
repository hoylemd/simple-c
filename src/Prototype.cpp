/**
 * implementation of Prototype
 */

# include "Prototype.h"

/**
 * Prototype constructor
 */
Prototype * newPrototype(string * name, VarKind retType, TreeNode * paramsList)
{
    /* get the parameter list */
    TreeNode * currentParam = paramsList;

    /* allocate memory */
    Prototype * p = (Prototype *) malloc(sizeof(Prototype));

    /* save the name */
    p->name = name;

    /* set the return type */
    p->type = retType;

    /* initialize and fill the parameters list */
    p->numParams = 0;
    while(currentParam)
    {
        if (currentParam->type.tType == comma)
            currentParam = currentParam->sibling;
        else
        {
            p->params[p->numParams] = currentParam->varType;
            p->numParams++;
            currentParam = currentParam->sibling;
        }
    }

    /* return the new struct */
    return p;
}

/**
* Prototype destructor
*/
Prototype * destroyPrototype(Prototype * p)
{
    /* ensure the prototype exists */
    if (p)
    {
        /* delete objects */
        delete p->name;
        p->name = NULL;
        
        /* free memory */
        free(p);
    }
    
    return NULL;
}
