%{
	#include<stdio.h>
	#include"lex.yy.c"
	int syntaxerr_occur = 0;
	int last_syntaxerr_lineno = -1;
	extern struct Node* syntaxTreeRoot;
	#define prtSynErr printf
%}
/* declared types */
%union{
	//int type_int;
	//float type_float;
	struct Node* type_node;
	double type_double;
}

/* declared tokens */
%token <type_node> INT FLOAT ID SEMI COMMA ASSIGNOP RELOP PLUS MINUS STAR DIV AND OR DOT NOT TYPE LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE
%token <type_node> ENDL
/* declared non-terminals */
%type <type_node> Program ExtDefList ExtDef ExtDecList
%type <type_node> Specifier StructSpecifier OptTag Tag
%type <type_node> VarDec FunDec VarList ParamDec
%type <type_node> CompSt StmtList Stmt
%type <type_node> DefList Def DecList Dec Exp Args

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right UNARY NOT
%left LP RP LB RB DOT

%%
Program	:	ExtDefList
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"Program");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 1;
			InsertNode(node,$1);
			$$ = node;
			syntaxTreeRoot = node;
		}
	;
ExtDefList:	ExtDef ExtDefList
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"ExtDefList");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 2;
			InsertNode(node,$1);
			InsertNode(node,$2);
			$$ = node;
		}
	|	/* empty */
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"ExtDefList");
			node->info.lineno = -1;
			node->info.nodetype = STX;
			node->info.ruleId = 3;
//			InsertNode(node,NULL);
			$$ = node;
		}
	;
ExtDef	:	Specifier ExtDecList SEMI
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"ExtDef");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 4;
			InsertNode(node,$1);
			InsertNode(node,$2);
			InsertNode(node,$3);
			$$ = node;
		}
       	|	Specifier SEMI
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"ExtDef");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 5;
			InsertNode(node,$1);
			InsertNode(node,$2);
			$$ = node;
		}
	|	Specifier FunDec CompSt
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"ExtDef");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 6;
			InsertNode(node,$1);
			InsertNode(node,$2);
			InsertNode(node,$3);
			$$ = node;
		}
	|	error SEMI
	{
		prtSynErr("Illegal ExtDef (Expect \"Specifier ExtDecList;\" or \"Specifier;\")");
	}
	;
ExtDecList:	VarDec
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"ExtDecList");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 7;
			InsertNode(node,$1);
			$$ = node;
		}
	|	VarDec COMMA ExtDecList
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"ExtDecList");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 8;
			InsertNode(node,$1);
			InsertNode(node,$2);
			InsertNode(node,$3);
			$$ = node;
		}
	|	FunDec
		{
			/* added for Lab2, ExtDecList->FunDec */
			struct Node* node = getNewNode();
			strcpy(node->info.name,"ExtDecList");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 9;
			InsertNode(node,$1);
			$$ = node;
		}
		
	|	FunDec COMMA ExtDecList
		{
			/* added for Lab2, ExtDecList->FunDec COMMA ExtDecList */
			struct Node* node = getNewNode();
			strcpy(node->info.name,"ExtDecList");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 10;
			InsertNode(node,$1);
			InsertNode(node,$2);
			InsertNode(node,$3);
			$$ = node;
		}
	;

Specifier:	TYPE
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"Specifier");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 11;
			InsertNode(node,$1);
			$$ = node;
		}
	|	StructSpecifier	
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"Specifier");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 12;
			InsertNode(node,$1);
			$$ = node;
		}
	|	error TYPE
		{
			prtSynErr("Illegal Specifier (Extra thing before TYPE)");		//???
		}
	;
StructSpecifier:STRUCT OptTag LC DefList RC
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"StructSpecifier");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 13;
			InsertNode(node,$1);
			InsertNode(node,$2);
			InsertNode(node,$3);
			InsertNode(node,$4);
			InsertNode(node,$5);
			$$ = node;
		}
	|	STRUCT Tag
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"StructSpecifier");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 14;
			InsertNode(node,$1);
			InsertNode(node,$2);
			$$ = node;
		}
	|	error RC
		{
			prtSynErr("Illegal StructSpecifier (Expect \"struct OptTag ( DefList )\")");
		}
	;
OptTag	:	ID
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"OptTat");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 15;
			InsertNode(node,$1);
			$$ = node;
		}
	|	/* empty */
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"OptTag");
			node->info.lineno = -1;
			node->info.nodetype = STX;
			node->info.ruleId = 16;
//			InsertNode(node,NULL);
			$$ = node;
		}
	;
Tag	:	ID
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"Tag");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 17;
			InsertNode(node,$1);
			$$ = node;
		}
	;

