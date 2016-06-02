%{
	#include"syntax.tab.h"
	#include"syntaxtree.h"
	static int lexicalerr_occur = 0;
%}
%option yylineno
digit	[0-9]
letter	[a-zA-Z]
_letter	(_|[a-zA-Z])
%%
0|([1-9]{digit}*)	{		//dec-integer
	struct Node* node = getNewNode();//printf(%s",yytext);
	strcpy(node->info.name,"INT");
	node->info.lineno = yylineno;
	node->info.value.INT = (unsigned int)atoi(yytext);
	node->info.nodetype = LEX;
	strcpy(node->info.notation,yytext);
	yylval.type_node = node;
	return INT;
}
0([0-7]+)	{			//oct-integer
	struct Node* node = getNewNode();//printf(%s",yytext);
	strcpy(node->info.name,"INT");
	node->info.lineno = yylineno;
	node->info.value.INT = (unsigned int)strtoul(yytext,NULL,8);
	node->info.nodetype = LEX;
	strcpy(node->info.notation,yytext);
	yylval.type_node = node;
	return INT;
}
(0x|0X)({digit}|[A-F]|[a-f])+	{	//hex-integer
	struct Node* node = getNewNode();//printf(%s",yytext);
	strcpy(node->info.name,"INT");
	node->info.lineno = yylineno;
	node->info.value.INT = (unsigned int)strtoul(yytext,NULL,16);
	node->info.nodetype = LEX;
	strcpy(node->info.notation,yytext);
	yylval.type_node = node;
	return INT;
}
(0|([1-9]{digit}*))\.{digit}+	{	//normal float
	struct Node* node = getNewNode();//printf(%s",yytext);
	strcpy(node->info.name,"FLOAT");
	node->info.lineno = yylineno;
	node->info.value.FLOAT = (float)atof(yytext);
	node->info.nodetype = LEX;
	strcpy(node->info.notation,yytext);
	yylval.type_node = node;
	return FLOAT;
}
(({digit}+\.{digit}*)|({digit}*\.{digit}+))(E|e)[+-]?{digit}+				{		//scientific float
	struct Node* node = getNewNode();//printf(%s",yytext);
	strcpy(node->info.name,"FLOAT");
	node->info.lineno = yylineno;
	node->info.value.FLOAT = (float)atof(yytext);
	node->info.nodetype = LEX;
	strcpy(node->info.notation,yytext);
	yylval.type_node = node;
	return FLOAT;
}
;	{				//SEMI
	struct Node* node = getNewNode();//printf(%s",yytext);
	strcpy(node->info.name,"SEMI");
	node->info.lineno = yylineno;
	node->info.nodetype = LEX;
	yylval.type_node = node;
	return SEMI;
}
,	{				//COMMA
	struct Node* node = getNewNode();//printf(%s",yytext);
	strcpy(node->info.name,"COMMA");
	node->info.lineno = yylineno;
	node->info.nodetype = LEX;
	yylval.type_node = node;
	return COMMA;
}
=	{				//ASSIGNOP
	struct Node* node = getNewNode();//printf(%s",yytext);
	strcpy(node->info.name,"ASSIGNOP");
	node->info.lineno = yylineno;
	node->info.nodetype = LEX;
	yylval.type_node = node;
	return ASSIGNOP;
}
>|<|(>=)|(<=)|(==)|(!=)		{	//RELOP (RELation OPeration)
/*	Type	UNSIGNED INT
	>	1
	<	2
	>=	3
	<=	4
	==	5
	!=	6		*/
	struct Node* node = getNewNode();//printf(%s",yytext);
	strcpy(node->info.name,"RELOP");
	node->info.lineno = yylineno;
	switch(yytext[0])
	{
		case '>':node->info.value.INT = yytext[1]=='='?3:1;	break;
		case '<':node->info.value.INT = yytext[1]=='='?4:2;	break;
		case '=':node->info.value.INT = 5;	break;
		case '!':node->info.value.INT = 6;
	}
	node->info.nodetype = LEX;
	strcpy(node->info.notation,yytext);
	yylval.type_node = node;
	return RELOP;
}
\+	{				//PLUS	
	struct Node* node = getNewNode();//printf(%s",yytext);
	strcpy(node->info.name,"PLUS");
	node->info.lineno = yylineno;
	node->info.nodetype = LEX;
	yylval.type_node = node;
	return PLUS;
}
\-	{				//MINUS
	struct Node* node = getNewNode();//printf(%s",yytext);
	strcpy(node->info.name,"MINUS");
	node->info.lineno = yylineno;
	node->info.nodetype = LEX;
	yylval.type_node = node;
	return MINUS;
}
\*	{				//STAR
	struct Node* node = getNewNode();//printf(%s",yytext);
	strcpy(node->info.name,"STAR");
	node->info.lineno = yylineno;
	node->info.nodetype = LEX;
	yylval.type_node = node;
	return STAR;
}
\/	{				//DIV
	struct Node* node = getNewNode();//printf(%s",yytext);
	strcpy(node->info.name,"DIV");
	node->info.lineno = yylineno;
	node->info.nodetype = LEX;
	yylval.type_node = node;
	return DIV;
}
&&	{				//AND
	struct Node* node = getNewNode();//printf(%s",yytext);
	strcpy(node->info.name,"AND");
	node->info.lineno = yylineno;
	node->info.nodetype = LEX;
	yylval.type_node = node;
	return AND;
}
\|\|	{				//OR
	struct Node* node = getNewNode();//printf(%s",yytext);
	strcpy(node->info.name,"OR");
	node->info.lineno = yylineno;
	node->info.nodetype = LEX;
	yylval.type_node = node;
	return OR;
}
\.	{				//DOT
	struct Node* node = getNewNode();//printf(%s",yytext);
	strcpy(node->info.name,"DOT");
	node->info.lineno = yylineno;
	node->info.nodetype = LEX;
	yylval.type_node = node;
	return DOT;
}
!	{				//NOT
	struct Node* node = getNewNode();//printf(%s",yytext);
	strcpy(node->info.name,"NOT");
	node->info.lineno = yylineno;
	node->info.nodetype = LEX;
	yylval.type_node = node;
	return NOT;
}
(int)|(float)	{			//TYPE
	struct Node* node = getNewNode();//printf(%s",yytext);
	strcpy(node->info.name,"TYPE");
	node->info.lineno = yylineno;
	node->info.value.INT = yytext[0]=='i'?1:2;
	node->info.nodetype = LEX;
	strcpy(node->info.notation,yytext);
	yylval.type_node = node;
	return TYPE;
}
\(	{				//LeftParentheses
	struct Node* node = getNewNode();//printf(%s",yytext);
	strcpy(node->info.name,"LP");
	node->info.lineno = yylineno;
	node->info.nodetype = LEX;
	yylval.type_node = node;
	return LP;
}
\)	{				//RightParentheses
	struct Node* node = getNewNode();//printf(%s",yytext);
	strcpy(node->info.name,"RP");
	node->info.lineno = yylineno;
	node->info.nodetype = LEX;
	yylval.type_node = node;
	return RP;
}
\[	{				//LeftBracket
	struct Node* node = getNewNode();//printf(%s",yytext);
	strcpy(node->info.name,"LB");
	node->info.lineno = yylineno;
	node->info.nodetype = LEX;
	yylval.type_node = node;
	return LB;
}
\]	{				//RightBracket
	struct Node* node = getNewNode();//printf(%s",yytext);
	strcpy(node->info.name,"RB");
	node->info.lineno = yylineno;
	node->info.nodetype = LEX;
	yylval.type_node = node;
	return RB;
}
\{	{				//LeftCurlyBracket
	struct Node* node = getNewNode();//printf(%s",yytext);
	strcpy(node->info.name,"LC");
	node->info.lineno = yylineno;
	node->info.nodetype = LEX;
	yylval.type_node = node;
	return LC;
}
\}	{				//RightCurlyBracket
	struct Node* node = getNewNode();//printf(%s",yytext);
	strcpy(node->info.name,"RC");
	node->info.lineno = yylineno;
	node->info.nodetype = LEX;
	yylval.type_node = node;
	return RC;
}
struct	{				//STRUCT
	struct Node* node = getNewNode();//printf(%s",yytext);
	strcpy(node->info.name,"STRUCT");
	node->info.lineno = yylineno;
	node->info.nodetype = LEX;
	yylval.type_node = node;
	return STRUCT;
}
return	{				//RETURN
	struct Node* node = getNewNode();//printf(%s",yytext);
	strcpy(node->info.name,"RETURN");
	node->info.lineno = yylineno;
	node->info.nodetype = LEX;
	yylval.type_node = node;
	return RETURN;
}
if	{				//IF
	struct Node* node = getNewNode();//printf(%s",yytext);
	strcpy(node->info.name,"IF");
	node->info.lineno = yylineno;
	node->info.nodetype = LEX;
	yylval.type_node = node;
	return IF;
}
else	{				//ELSE
	struct Node* node = getNewNode();//printf(%s",yytext);
	strcpy(node->info.name,"ELSE");
	node->info.lineno = yylineno;
	node->info.nodetype = LEX;
	yylval.type_node = node;
	return ELSE;
}
while	{				//WHILE
	struct Node* node = getNewNode();//printf(%s",yytext);
	strcpy(node->info.name,"WHILE");
	node->info.lineno = yylineno;
	node->info.nodetype = LEX;
	yylval.type_node = node;
	return WHILE;
}
"/*"([*]*(([^*/])+([/])*)*)*"*/" 	{	/*Comonts*/
	/* no action */
}
\/\/.*\n	{				//Comonts
	/* no action */
}
(" ")|(\t)	{			//BLANK (space and tab)
	/* no action */
}
\n	{			//ENDL	(end of line)
	/* no action */
}
{_letter}({_letter}|{digit})*	{	//ID
	struct Node* node = getNewNode();//printf(%s",yytext);
	strcpy(node->info.name,"ID");
	node->info.lineno = yylineno;
	node->info.nodetype = LEX;
	strcpy(node->info.notation,yytext);
	yylval.type_node = node;
	return ID;
}
.	{				//ILLEGAL character
	lexicalerr_occur = 1;
	PrtErrA(yylineno);
	printf("Unknown \"%s\".\n",yytext);
	}

