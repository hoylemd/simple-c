* C-Minus Compilation to TM Code
* File: 1.cm
* Standard prelude:
  0:     LD  6,0(0) 	load gp with maxaddress
  1:    LDA  5,0(6) 	copy gp to fp
  2:     ST  0,0(0) 	clear location 0
  3:     ST  5,-3(5) 	store global pointer
  4:    LDA  5,-3(5) 	advance to main frame
  5:    LDA  0,1(7) 	load ac with ret ptr
  6:    LDC  7,52(0) 	jump to main
  7:   HALT  0,0,0 	End program
* Jump around i/o routines here
* code for input routine
  8:     ST  0,-1(5) 	store return
  9:     IN  4,0,0 	input
 10:     LD  7,-1(5) 	return to caller
* code for output routine
 11:     ST  0,-1(5) 	store return
 12:     LD  0,-2(5) 	load output value
 13:    OUT  0,0,0 	output
 14:     LD  7,-1(5) 	return to caller
* End of standard prelude
* function increment
 15:     ST  0,-1(5) 	store return for increment 15
* Selection Statement. evaluate condition
 16:    LDA  2,0(5) 	set the scope pointer
 17:     LD  4,-2(2) 	load variable i
 18:     ST  4,-3(5) 	Storing left operand in left temp
 19:    LDC  4,0(0) 	load constant 0
 20:     ST  4,-4(5) 	Storing right operand in right temp
* Evaluating 'i' == '0'
 21:     LD  2,-3(5) 	Load left operand
 22:     LD  3,-4(5) 	Load right operand
 23:    SUB  2,2,3 	calculate difference
 24:    LDC  4,0(0) 	set null result
 25:    JNE  2,1(7) 	skip next line if ops not equal
 26:    LDC  4,1(0) 	set result to true
* pushing frame for compound statement
 27:     ST  5,-5(5) 	push ofp
 28:    LDA  5,-5(5) 	push frame
 29:    JEQ  4,5(7) 	jump around if body if condition false
 30:    LDA  2,0(5) 	set the scope pointer
 31:     LD  2,0(2) 	raise scope level
 32:     LD  4,-2(2) 	load variable i
 33:     LD  5,0(5) 	pop frame
 34:     LD  7,-1(5) 	return to caller
 35:     LD  5,0(5) 	pop frame
* Call to function increment
 36:    LDA  2,0(5) 	set the scope pointer
 37:     LD  4,-2(2) 	load variable i
 38:     ST  4,-3(5) 	Storing left operand in left temp
 39:    LDC  4,1(0) 	load constant 1
 40:     ST  4,-4(5) 	Storing right operand in right temp
* Evaluating 'i' - '1'
 41:     LD  2,-3(5) 	Load left operator
 42:     LD  3,-4(5) 	Load right operator
 43:    SUB  4,2,3 	Evaluate subtraction
 44:     ST  4,-7(5) 	store argument
 45:     ST  5,-5(5) 	push ofp
 46:    LDA  5,-5(5) 	push frame
 47:    LDA  0,1(7) 	load ac with ret ptr
 48:    LDC  7,15(0) 	jump to function body
 49:     LD  5,0(5) 	pop frame
 50:     LD  7,-1(5) 	return to caller
* return to caller
 51:     LD  7,-1(5) 	return to caller
* function main
 52:     ST  0,-1(5) 	store return for main 52
* Call to function input
 53:     ST  5,-7(5) 	push ofp
 54:    LDA  5,-7(5) 	push frame
 55:    LDA  0,1(7) 	load ac with ret ptr
 56:    LDC  7,8(0) 	jump to function body
 57:     LD  5,0(5) 	pop frame
* Assigning value to 'inc'
 58:    LDA  2,0(5) 	set the scope pointer
 59:    LDA  2,0(6) 	set scope to global
 60:     ST  4,-2(2) 	assign value to local variable
* Call to function input
 61:     ST  5,-7(5) 	push ofp
 62:    LDA  5,-7(5) 	push frame
 63:    LDA  0,1(7) 	load ac with ret ptr
 64:    LDC  7,8(0) 	jump to function body
 65:     LD  5,0(5) 	pop frame