VarDec	:	ID
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"VarDec");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 18;
			InsertNode(node,$1);
			$$ = node;
		}
	|	VarDec LB INT RB
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"VarDec");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 19;
			InsertNode(node,$1);
			InsertNode(node,$2);
			InsertNode(node,$3);
			InsertNode(node,$4);
			$$ = node;
		}
	|	error RB
		{
			prtSynErr("Illegal VarDec (Expect \"VarDec[INT]\")");
		}
	;
FunDec	:	ID LP VarList RP
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"FunDec");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 20;
			InsertNode(node,$1);
			InsertNode(node,$2);
			InsertNode(node,$3);
			InsertNode(node,$4);
			$$ = node;
		}
	|	ID LP RP
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"FunDec");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 21;
			InsertNode(node,$1);
			InsertNode(node,$2);
			InsertNode(node,$3);
			$$ = node;
		}
	|	error RP
		{
			prtSynErr("Illegal FunDec (Expect \"ID(VarList)\" or \"ID()\")");
		}
	;
VarList	:	ParamDec COMMA VarList
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"VarList");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 22;
			InsertNode(node,$1);
			InsertNode(node,$2);
			InsertNode(node,$3);
			$$ = node;
		}
	|	ParamDec
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"VarList");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 23;
			InsertNode(node,$1);
			$$ = node;
		}
	|	error COMMA
		{
			prtSynErr("Illegal VarList (Extra \",\")");
		}
	;
ParamDec:	Specifier VarDec
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"ParamDec");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 24;
			InsertNode(node,$1);
			InsertNode(node,$2);
			$$ = node;
		}
	;

CompSt	:	LC DefList StmtList RC
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"CompSt");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 25;
			InsertNode(node,$1);
			InsertNode(node,$2);
			InsertNode(node,$3);
			InsertNode(node,$4);
			$$ = node;
		}
	|	error RC
		{
			prtSynErr("Illegal CompSt (Expect \"(DefList StmtList)\")");
		}
	;
StmtList:	Stmt StmtList
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"StmtList");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 26;
			InsertNode(node,$1);
			InsertNode(node,$2);
			$$ = node;
		}
	|	/* empty */
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"StmList");
			node->info.lineno = -1;
			node->info.nodetype = STX;
			node->info.ruleId = 27;
//			InsertNode(node,NULL);
			$$ = node;
		}
	;
Stmt	:	Exp SEMI
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"Stmt");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 28;
			InsertNode(node,$1);
			InsertNode(node,$2);
			$$ = node;
		}
     	|	CompSt
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"Stmt");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 29;
			InsertNode(node,$1);
			$$ = node;
		}
	|	RETURN Exp SEMI
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"Stmt");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 30;
			InsertNode(node,$1);
			InsertNode(node,$2);
			InsertNode(node,$3);
			$$ = node;
		}
	|	IF LP Exp RP Stmt		%prec LOWER_THAN_ELSE
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"Stmt");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 31;
			InsertNode(node,$1);
			InsertNode(node,$2);
			InsertNode(node,$3);
			InsertNode(node,$4);
			InsertNode(node,$5);
			$$ = node;
		}
	|	IF LP Exp RP Stmt ELSE Stmt
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"Stmt");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 32;
			InsertNode(node,$1);
			InsertNode(node,$2);
			InsertNode(node,$3);
			InsertNode(node,$4);
			InsertNode(node,$5);
			InsertNode(node,$6);
			InsertNode(node,$7);
			$$ = node;
		}
	|	WHILE LP Exp RP Stmt
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"Stmt");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 33;
			InsertNode(node,$1);
			InsertNode(node,$2);
			InsertNode(node,$3);
			InsertNode(node,$4);
			InsertNode(node,$5);
			$$ = node;
		}
	|	error SEMI
		{
			prtSynErr("Illegal Stmt (Expect \"Exp;\" or \"return Exp;\")");
		}
	;

DefList	:	Def DefList
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"DefList");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 34;
			InsertNode(node,$1);
			InsertNode(node,$2);
			$$ = node;
		}
	|	/* empty */
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"DefList");
			node->info.lineno = -1;
			node->info.nodetype = STX;
			node->info.ruleId = 35;
//			InsertNode(node,NULL);
			$$ = node;
		}
	;
Def	:	Specifier DecList SEMI
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"Def");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 36;
			InsertNode(node,$1);
			InsertNode(node,$2);
			InsertNode(node,$3);
			$$ = node;
		}
	|	error SEMI
		{
			prtSynErr("Illegal Def (Expect \"Specifier DecList;\")");
		}
	;
