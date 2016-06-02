#include<stdio.h>
#include"syntaxtree.h"
#include"semantic.h"
#include"translate.h"
#include"iropt.h"

extern yylineno;
extern int yydebug;
extern int syntaxerr_occur;
extern int lexicalerr_occur;
//extern void printSyntaxTree(struct Node* root, int tab);
extern struct Node* syntaxTreeRoot;
extern struct CodeSegment global_code_segment;
int main(int argc, char** argv)
{
	FILE* f = NULL;		//c--代码文件
	FILE* f2 = NULL;	//MIPS代码文件
	FILE* f3 = NULL;	//中间代码文件
	int flag_outputir = 0;	//是否输出中间代码
	if(argc>2)
	{
		if(!(f=fopen(argv[1],"r")))
		{
			perror(argv[1]);
			exit(-1);
		}
		if(!(f2=fopen("mytest.ir","w")))
		{
			perror("mytest.ir");
			exit(-1);
		}
		if(!(f3=fopen(argv[2],"w")))
		{
			perror(argv[2]);
			exit(-1);
		}
	}
	else
	{
		perror("no input/output file\r\n");
		exit(-1);
	}
//	while(yylex()!=0);
	yyrestart(f);
	yylineno = 1;
//yydebug =1;
	yyparse();
//	printSyntaxTree(syntaxTreeRoot,0);
	initTranslate(f2);
	semanticAnalyze(syntaxTreeRoot);
/*
 * 为适应Lab3，不再检查未定义
	findUndefinedFunc();
 */
	avoidTmpAndLabelNameConflict();
	generateIrCode(syntaxTreeRoot);
	iroptimizer();
	translate_patch1();	//中间代码补丁1
	if(flag_outputir)
	{
		unsigned code_cnt = outputCodeSegment(global_code_segment);
//		printf("共输出%u条中间代码到文件\r\n",code_cnt);
	}


	genMips(global_code_segment);
	outputMips(f3);
	return 0;

}
