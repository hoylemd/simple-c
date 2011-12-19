/* test simple-c program */
/* adds 5 to a number and outputs the result */

byte one;
byte two;
byte answer;

/* set up operands */
one << 5;
two << $IN;

/* calculate */
answer << two + one;

/* output */
$OUT << answer;