DecList	:	Dec
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"DecList");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 37;
			InsertNode(node,$1);
			$$ = node;
		}
	|	Dec COMMA DecList
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"DecList");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 38;
			InsertNode(node,$1);
			InsertNode(node,$2);
			InsertNode(node,$3);
			$$ = node;
		}
	;
Dec	:	VarDec
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"Dec");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 41;
			InsertNode(node,$1);
			$$ = node;
		}
	|	VarDec ASSIGNOP Exp
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"Dec");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 42;
			InsertNode(node,$1);
			InsertNode(node,$2);
			InsertNode(node,$3);
			$$ = node;
		}
	;

Exp	:	Exp ASSIGNOP Exp
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"Exp");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 43;
			InsertNode(node,$1);
			InsertNode(node,$2);
			InsertNode(node,$3);
			$$ = node;
		}
	|	Exp AND Exp
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"Exp");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 44;
			InsertNode(node,$1);
			InsertNode(node,$2);
			InsertNode(node,$3);
			$$ = node;
		}
	|	Exp OR Exp
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"Exp");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 45;
			InsertNode(node,$1);
			InsertNode(node,$2);
			InsertNode(node,$3);
			$$ = node;
		}
	|	Exp RELOP Exp
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"Exp");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 46;
			InsertNode(node,$1);
			InsertNode(node,$2);
			InsertNode(node,$3);
			$$ = node;
		}
	|	Exp PLUS Exp
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"Exp");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 47;
			InsertNode(node,$1);
			InsertNode(node,$2);
			InsertNode(node,$3);
			$$ = node;
		}
	|	Exp MINUS Exp
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"Exp");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 48;
			InsertNode(node,$1);
			InsertNode(node,$2);
			InsertNode(node,$3);
			$$ = node;
		}
	|	Exp STAR Exp
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"Exp");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 49;
			InsertNode(node,$1);
			InsertNode(node,$2);
			InsertNode(node,$3);
			$$ = node;
		}
	|	Exp DIV Exp
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"Exp");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 50;
			InsertNode(node,$1);
			InsertNode(node,$2);
			InsertNode(node,$3);
			$$ = node;
		}
	|	LP Exp RP
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"Exp");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 51;
			InsertNode(node,$1);
			InsertNode(node,$2);
			InsertNode(node,$3);
			$$ = node;
		}
	|	MINUS Exp	%prec UNARY
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"Exp");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 52;
			InsertNode(node,$1);
			InsertNode(node,$2);
			$$ = node;
		}
	|	NOT Exp
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"Exp");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 53;
			InsertNode(node,$1);
			InsertNode(node,$2);
			$$ = node;
		}
	|	ID LP Args RP
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"Exp");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 54;
			InsertNode(node,$1);
			InsertNode(node,$2);
			InsertNode(node,$3);
			InsertNode(node,$4);
			$$ = node;
		}
	|	ID LP RP
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"Exp");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 55;
			InsertNode(node,$1);
			InsertNode(node,$2);
			InsertNode(node,$3);
			$$ = node;
		}
	|	Exp LB Exp RB
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"Exp");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 56;
			InsertNode(node,$1);
			InsertNode(node,$2);
			InsertNode(node,$3);
			InsertNode(node,$4);
			$$ = node;
		}
	|	Exp DOT ID
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"Exp");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 57;
			InsertNode(node,$1);
			InsertNode(node,$2);
			InsertNode(node,$3);
			$$ = node;
		}
	|	ID
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"Exp");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 58;
			InsertNode(node,$1);
			$$ = node;
		}
	|	INT
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"Exp");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 59;
			InsertNode(node,$1);
			$$ = node;
		}
	|	FLOAT
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"Exp");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 60;
			InsertNode(node,$1);
			$$ = node;
		}
	|	error RP 
		{
			prtSynErr("Illegal Exp (Expect \"(Exp)\", \"ID(Args)\" or \"ID()\")");
		}
	|	error RB
		{
			prtSynErr("Illegal Exp (Expect \"Exp[Exp]\")");
		}
	;
Args	:	Exp COMMA Args
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"Args");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 61;
			InsertNode(node,$1);
			InsertNode(node,$2);
			InsertNode(node,$3);
			$$ = node;
		}
	|	Exp
		{
			struct Node* node = getNewNode();
			strcpy(node->info.name,"Args");
			node->info.lineno = $1->info.lineno;
			node->info.nodetype = STX;
			node->info.ruleId = 62;
			InsertNode(node,$1);
			$$ = node;
		}
	;

%%

int yyerror(char* ch)
{
	syntaxerr_occur = 1;
	last_syntaxerr_lineno=yylineno;
	printf("Error type B at line %d: Syntax error. ",yylineno);
}
