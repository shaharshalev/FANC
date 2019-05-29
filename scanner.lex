%{

/* Declarations section */
#include <stdio.h>
#include <string.h> 
#include <stdlib.h>
#include <stdbool.h>
#include "output.hpp"
#include "parser.hpp"
#include "parser.tab.hpp"
using namespace FanC;
%}

%option yylineno
%option noyywrap
whitespace 			([\x20\x09\x0A\x0D])
comment             (\/\/.*)



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
(<|>|<=|>=)                         {yylval = new Relop(yytext); return RELATIONAL;}
(==|!=)              				{yylval = new Relop(yytext); return EQUALITY;}
(\*|\/)                             {yylval = new Multiplicative(yytext); return MULTIPLICATIVE;}
(\+|-)					        	{yylval = new Additive(yytext);return ADDITIVE;}
[a-zA-Z][a-zA-Z0-9]*				{yylval = new Id(yytext); return ID;}
(0|[1-9][0-9]*)						{yylval = new Number(yytext,new Type());return NUM;}
\"([^\n\r\"\\]|\\[rnt"\\])+\"		{yylval = new String(yytext); return STRING;}
{whitespace}						;
<<EOF>>								exit(0);
{comment}                           ;
.									{errorLex(yylineno);exit(0);}






%%


/*Code*/


