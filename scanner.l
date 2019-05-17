%{

/* Declarations section */
#include <stdio.h>
#include <string.h> 
#include <stdlib.h>
#include <stdbool.h>
#include "fanC.hpp"

#define BYTE_MAX_VALUE (255)




%}

%option yylineno
%option noyywrap


%x C_COMMENT
%x C_STRING_TYPE1
%x C_STRING_TYPE2

%%

"void"			{return VOID;}
"int"			{return INT;}
"byte" 			{return BY}



%%

void error(const char* errorType){
	printf("%s\n",errorType);
	exit(0);
}

void errorAscii(){
	printf("Error undefined escape sequence %c\n",yytext[1]);
	exit(0);
}

void errorChar(){
	printf("Error %s\n",yytext);
	exit(0);
}

int indexof(const char* str,char c){
	int i=0;
	while('\0'!=str[i]){
		if(str[i]==c)
			return i;
		i++;
	}
	return -1;
}

void addCharToBuffer(char c){
	stringBuffer[stringIndex]=c;
	stringIndex++;
	stringBuffer[stringIndex]='\0';
	CRFlag = false;
}


void convertEscapings(){
	
	char c = yytext[1];
	if(c == 't'){
		addCharToBuffer('\t');
	}else if(c == 'r'){
		addCharToBuffer('\r');
		CRFlag = true;
	}else if(c == 'n' /*&& !CRFlag*/){
		addCharToBuffer('\n');
	}else{
		if(c=='\\')
			addCharToBuffer('\\');
	}
	
}

void convertToAscii(){
	int i,ascii;
	char c,tempChar;
	i=indexof(yytext,'x');
	if(i<0) i=0;
	ascii=strtol(yytext+i+1, NULL, 16);
	
	
	if( 32<= ascii && ascii<127){
			tempChar=(char)ascii;
			addCharToBuffer(tempChar);
	}
	
}

void initStringBuffer(){
	stringIndex=0;
}
void addSubStringToBuffer(){
	int i,j;
	for(i=stringIndex,j=0;j<yyleng && i<STRING_MAX_SIZE;i++,j++){
		stringBuffer[i]=yytext[j];
	}
	stringIndex+=yyleng;
	stringBuffer[stringIndex]='\0';
	CRFlag = false;
}

void showString(){
	printf("%d STRING %s\n",yylineno,stringBuffer);
}


void initCommentCounter(){
	commentCounter=1;
}

void incCommentCounter(){
	commentCounter++;
}

void showComment(){
	printf("%d COMMENT %d\n",yylineno,commentCounter);
}

void showToken(const char * name){
		printf("%d %s %s\n",yylineno,name,yytext);
        
}


