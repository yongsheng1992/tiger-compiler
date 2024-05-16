%{
#include <string.h>
#include "util.h"
#include "tokens.h"
#include "errormsg.h"

int charPos = 1;
int comment_depth = 0;

int yywrap(void)
{
    charPos = 1;
    return 1;
}


void adjust(void)
{
    EM_tokPos=charPos;
    charPos+=yyleng;
}

%}
ID          ([a-zA-Z][_a-zA-Z0-9]*)
INT         ([0-9]+)

%x COMMENT STRING

%%
[ \t]               {adjust(); continue;}
\n	                {adjust(); EM_newline(); continue;}
","	                {adjust(); return COMMA;}
":"                 {adjust(); return COLON;}
";"                 {adjust(); return SEMICOLON;}
"("                 {adjust(); return LPAREN;}
")"                 {adjust(); return RPAREN;}
"["                 {adjust(); return LBRACK;}
"]"                 {adjust(); return RBRACK;}
"{"                 {adjust(); return LBRACE;}
"}"                 {adjust(); return RBRACE;}
"."                 {adjust(); return DOT;}
"+"                 {adjust(); return PLUS;}
"-"                 {adjust(); return MINUS;}
"*"                 {adjust(); return TIMES;}
"/"                 {adjust(); return DIVIDE;}
"="                 {adjust(); return EQ;}
"!="                {adjust(); return NEQ};
"<"                 {adjust(); return LT;}
"<="                {adjust(); return LE;}
">"                 {adjust(); return GT;}
">="                {adjust(); return GE;}
"&"                 {adjust(); return AND;}
"|"                 {adjust(); return OR;}
":="                {adjust(); return ASSIGN;}
.	                {adjust(); EM_error(EM_tokPos,"illegal token"); yyterminate();}

array               {adjust(); return array}
if                  {adjust(); return IF;}
then                {adjust(); return THEN;}
else                {adjust(); return ELSE;}
while               {adjust(); return WHILE;}
to                  {adjust(); return TO;}
do                  {adjust(); return DO;}
let                 {adjust(); return LET;}
in                  {adjust(); return IN;}
end                 {adjust(); return END;}
of                  {adjust(); return OF;}
break               {adjust(); return BREAK;}
nil                 {adjust(); return NIL;}
function            {adjust(); return FUNCTION;}
var                 {adjust(); return VAR;}
type                {adjust(); return TYPE;}
for  	            {adjust(); return FOR;}

{INT}	            {adjust(); yylval.ival=atoi(yytext); return INT;}
{ID}                {adjust();yylval.sval=String(yytext); return ID;}

/* handle comment */
"/*"                {adjust(); comment_depth++; BEGIN(COMMENT);}
<COMMENT>"/*"       {adjust(); comment_depth++; continue;}
<COMMENT>\n         {adjust(); EM_newline(); continue;}
<COMMENT><<EOF>>    {adjust(); EM_error(EM_tokPos,"EOF in comment"); yyterminate();}
<COMMENT>"*/"       {adjust(); comment_depth--; if(comment_depth == 0) BEGIN(INITIAL); }
<COMMENT>.          {adjust(); continue;}

/* handle string*/
\"                  {adjust(); BEGIN(STRING);}
<STRING>\n          {adjust(); EM_newline(); continue;}
<STRING><<EOF>>     {adjust(); EM_error(EM_tokPos,"EOF in string"); yyterminate();}
<STRING>\"          {adjust(); BEGIN(INITIAL);}
<STRING>
%%

int main() {

}