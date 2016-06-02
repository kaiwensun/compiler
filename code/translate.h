#ifndef _TRANSLATE_H_
#define _TRANSLATE_H_
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include"semantic.h"

#include<limits.h>
#include<unistd.h>

/* 结构体声明 */
struct Operand	//操作数
{
	enum{
		OPD_LAB,	//label
		OPD_FUN,	//function
		OPD_VAR,	//variable
		OPD_CON,	//constant
		OPD_REL,	//relation token
		OPD_GCN,	//get content (*)
		OPD_GAD		//get address (&)
	} kind;
	char name[256];
//	int value;
//	unsigned int address;
};
struct Code	//对应一行中间代码
{
	enum{
		OP_ERR,
		OP_LAB,OP_FUN,OP_ASS,OP_ADD,
		OP_SUB,OP_MUL,OP_DIV,OP_GTO,
		OP_IFJ,OP_RET,OP_DEC,OP_ARG,
		OP_CAL,OP_PAR,OP_RED,OP_WRI
	} op;
	struct Operand* op1;
	struct Operand* op2;
	struct Operand* op3;
	struct Operand* op4;
	struct Code* prev;
	struct Code* next;
};
struct CodeSegment
{
	/* 对应一段中间代码，仅用于link
	 * 注意有可能会因调整code顺序时动了首尾而使之失效
	 * 不允许建立此结构的指针类型！！！！
	 */
	struct Code* head;
	struct Code* tail;
};
struct ArgList
{
	char argname[256];
	int opd_kind;//建议上层使用这个操作数类型
	struct ArgList* next;
};

/* 函数声明 */

/* 中间代码结构函数 */
/* 函数功能：将两块中间代码连接起来 */
struct CodeSegment linkCodeSegment(struct CodeSegment seg1,struct CodeSegment seg2);
/* 函数功能：将两行中间代码连接起来 */
struct CodeSegment linkTwoCode(struct Code* code1,struct Code* code2);
/* 函数功能：打印一条代码到文件 */
void outputCode(const struct Code* code);
/* 函数功能：输出代码段到文件 */
unsigned int outputCodeSegment(const struct CodeSegment seg_code);
/* 函数功能：释放Operand空间 */
int freeOperand(struct Operand* operand);
/* 函数功能：释放一行Code的空间 */
int freeCode(struct Code* code);
/* 函数功能：申请一行Code的空间，并初始化 */
struct Code* newCode(int operand_num);

/* 其他函数 */
/* 函数功能：在符号表中加入read和write函数 */
void initTranslate(FILE* fp);
/* 函数功能：生成一个全局唯一的临时变量名^tmp%u */
char* newTmpName();
/* 函数功能：生成一个全局唯一的临时Label名^label%u */
char* newLabelName();
/* 函数功能：驱动整个中间代码生成过程，遍历整个语法树 */
void generateIrCode(struct Node* root);
/* 函数功能：生产为数组或结构体分配空间的中间代码 */
struct CodeSegment translate_allocateSpaceForArrayOrStruct(struct Prop* varprop);
/* 函数功能：计算基本类型、数组类型的或结构体类型的所占用空间的大小（单位：4字节，不是字节） */
unsigned int calculateTypeTotalSize(struct Type* type);
/* 函数功能：释放ArgList链表空间 */
unsigned int freeArgList(struct ArgList* arg_list);
/* 函数功能：计算数组变量的偏移地址，并给出计算偏移地址的中间代码 */
struct CodeSegment calculateArrOrStructOffset(struct Node* node_Exp,struct Type** callerType,const char* place);
/* 函数功能：判断node_Exp是由ID/数组/还是结构体/其他生成的 */
int whichKindOfExp(const struct Node* node_Exp,char* idname);
/* 函数功能：判断符号表中的char* name是不是函数的形参列表中的形参 */
int idnameIsAParameter(char* name);
/* 函数功能：扫描符号表，避免label、tmp与符号表中的变量重名 */
void avoidTmpAndLabelNameConflict();
/* 函数功能：把操作数名称为带井号常量的操作数的kind修正为OPD_CON */
int translate_patch1();

/* 节点翻译函数 */
struct CodeSegment translate_node_Program(struct Node* node_Program);
struct CodeSegment translate_node_ExtDefList(struct Node* node_ExtDefList);
struct CodeSegment translate_node_ExtDef(struct Node* node_ExtDef);
struct Type* translate_node_Specifier(struct Node* node_Specifier);
struct Type* translate_node_TYPE(struct Node* node_TYPE);
struct Type* translate_node_StructSpecifier(struct Node* node_StructSpecifier);
struct Type* translate_node_OptTag(struct Node* node_OptTag);
struct Type* translate_node_Tag(struct Node* node_Tag);
struct CodeSegment translate_node_FunDec(struct Node* node_FunDec);
struct CodeSegment translate_node_VarList(struct Node* node_VarList);
struct CodeSegment translate_node_ParamDec(struct Node* node_ParamDec);
struct Prop* translate_node_VarDec(struct Node* node_VarDec);
struct CodeSegment translate_node_CompSt(struct Node* node_CompSt);
struct CodeSegment translate_node_DefList(struct Node* node_DefList);
struct CodeSegment translate_node_Def(struct Node* node_Def);
struct CodeSegment translate_node_DecList(struct Node* node_DecList);
struct CodeSegment translate_node_Dec(struct Node* node_Dec);
struct CodeSegment translate_node_StmtList(struct Node* node_StmtList);
struct CodeSegment translate_node_Stmt(struct Node* node_Stmt);
struct CodeSegment translate_node_Exp(struct Node* node_Exp,const char* place,int* opd_place);
struct CodeSegment translate_node_Args(struct Node* node_Args,struct ArgList** arg_list);

struct CodeSegment translate_Cond(struct Node* node_Exp,const char* label_true,const char* label_false);


/* 调试函数 */
/* 函数功能：输出代码段中的信息 */
void dbg_printCodeSegment(const struct CodeSegment seg_code);
/* 函数功能：输出一行代码信息 */
void dbg_printCode(const struct Code* code);
#endif
