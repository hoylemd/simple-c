
all : binaries

binaries :
	cd src && $(MAKE)
	cp src/tm src/cm .
	
clean : 
	cd src && $(MAKE) clean
	rm -f tm cm valgrind-results.txt *~ *.tm
	
test : clean binaries 
	./cm cm-src/1.cm output.tm
	diff output.tm test/template.tm

memtest : clean binaries
	valgrind --log-file=valgrind-results.txt ./cm cm-src/1.cm output.tm
	diff output.tm test/template.tm
