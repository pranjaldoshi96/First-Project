%{
#include <stdio.h>
#include <string.h>
#include "y.tab.h"

%}
DIGIT [0-9]



%%
[ \t]	;
[\n]	{yylineno = yylineno+1;}
int	return INT;
float	return FLOAT;
char	return CHAR;
if  return IF;
^"#include".+ ;
"<="    {return LE;}
">="    {return GE;}
"=="    {return EQ;}
"!="    {return NE;}
{DIGIT}*		{ yylval.floatValue=atoi(yytext);  return I_NUM;}
{DIGIT}*\.{DIGIT}+  	{ yylval.floatValue=atof(yytext); return I_NUM;}
{DIGIT}+\.{DIGIT}*	{ yylval.floatValue=atof(yytext); return I_NUM;}
\'[a-zA-Z0-9]\' { yylval.stringValue = strdup(yytext); return CH; }
[a-z0-9]*	{ yylval.stringValue = strdup(yytext); return ID; }
\/\/.*	;
\/\*(.*\n)*.*\*\/	;

.	{ return yytext[0];}
%%

int yywrap(){
    return 1;
}