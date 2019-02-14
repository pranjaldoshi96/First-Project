%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define RUN 1
#define CREATE 0
int yylex();
int yyerror(char *s);
extern FILE *yyin;
extern int yylineno;
extern char yytext[1000];
char output[10000];
int is_err = 0;

%}

%union{
    int a;
    float floatValue;
    char *stringValue;
}
%token <floatValue> I_NUM
%token <stringValue> ID
%token <stringValue> CH
%token INT FLOAT CHAR
%token IF 
%token <floatValue> F_NUM
%left '/' '*'
%left '+' '-'
%right '='
%right '>' '<' 
%right LE GE EQ NE

%%

start:	function	{strcat(output,"RET\n");}
     |	declaration	{strcat(output,"RET\n");}
	;
declaration:	type expr
	|	expr
	   |	error
	;
expr:	ID'='ID';'	{
						char temp[100];
						sprintf(temp,"MOV %s,%s \n",$1,$3); 
						strcat(output,temp);
					}
	|	ID'='I_NUM';'{
						char temp[100];	
						sprintf(temp,"MOV %s,%g\n",$1,$3); 
						strcat(output,temp);
					}
	|	ID'='CH';'{
						char temp[100];	
						sprintf(temp,"MOV %s,%s\n",$1,$3); 
						strcat(output,temp);
					}
	|	ID'='ID'/'ID';'{
						char temp[100];		sprintf(temp,"DIV %s,%s\n",$3,$5); 	
						char temp1[100];	sprintf(temp1,"MOV %s,%s\n",$1,$3);		
						strcat(output,temp);	strcat(output,temp1);
						}
	|	ID'='ID'/'I_NUM';'{
						char temp[100];		sprintf(temp,"DIV %s,%g\n",$3,$5); 	
						char temp1[100];	sprintf(temp1,"MOV %s,%s\n",$1,$3);		
						strcat(output,temp);	strcat(output,temp1);
						}
	|	ID'='I_NUM'/'I_NUM';'{
						char temp[100];		sprintf(temp,"DIV %g,%g\n",$3,$5);
						float res = (float)$3/$5; 	
						char temp1[100];	sprintf(temp1,"MOV %s,%g\n",$1,res);		
						strcat(output,temp);	strcat(output,temp1);
						}
	|	ID'='ID'*'ID';'{
						char temp[100];	sprintf(temp,"MUL %s,%s\n",$3,$5); 	
						char temp1[100];	sprintf(temp1,"MOV %s,%s\n",$1,$3);		
						strcat(output,temp);	strcat(output,temp1);
						}
	|	ID'='ID'*'I_NUM';'{
						char temp[100];		sprintf(temp,"MUL %s,%g\n",$3,$5); 	
						char temp1[100];	sprintf(temp1,"MOV %s,%s\n",$1,$3);		
						strcat(output,temp);	strcat(output,temp1);
						}
	|	ID'='I_NUM'*'I_NUM';'{
						char temp[100];		sprintf(temp,"MUL %g,%g\n",$3,$5);
						float res = (float)$3*$5; 	
						char temp1[100];	sprintf(temp1,"MOV %s,%g\n",$1,res);		
						strcat(output,temp);	strcat(output,temp1);
						}

	|	ID'='ID'+'ID';'{
						char temp[100];	sprintf(temp,"ADD %s,%s\n",$3,$5); 	
						char temp1[100];	sprintf(temp1,"MOV %s,%s\n",$1,$3);		
						strcat(output,temp);	strcat(output,temp1);
						}
	|	ID'='ID'+'I_NUM';'{
						char temp[100];		sprintf(temp,"ADDV %s,%g\n",$3,$5); 	
						char temp1[100];	sprintf(temp1,"MOV %s,%s\n",$1,$3);		
						strcat(output,temp);	strcat(output,temp1);
						}
	|	ID'='I_NUM'+'I_NUM';'{
						char temp[100];		sprintf(temp,"ADD %g,%g\n",$3,$5);
						float res = $3+$5; 	
						char temp1[100];	sprintf(temp1,"MOV %s,%g\n",$1,res);		
						strcat(output,temp);	strcat(output,temp1);
						}

	|	ID'='ID'-'ID';'{
						char temp[100];	sprintf(temp,"SUB %s,%s\n",$3,$5); 	
						char temp1[100];	sprintf(temp1,"MOV %s,%s\n",$1,$3);		
						strcat(output,temp);	strcat(output,temp1);
						}
	|	ID'='ID'-'I_NUM';'{
						char temp[100];		sprintf(temp,"SUB %s,%g\n",$3,$5); 	
						char temp1[100];	sprintf(temp1,"MOV %s,%s\n",$1,$3);		
						strcat(output,temp);	strcat(output,temp1);
						}
	|	ID'='I_NUM'-'I_NUM';'{
						char temp[100];		sprintf(temp,"SUB %g,%g\n",$3,$5);
						float res = $3-$5; 	
						char temp1[100];	sprintf(temp1,"MOV %s,%g\n",$1,res);		
						strcat(output,temp);	strcat(output,temp1);
						}
	|	ID'='ID'>'ID';'	{
						char temp[100];	sprintf(temp,"GREATER %s,%s\n",$3,$5); 		
						strcat(output,temp);
						}
	|	ID'='ID'<'ID';'	{char temp[100];	sprintf(temp,"SMALLER %s,%s\n",$3,$5); 		
						strcat(output,temp);}
	|	ID'='ID GE ID';'	{char temp[100];	sprintf(temp,"GE %s,%s\n",$3,$5); 		
							strcat(output,temp);}
	|	ID'='ID LE ID';'	{char temp[100];	sprintf(temp,"LE %s,%s\n",$3,$5); 		
							strcat(output,temp);}
	|	ID'='ID EQ ID';'	{char temp[100];	sprintf(temp,"EQ %s,%s\n",$3,$5); 		
							strcat(output,temp);}
	|	ID'='ID NE ID';'	{char temp[100];	sprintf(temp,"NE %s,%s\n",$3,$5); 		
							strcat(output,temp);}


    |	ID';'
	;
type:	INT
	|	FLOAT
	|	CHAR
    ;
function:	type ID '('')' cstmt
	;
stmt:	declaration
	|	condition
    ;
stmtlist:	stmtlist stmt
	|
	;
cstmt:	'{'stmtlist'}'
     ;
condition:	IF '(' if_cond ')' cstmt
	;
if_cond:	ID'='ID
	|	num'>'num	
	|	num'<'num
	|	num GE num
	|	num LE num
	|	num EQ num
	|	num NE num
	|	num
	;
num:	I_NUM
	|	ID
	|	CH
	;
%%

#include <ctype.h>

int main(int argc, char *argv[]){
	memset(output,'\0',10000);
	int show_assembly = RUN;
	if(argc == 3){
		show_assembly = argv[2][0] - '0';
	}
	yyin = fopen(argv[1],"r");
	if(yyin == NULL){
		printf("File Does not exist\n");
		exit(0);
	}
	yyparse();
	if(!is_err){	
		if(show_assembly == RUN){
			printf("%s",output);
			printf("Number of lines : %d\n",yylineno-1);
		}
		else{
			printf("NO ERROR\n");
		}
	}
	fclose(yyin);
	return 0;
}

int yyerror(char *s){
	is_err = 1;
	printf("%d : %s \n",yylineno,s);
	return 0;
}
