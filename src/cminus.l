%{
	#include <stdio.h>
	#include "y.tab.h"

    #include "globals.h"

    /**
     * wrap routine. Just exit on EOF
     * extern "C" stuff is so that it'll link properly
     */
    extern "C" {
        int yywrap()
        {
            return 1;
        }
    }
%}

/* macros */
letter			[a-zA-Z]
digit			[0-9]
id				{letter}+
number			{digit}+
whitespace		[ \t]+
newline			\n
openComment		\/\*
closeComment 	\*\/

%%

else				{return ELSE;}
if					{return IF;}
int					{return INT;}
void				{return VOID;}
while				{return WHILE;}
return				{return RETURN;}
{id}				{return ID;}
{number}			{return NUMBER;}
{whitespace}		{/*skip whitespace*/}
{newline}			{lineno++;}
\+					{return PLUS;}
\-					{return MINUS;}
\*					{return MULTIPLY;}
\/					{return DIVIDE;}
\=\=				{return EQUAL;}
\!\=				{return NOTEQUAL;}
\<\=				{return LESSOREQUAL;}
\>\=				{return MOREOREQUAL;}
\<					{return LESS;}
\>					{return MORE;}
\=					{return ASSIGN;}
;					{return ENDOFLINE;}
,					{return COMMA;}
\{					{return OPENBRACE;}
\}					{return CLOSEBRACE;}
\[					{return OPENSQUARE;}
\]					{return CLOSESQUARE;}
\(					{return OPENPAREN;}
\)					{return CLOSEPAREN;}
{openComment}.*{closeComment}	{/*skip comments*/}

%%

/**
 * Function to get token data from the next bit
 */
int getToken()
{
    /* initialize variables */
    int placeholder = 0;
    static int firstTime = TRUE;

    /* set up io streams on first run */
    if (firstTime)
    {
		firstTime = FALSE;
		lineno++;
		yyin = source;
		yyout = out;
    }
	
	/* grab the next token */
    placeholder = yylex();

    /* save identifiers and literals in the global field */
    if (placeholder == ID || placeholder == NUMBER) 
    {
		/* if this is an id or number, put it in the token field */
	    strncpy(tokenString, yytext, MAXTOKENLEN);
    }

    /* return the token type */
    return placeholder;
}