0([0-9]+)	{			//ILLEGAL oct-integer
	lexicalerr_occur = 1;
	PrtErrA(yylineno);
	printf("Illegal oct-integer %s.\n",yytext);
	struct Node* node = getNewNode();//printf(%s",yytext);
	strcpy(node->info.name,"INT");
	node->info.lineno = yylineno;
	node->info.value.INT = (unsigned int)strtoul(yytext,NULL,8);
	node->info.nodetype = LEX;
	strcpy(node->info.notation,yytext);
	yylval.type_node = node;
	return INT;
}
(0x|0X)({digit}|{letter})+	{		//ILLEGAL hex-integer
	lexicalerr_occur = 1;
	PrtErrA(yylineno);
	printf("Illegal hex-integer %s.\n",yytext);
	struct Node* node = getNewNode();//printf(%s",yytext);
	strcpy(node->info.name,"INT");
	node->info.lineno = yylineno;
	node->info.value.INT = (unsigned int)strtoul(yytext,NULL,16);
	node->info.nodetype = LEX;
	strcpy(node->info.notation,yytext);
	yylval.type_node = node;
	return INT;
}
(({digit}+\.{digit}*)|({digit}*\.{digit}+))(E|e)		{		//ILLEGAL scientific float
	lexicalerr_occur = 1;
	PrtErrA(yylineno);
	printf("Illegal scientific float %s (exponent is wanted).\n",yytext);
	struct Node* node = getNewNode();//printf(%s",yytext);
	strcpy(node->info.name,"FLOAT");
	node->info.lineno = yylineno;
	node->info.value.FLOAT = (float)atof(yytext);
	node->info.nodetype = LEX;
	strcpy(node->info.notation,yytext);
	yylval.type_node = node;
	return FLOAT;
}
(\.{digit}+)|(0|([1-9]{digit}*)\.)	{	//ILLEGAL normal float
	lexicalerr_occur = 1;
	PrtErrA(yylineno);
	printf("Illegal float %s (\".\" should be surrounded by number at both sides).",yytext);
	struct Node* node = getNewNode();//printf(%s",yytext);
	strcpy(node->info.name,"FLOAT");
	node->info.lineno = yylineno;
	node->info.value.FLOAT = (float)atof(yytext);
	node->info.nodetype = LEX;
	strcpy(node->info.notation,yytext);
	yylval.type_node = node;
	return FLOAT;
}

({digit}+)(({_letter}+)({digit}*))+	{	//ILLEGAL ID
	lexicalerr_occur = 1;
	PrtErrA(yylineno);
	printf("Illegal ID %s (ID should not start with number).\n",yytext);
	struct Node* node = getNewNode();//printf(%s",yytext);
	strcpy(node->info.name,"ID");
	node->info.lineno = yylineno;
	node->info.nodetype = LEX;
	strcpy(node->info.notation,yytext);
	yylval.type_node = node;
	return ID;
}
%%
int PrtErrA(int lineno)
{
	printf("Error type A at lien %d: ",lineno);
}
