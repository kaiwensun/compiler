/*
 * semantic.h
 * 作者：孙楷文
 */
#ifndef _SEMANTIC_H_
#define _SEMANTIC_H_

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include"syntaxtree.h"
#define dbgprt printf	//用于程序调试输出（与正常使用的报错不同）
#define MAX_ID_LEN 64	//ID允许的最长长度，与 "syntaxtree.h" 中的MAX_NOTATION_LEN一致 

/* 结构声明 */

struct Type;
struct FieldList;
struct VarProp;
struct FunProp;
struct Prop;

/* 结构定义 */

struct Type
{
	/* 数据类型，包括：basic(int/float),array,struct */
	int kind;		//哪一类类型。0:basic;1:array;2:struct
	union{
		/* 0:INT;1:FLOAT */
		 int basic;					
		/* arreletype:数组的基础类型；arrsize:数组这一维的长度 */
		struct{struct Type* arreletype;int arrsize;} array;
		/* struct_name:结构体名（不是结构体变量名！）;struct_field:结构体所包含的域 */
		struct{char struct_name[MAX_ID_LEN];struct FieldList* struct_field;}structure;
	}un;			//类型的具体信息
	int def_lineno;		//类型【第一次】被定义时所在的行号（如有需要，int和float设为-1，)
	int layer;		//类型【第一次】被定义时所在的{}层数（如有需要，常量或左值设为-2，)
	struct Type* next;	//将所有定义的类型串在一个栈里的链表指针
};

struct FieldList
{
	/* 结构体中的一个域*/
	char name[MAX_ID_LEN];	//域的变量名
	struct Type* type;	//域变量的数据类型
	int def_lineno;		//这个域在被定义时所在的行号
	struct FieldList* next;	//将一个结构体中的域串起来的链表指针

	int layer;		//特殊：struct的域中不需要，仅为ExtDefList中转换为VarProp而用
};

struct VarProp
{
	/* 变量的属性（在Prop结构中使用) */
	struct Type* type;		//变量的数据类型(int,float,array,structure...)，指向独立的空间而非类型栈！
	int def_lineno;
};

struct FunProp
{
	/* 函数的属性（在Prop结构中使用） */
	struct Type* f_rtn_type;	//函数返回值类型(指向独立空间而非类型栈)
	int paracnt;			//函数参数的个数
	struct Prop* paraprop;		//描述函数形参的【链表】，只保存形参类型。形参名由符号表管理。
	int declared;			//-1:未声明;+i:在第i行被声明(定义不算做声明)
	int defined;			//-1:未定义;+i:在第i行被定义;
};

struct Prop
{
	/* 用户自己定义的变量（基本变量、数组、结构体）和函数 */
	int kind;				//变量/函数。0:变量;1:函数
	union{
		struct VarProp* varprop;	//变量属性
		struct FunProp* funprop;	//函数属性
	}un;
	char name[MAX_ID_LEN];			//变量名/函数名
	int layer;				//变量/函数所在定义域的层数
	struct Prop* next;			//用于串成Prop stack链表的指针，或者串成函数参数列表
};


/* 函数声明 */
/* 函数声明-基本元素操作 */

