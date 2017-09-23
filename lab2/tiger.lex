%{
#include <string.h>
#include "util.h"
#include "tokens.h"
#include "errormsg.h"

int charPos=1;

int yywrap(void)
{
 charPos=1;
 return 1;
}

void adjust(void)
{
 EM_tokPos=charPos;
 charPos+=yyleng;
}
/*
* Please don't modify the lines above.
* You can add C declarations of your own below.
*/

%}
  /* You can add lex definitions here. */

%%
  /* 
  * Below are some examples, which you can wipe out
  * and write reguler expressions and actions of your own.
  */ 

" "  {adjust(); continue;} 
\n	 {adjust(); EM_newline(); continue;}
\t	 {adjust(); continue;}
","	 {adjust(); return COMMA;}
":"  {adjust(); return COLON;}
";"  {adjust(); return SEMICOLON;}
"{"  {adjust(); return LBRACE;}
"}"  {adjust(); return RBRACE;}
"["  {adjust(); return LBRACK;}
"]"  {adjust(); return RBRACK;}
"("  {adjust(); return LPAREN;}
")"  {adjust(); return RPAREN;}
"."  {adjust(); return DOT;}
"+"  {adjust(); return PLUS;}
"-"  {adjust(); return MINUS;}
"*"  {adjust(); return TIMES;}
"/"  {adjust(); return DIVIDE;}
"="  {adjust(); return EQ;}
"<>" {adjust(); return NEQ;}
"<"  {adjust(); return LT;}
"<=" {adjust(); return LE;}
">"  {adjust(); return GT;}
">=" {adjust(); return GE;}
"&"  {adjust(); return AND;}
"|"  {adjust(); return OR;}
":=" {adjust(); return ASSIGN;}
end {adjust(); return END;}
array {adjust(); return ARRAY;}
if {adjust(); return IF;}
then {adjust(); return THEN;}
else {adjust(); return ELSE;}
while {adjust(); return WHILE;}
for {adjust(); return FOR;}
to  {adjust(); return TO;}
do  {adjust(); return DO;}
let {adjust(); return LET;}
in  {adjust(); return IN;}
of  {adjust(); return OF;}
break {adjust(); return BREAK;}
nil {adjust(); return NIL;}
function {adjust(); return FUNCTION;}
var {adjust(); return VAR;}
type {adjust(); return TYPE;}
[0-9]+	 {adjust(); yylval.ival=atoi(yytext); return INT;}
[a-zA-Z][_a-zA-Z0-9]*	{adjust(); yylval.sval=String(yytext); return ID;}
"/*"([^\*]|(\*)*[^\*/])*(\*)*"*/"	{adjust();}
\"(\\.|[^"\\])*\"  {
    int len = strlen(yytext), i=1, j=0;
    adjust();

    if (len==2) {
      yylval.sval=String("(null)");
      return STRING;
    }

    yylval.sval=checked_malloc(len-2);

    while (i<len-1) {
      if (yytext[i]!='\\') {
        yylval.sval[j]=yytext[i];
        i++;
        j++;
      }
      else {
        if (yytext[i+1]=='n') {
          yylval.sval[j]='\n';
        }
        else if (yytext[i+1]=='t') {
          yylval.sval[j]='\t';
        }
        else if (yytext[i+1]=='\\') {
          yylval.sval[j]='\\';
        }
        i+=2;
        j++;
      }
    }
    yylval.sval[j]='\0';

    return STRING;
  }
.	 {adjust(); EM_error(EM_tokPos,"illegal token");}