* Assigning value to 'xi'
 66:    LDA  2,0(5) 	set the scope pointer
 67:     ST  4,-2(2) 	assign value to local variable
* Call to function input
 68:     ST  5,-7(5) 	push ofp
 69:    LDA  5,-7(5) 	push frame
 70:    LDA  0,1(7) 	load ac with ret ptr
 71:    LDC  7,8(0) 	jump to function body
 72:     LD  5,0(5) 	pop frame
* Assigning value to 'yi'
 73:    LDA  2,0(5) 	set the scope pointer
 74:     ST  4,-3(2) 	assign value to local variable
 75:    LDA  2,0(5) 	set the scope pointer
 76:     LD  4,-2(2) 	load variable xi
 77:     ST  4,-5(5) 	Storing left operand in left temp
 78:    LDA  2,0(5) 	set the scope pointer
 79:     LD  4,-3(2) 	load variable yi
 80:     ST  4,-6(5) 	Storing right operand in right temp
* Evaluating 'xi' * 'yi'
 81:     LD  2,-5(5) 	Load left operator
 82:     LD  3,-6(5) 	Load right operator
 83:    MUL  4,2,3 	Evaluate multiplication
* Assigning value to 'zi'
 84:    LDA  2,0(5) 	set the scope pointer
 85:     ST  4,-4(2) 	assign value to local variable
* Selection Statement. evaluate condition
 86:    LDA  2,0(5) 	set the scope pointer
 87:     LD  4,-4(2) 	load variable zi
 88:     ST  4,-5(5) 	Storing left operand in left temp
 89:    LDC  4,20(0) 	load constant 20
 90:     ST  4,-6(5) 	Storing right operand in right temp
* Evaluating 'zi' < '20'
 91:     LD  2,-5(5) 	Load left operand
 92:     LD  3,-6(5) 	Load right operand
 93:    SUB  2,2,3 	calculate difference
 94:    LDC  4,0(0) 	set null result
 95:    JGE  2,1(7) 	skip next line if ops greater than or equal
 96:    LDC  4,1(0) 	set result to true
* pushing frame for compound statement
 97:     ST  5,-7(5) 	push ofp
 98:    LDA  5,-7(5) 	push frame
 99:    JEQ  4,19(7) 	jump around if body if condition false
* Call to function increment
100:    LDA  2,0(5) 	set the scope pointer
101:     LD  2,0(2) 	raise scope level
102:     LD  4,-4(2) 	load variable zi
103:     ST  4,-6(5) 	store argument
104:     ST  5,-4(5) 	push ofp
105:    LDA  5,-4(5) 	push frame
106:    LDA  0,1(7) 	load ac with ret ptr
107:    LDC  7,15(0) 	jump to function body
108:     LD  5,0(5) 	pop frame
* Assigning value to 'zi'
109:    LDA  2,0(5) 	set the scope pointer
110:     LD  2,0(2) 	raise scope level
111:     ST  4,-4(2) 	assign value to local variable
* Call to function output
112:    LDC  4,20(0) 	load constant 20
113:     ST  4,-6(5) 	store argument
114:     ST  5,-4(5) 	push ofp
115:    LDA  5,-4(5) 	push frame
116:    LDA  0,1(7) 	load ac with ret ptr
117:    LDC  7,11(0) 	jump to function body
118:     LD  5,0(5) 	pop frame
119:     LD  5,0(5) 	pop frame
* Call to function output
120:    LDA  2,0(5) 	set the scope pointer
121:     LD  4,-4(2) 	load variable zi
122:     ST  4,-9(5) 	store argument
123:     ST  5,-7(5) 	push ofp
124:    LDA  5,-7(5) 	push frame
125:    LDA  0,1(7) 	load ac with ret ptr
126:    LDC  7,11(0) 	jump to function body
127:     LD  5,0(5) 	pop frame
128:     LD  7,-1(5) 	return to caller
* return to caller
129:     LD  7,-1(5) 	return to caller
