%{

/* Declarations section */
#include <stdio.h>
#include <string.h> 
#include <stdlib.h>
#include <stdbool.h>
#include "fanC.hpp"
#include "attributes.h"

#define BYTE_MAX_VALUE (255)




%}

%option yylineno
%option noyywrap
whitespace 			([\x20\x09\x0A\x0D])


%x C_COMMENT
%x C_STRING_TYPE1
%x C_STRING_TYPE2

%%

"void"								{return VOID;}
"int"								{return INT;}
"byte" 								{return BYTE;}
"b"									{return B;}
"bool"								{return BOOL;}
"and"								{return AND;}
"or"								{return OR;}
"not"								{return NOT;}
"true"								{return TRUE;}
"false"								{return FALSE;}
"return"							{return RETURN;}
"if"								{return IF;}
"else"								{return ELSE;}
"while"								{return WHILE;}
"break"								{return BREAK;}
"continue"							{return CONTINUE;}
"@pre"								{return PRECOND;}
";"									{return SC;}
","									{return COMMA;}
"("									{return LPAREN;}
")"									{return RPAREN;}
"{"									{return LBRACE;}
"}"									{return RBRACE;}
"="									{return ASSIGN;}
(=|!=|<|>|<=|>=)					{return RELOP;}
(\+|-|\*|\/)						{return BINOP;}
[a-zA-Z][a-zA-Z0-9]*				{return ID;}
(0|[1-9][0-9]*)						{return NUM;}
\"([^\n\r\"\\]|\\[rnt"\\])+\"		{return STRING;}
{whitespace}						;
<<EOF>>								;
.									errorLex(yylineno);






%%

/*Code*/


