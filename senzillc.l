/*************************************************************************** 
Scanner for the Simple language 
Author: Anthony A. Aaby
Modified by: Jordi Planes
***************************************************************************/ 

%option nounput
%option noinput

%{ 
/*========================================================================= 
C-libraries and Token definitions 
=========================================================================*/ 
#include <string.h> /* for strdup */ 
#include <stdlib.h> /* for atoi */ 
#include "senzillc.tab.h" /* for token definitions and yylval */ 
%} 

/*========================================================================= 
TOKEN Definitions 
=========================================================================*/ 
DIGIT [0-9] 
ID [a-z][a-z0-9]*
COMMENT "//".*\n
STRING \".*\"

/*========================================================================= 
REGULAR EXPRESSIONS defining the tokens for the Simple language 
=========================================================================*/ 
%% 

":=" { return(ASSGNOP); } 
">=" { return(GRTOEQ); }
"<=" { return(LWROEQ); }
"!=" { return(NOTEQ); }
{DIGIT}+ { yylval.intval = strtol(yytext, NULL, 10); return(NUMBER); }
{DIGIT}+"."{DIGIT}+ { yylval.rval = strtof(yytext, NULL); return(RNUMBER); }
do { return(DO); } 
else { return(ELSE); } 
end { return(END); } 
fi { return(FI); } 
if { return(IF); }
and { return(ANDOP); }
or { return(OROP); }
in { return(IN); } 
integer { return(INTEGER); } 
real { return(REAL); }
string { return(STRING); }
let { return(LET); } 
read { return(READ); } 
skip { return(SKIP); } 
then { return(THEN); } 
while { return(WHILE); } 
write { return(OUT); }
length { return(LENGTH); }
{ID} { yylval.id = (char *) strdup(yytext); return(IDENTIFIER); }
{STRING} { yylval.strval.str = (char *) strndup(&yytext[1], strlen( yytext - 2 ) ); 
	yylval.strval.len = strlen(yytext - 2); return(CHARS); }
[ \t\n]+ /* eat up whitespace */ 
{COMMENT} /* ignore comments */
. { return(yytext[0]); } 

%% 

int yywrap(void){ 
   return 0; 
} 

/************************** End Scanner File *****************************/ 