/* 函数功能：向type_stack_top中压栈 */
void push_type_stack(struct Type* type);
/* 函数功能：从type_stack_top栈中弹栈并释放空间，直到发现栈顶元素的layer不大于until_layer */
void pop_type_stack(int until_layer);
/* 函数功能：向prop_stack_top中压栈 */
void push_prop_stack(struct Prop* prop);
/* 函数功能：从prop_stack_top栈中弹栈并释放空间，直到发现栈顶元素的layer不大于until_layer */
void pop_prop_stack(int until_layer);
/* 函数功能：复制一个结构体的一个域（申请了新空间）*/
struct FieldList* copyField(struct FieldList* fld);
/* 函数功能：复制一个数据类型（如int,float,array,struct...）（申请了新空间）*/
struct Type* copyType(struct Type* type);
/* 函数功能：复制一个变量Prop（申请了新空间）*/
struct Prop* copyVarProp(struct Prop* prop);
/* 函数功能：计算数组的维度数 */
int countArrDim(struct Type* type);
/* 函数功能：获得数组的基本元素的类型 */
struct Type* getArrEleType(struct Type* type);
/* 函数功能：判断两个数据类型是否是结构等价的 */
int typeIsSame(struct Type* type1,struct Type* type2);
/* 函数功能：判断两个结构体数据类型是否是结构等价的 */
int structureIsSame(struct Type* struct1,struct Type* struct2);
/* 函数功能：判断两个函数Prop节点的结构是否等价（返回值类型、形参数量、形参类型）*/
int funIsSame(struct Prop* prop1,struct Prop* prop2);
/* 函数功能：在type栈中根据类型名和定义域查找struct类型（定义过的结构体）*/
struct Type* searchTypeStackByStructName(char* name,int out_layer);
/* 函数功能：在Prop栈中根据变量/函数名和定义域查找Prop节点*/
struct Prop* searchPropStackByPropName(char* name,int out_layer,int search_var,int search_fun);
/* 函数功能：把两个FieldList域链表链接成一个链表 */
struct FieldList* linkFieldList(struct FieldList* fldlst_head,struct FieldList* fldlst_tail);
/* 函数功能：根据name查找fld_lst中的一个域节点 */
struct FieldList* searchFieldListByName(char* name,struct FieldList* fld_lst);
/* 函数功能：释放fld_lst所带领的链表空间 或 单独的fld_lst的空间 */
struct FieldList* freeFieldList(struct FieldList* fld_lst,int only_free_node);
/* 函数功能：释放一个Type的空间 */
struct Type* freeType(struct Type* type);
/* 函数功能：释放一个Prop的空间 */
struct Prop* freeProp(struct Prop* prop);
/* 函数功能：把FieldList*类型的变量转换成Prop类型的变量 */
struct Prop* transferFieldToProp(struct FieldList* fld_lst);
/* 函数功能：判断type是否为int型 */
int typeIsBasic(struct Type* type,int basic);

/* 函数声明-其他函数 */
/* 函数功能：驱动整个语法分析过程，遍历整个语法树，遇到ExtDef、Def、Exp、{、} 后调用专门的解析函数 */
void semanticAnalyze(struct Node* root);
/* 函数功能：按照实验要求打印语义错误信息 */
void semanticErrorPrint(int type,int lineno,char* info);
/* 函数功能：计算函数参数列表的个数 */
int countFunParaList(struct Prop* paraprop);
/* 函数功能：申请一个type空间作为INT型左值类型 */
struct Type* getAnIntType();
/* 函数功能：检查未定义的函数，并输出错误信息 */
int findUndefinedFunc();

/* 函数声明-语义节点属性构造 */
int			analyze_node_ExtDef(struct Node* node_ExtDef);
struct Type*		analyze_node_Specifier(struct Node* node_Specifier);
struct Type*		analyze_node_TYPE(struct Node* node_TYPE);
struct Type*		analyze_node_StructSpecifier(struct Node* node_StructSpecifier);
char*			analyze_node_OptTag(struct Node* node_OptTag);
struct FieldList*	analyze_node_DefList(struct Node* node_DefList,int flag);
struct FieldList*	analyze_node_Def(struct Node* node_Def,int flag);
struct FieldList*	analyze_node_DecList(struct Node* node_DecList,int flag,struct Type* t_Specifier);
struct FieldList*	analyze_node_Dec(struct Node* node_Dec,int flag,struct Type* t_Specifier);
struct FieldList*	analyze_node_VarDec(struct Node* node_VarDec,int flag,struct Type* t_Specifier);
int			analyze_node_ExtDecList(struct Node* node_ExtDecList,struct Type* t_Specifier);
struct Prop*		analyze_node_FunDec(struct Node* node_FunDec,int flag,struct Type* t_Specifier);
struct Prop*		analyze_node_VarList(struct Node* node_VarList,int flag);
int			analyze_node_CompSt(struct Node* node_CompSt,struct Type* t_Specifier);
struct Prop*		analyze_node_ParamDec(struct Node* node_ParamDec);
struct Type*		analyze_node_Exp(struct Node* node_Exp,int dim);
int			analyze_node_StmtList(struct Node* node_StmtList,struct Type* t_Specifier);
int			analyze_node_Stmt(struct Node* node_Stmt,struct Type* t_Specifier);
struct Type*		analyze_node_Args(struct Node* node_Args);
int			analyze_node_LC();
int			analyze_node_RC();

/* 函数声明-用于调试程序的函数 */
void dbg_printPropStack(int need_detail);
void dbg_printTypeStack(int need_detail);

#endif
