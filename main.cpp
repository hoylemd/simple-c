/**
* program to compile c-minus source code into a c-minus virtual machine program
*/

#include "main.h"

/**
 * Main driver function.
 */
int main(int argc, char * argv[])
{
    /* initialize variables and pointers */
	Prototype * proto = NULL;
	TreeNode * dummyTN = NULL;
	string * file;
	int numInstructions = 0, NIwidth = 2, i = 0, cont = 1;
	AssemblyChunk * aChunk = NULL;
	AssemblyCode * header = NULL, * body = NULL, * fullCode = NULL;
	InstructionList * iList = NULL;
	Instruction * inst = NULL, * inst2 = NULL, * mainInstruction = NULL;
    
    /* if arguments are given use them as file inputs */
	if (argc >2)
	{
		/* source file */
		source = fopen(argv[1], "r");
		
		/* output file */
		out = fopen(argv[2], "w");

        /* error file */
        if (DEBUGMODE)
            error = stderr;

        /* if debugging is off, just throw the messages away */
        else
            error = fopen("/dev/null","w");
	
		/* handle -a and -s flags */
        /* TODO: refactor this to be less ugh */
		if(argc > 3)
			if (!strcmp("-a", argv[3]))
				PrintTreeFlag = 1;
			else if (!strcmp("-s", argv[3]))
				PrintTableFlag =1;
		if(argc > 4)
			if (!strcmp("-a", argv[4]))
				PrintTreeFlag = 1;
			else if (!strcmp("-s", argv[4]))
				PrintTableFlag =1;
	
		/* set up the global scope */
		global = newScope(new string( "global"), NULL);
		
		/* switch to global scope */
		currentScope = global;
		
		/* add the symbols for input and output */
		/*input */
		proto = (Prototype *) malloc(sizeof(Prototype));
		proto->name = new string("input");
		proto->type = intType;
		proto->numParams = 0;
		
		global->symbols->identifiers[0] = new string("input");
		global->symbols->types[0] = intType;
		global->symbols->functions[0] = proto;
		global->symbols->lineNo[0] = 8;
		global->symbols->arraySizes[0] = 1;
		global->symbols->numSymbols++;
		
		/* output*/
		proto = (Prototype *) malloc(sizeof(Prototype));
		proto->name = new string("output");
		proto->type = voidType;
		proto->params[0] = intType;
		proto->numParams = 1;
		
		global->symbols->identifiers[1] = new string("output");
		global->symbols->types[1] = voidType;
		global->symbols->functions[1] = proto;
		global->symbols->lineNo[1] = 11;
		global->symbols->arraySizes[1] = 1;
		global->symbols->numSymbols++;
		
		/* generate the AST */
		yyparse();
		
		/* emit assembly code */
		if (!errorEncountered)
		{
            string * tempstr = NULL;
            
			/* point file to the file name, moving past all directories */
			file = new string(argv[1]);
			tempstr = new string(file->substr(file->find_last_of("/")+1));
            delete file;
            file = tempstr;
            tempstr = NULL;
			
			/* set up header code */
			header = newAssemblyCode();
			
			/* set up initialization code section */
			aChunk = newAssemblyChunk();
			aChunk->preamble = new string(
                    "* C-Minus Compilation to TM Code\n* File: ");
            aChunk->preamble->append(*file);
            aChunk->preamble->append("\n* Standard prelude:");
			iList = newInstructionList();
            inst = newInstruction(RegisterMemoryInstruction, NoneR,
                                  Load, GP, 0, 0, new string( "load gp with maxaddress"));
			iList = appendInstruction(iList, inst);

            inst = newInstruction(RegisterMemoryInstruction, NoneR,
                                  LoadAddress, FP, 0, GP, new string( "copy gp to fp"));
			iList = appendInstruction(iList, inst);

            inst = newInstruction(RegisterMemoryInstruction, NoneR,
                                  Store, AC, 0, 0, new string( "clear location 0"));
			iList = appendInstruction(iList, inst);

            inst = newInstruction(RegisterMemoryInstruction, NoneR, Store, FP, global->symbols->nextOffset,
                                  FP, new string( "store global pointer"));
			iList = appendInstruction(iList, inst);

            inst = newInstruction(RegisterMemoryInstruction, NoneR, LoadAddress, FP,
                                  global->symbols->nextOffset, FP, new string( "advance to main frame"));
			iList = appendInstruction(iList, inst);

            inst = newInstruction(RegisterMemoryInstruction, NoneR, LoadAddress, AC, 1, PC,
                                  new string( "load ac with ret ptr"));
            iList = appendInstruction(iList, inst);
            
			inst = newInstruction(RegisterMemoryInstruction, NoneR, LoadConstant, PC, 0, 0,
                                  new string( "jump to main"));
			iList = appendInstruction(iList, inst);

            /* save the location of thie main redirect */
            mainInstruction = inst;
			
			/* push main to the stack list */
            getScopeByName(new string( "main"), global);

            inst = newInstruction(RegisterInstruction, Halt, NoneRM, 0, 0, 0, new string( "End program"));
			iList = appendInstruction(iList, inst);
			aChunk->postamble = new string("* Jump around i/o routines here");
			aChunk->iList = iList;
			
			/* add this code to the header */
			appendChunk(header, aChunk);

			/* set up input function */
			aChunk = newAssemblyChunk();
			aChunk->preamble = new string("* code for input routine");
			iList = newInstructionList();
            inst = newInstruction(RegisterMemoryInstruction, NoneR, Store, AC, -1, FP,
                                  new string( "store return"));
			iList = appendInstruction(iList, inst);
            inst = newInstruction(RegisterInstruction, Input, NoneRM, RES, 0, 0,
                                  new string( "input"));
			iList = appendInstruction(iList, inst);
            inst = newInstruction(RegisterMemoryInstruction, NoneR, Load, PC, -1, FP,
                                  new string( "return to caller"));
			iList = appendInstruction(iList, inst);
			aChunk->iList = iList;
			
			/* add it to the header */
			appendChunk(header, aChunk);
			
			/* set up output function */
			aChunk = newAssemblyChunk();
			aChunk->preamble =new string("* code for output routine");
			iList = newInstructionList();
            inst = newInstruction(RegisterMemoryInstruction, NoneR, Store, AC, -1, FP,
                                  new string( "store return"));
			iList = appendInstruction(iList, inst);
            inst = newInstruction(RegisterMemoryInstruction, NoneR, Load, AC, -2, FP,
                                  new string( "load output value"));
			iList = appendInstruction(iList, inst);
            inst = newInstruction(RegisterInstruction, Output, NoneRM, AC, 0, 0,
                                  new string( "output"));
			iList = appendInstruction(iList, inst);
            inst = newInstruction(RegisterMemoryInstruction, NoneR, Load, PC, -1, FP,
                                  new string( "return to caller"));
			iList = appendInstruction(iList, inst);
			aChunk->postamble = new string("* End of standard prelude");
			aChunk->iList = iList;
			
			/* add it to the header */
			appendChunk(header, aChunk);

            /* push the global scope */
			pushScope(global);
			
			/* generate the program code */
			body = compileAST(AST, global, instructionsInAssembly(header));
			
			/* connect header and body */
			fullCode = appendCode(header, body);
			
			/* find main and update the redirect */
            mainInstruction->o2 = global->symbols->lineNo[indexOfSymbol(new string( "main"),
                                                                        global->symbols)];
			
			/* output the code to the file */
			printCode(out,fullCode);
		}
		
	}
	else
	{
		/* incorrect call message */
		fprintf(stderr, "Incorrect useage. call with ./cm inputfile outputfile [-a]\n");
	}
	
}