CC = g++
CodeFiles = y.tab.c lex.yy.c main.cpp TreeNode.cpp assemble.cpp Scope.cpp SymbolTable.cpp Prototype.cpp \
Instruction.cpp InstructionList.cpp AssemblyChunk.cpp AssemblyCode.cpp
HeaderFiles = globals.h common.h TreeNode.h assemble.h Scope.h SymbolTable.h Prototype.h Instruction.h \
InstructionList.h AssemblyChunk.h AssemblyCode.h
ObjectFiles = y.tab.o lex.yy.o main.o TreeNode.o assemble.o Scope.o SymbolTable.o Prototype.o Instruction.o \
InstructionList.o AssemblyChunk.o AssemblyCode.o

all : cm tm

full: clean ${CodeFiles} ${HeaderFiles}
	${CC} -c y.tab.c -o y.tab.o -pedantic
	${CC} -c lex.yy.c -o lex.yy.o -pedantic
	${CC} -c main.cpp -o main.o -pedantic
	${CC} -c TreeNode.cpp -o TreeNode.o -pedantic
	${CC} -c assemble.cpp -o assemble.o -pedantic
	${CC} -c Scope.cpp -o Scope.o -pedantic
	${CC} -c SymbolTable.cpp -o SymbolTable.o -pedantic
	${CC} -c Prototype.cpp -o Prototype.o -pedantic
	${CC} -c Instruction.cpp -o Instruction.o -pedantic
	${CC} -c InstructionList.cpp -o InstructionList.o -pedantic
	${CC} -c AssemblyChunk.cpp -o AssemblyChunk.o -pedantic
	${CC} -c AssemblyCode.cpp -o AssemblyCode.o -pedantic
	${CC} ${ObjectFiles} -o cm -pedantic

cm : ${CodeFiles} ${HeaderFiles}
	${CC} ${CodeFiles} -o cm -pedantic -g

tm :
	cd vm && $(MAKE)
	cp vm/tm .

lex.yy.c : cminus.l
	flex cminus.l
	
y.tab.c : cminus.y
	yacc -d --verbose --debug cminus.y
	
clean : 
	rm -f lex.yy.c y.tab.c y.tab.h *~ *.o cm *.output out*.txt output.tm tm valgrind-results.txt
	cd vm && $(MAKE) clean
	cd cminusCode && rm -f *~

tidy :
	rm -f cminus.tab.c cminus.tab.h *.0
	
test : clean cm tm tidy
	./cm cminusCode/1.cm output.tm
	diff output.tm template.tm

memtest : clean cm
	valgrind --log-file=valgrind-results.txt ./cm cminusCode/1.cm output.txt 