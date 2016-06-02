#include"translate.h"
#include<string.h>
#define dbgprt printf
#define dbg_constprt printf
//8,9,10作为三个直接运算的操作数；11作为指针寻址等操作的辅助操作数
#define REG_START 4
#define REG_END 25
struct MIPSCode
{
	//一行MIPS代码
	char mcode[256];
	struct MIPSCode* next;
};
struct FuncLocalVar
{
	//一个局部变量/形参
	char varname[256];		//局部变量名
	int offset;			//局部变量所占内存对SP的偏移量（byte）
	int regnum;			//局部变量对应的寄存器编号。-1为未分配
	struct FuncLocalVar* next;	//函数中下一个局部变量
};
struct FuncLocalvarList
{
	//一个函数活动记录链表节点
	char funcname[256];		//函数名
	int localarg_cnt;		//函数形参相对于$sp的最大偏移量（单位byte）
	struct FuncLocalVar* arglist;	//函数形参链表
	int localvar_cnt;		//函数内局部变量相对于$sp的总偏移量（单位byte）
	struct FuncLocalVar* varlist;	//函数内局部变量名链表
	struct FuncLocalvarList* next;	//下一个函数活动记录(是静态记录。动态函数栈为struct FuncCallingStack)
};


/* 函数声明 */
/* 函数功能：添加一行MIPS代码 */
void appendMipsCode(char* str);
/* 函数功能：释放MIPSCode链表空间 */
int freeMIPSCodeList(struct MIPSCode* mcode);
/* 函数功能：在局部变量/形参链表中搜索制定名称的变量/形参 */
struct FuncLocalVar* searchLocalvarByName(struct FuncLocalVar* varlist, const char* name);
/* 函数功能：根据函数名再静态活动记录链表中搜索对应的活动记录 */
struct FuncLocalvarList* searchFuncLocalvarListByName(const char* name,struct FuncLocalvarList* funclist);
/* 函数功能：把opd里的变量名放到varlist里 */
int addOpdToList(const struct Operand* opd,struct FuncLocalVar** varlist,int offset);
/* 函数功能：计算code中涉及到的局部变量在栈中的偏移量 */
int countLVStack(struct FuncLocalvarList* funcvars,struct Code* code,int oldoffset);
/* 函数功能：在把中间代码翻译为MIPS代码之前，数出每一个函数中的局部变量的个数，计算局部变量的偏移位置，以为函数调用压栈时做准备 */
void prepareFuncLocalVarList(struct CodeSegment seg);
/* 函数功能：产生把指定寄存器的值存入指定变量内存中的MIPS代码，并直接加入全局代码链表中 */
void genSW(int reg_id,const char* varname,struct FuncLocalvarList* funclv,int flag);
/* 函数功能：产生把指定变量内存中的 值/地址/指针内容 存入指定寄存器的MIPS代码，并直接加入全局代码链表中 */
void genLW(int reg_id,const char* varname,struct FuncLocalvarList* funclv,int flag);
/* 函数功能：把一行中间代码翻译成MIPS代码，并加入MIPS代码链表 */
void mipsCode(struct Code* code);
/* 函数功能：翻译opd类型，供genLW和genSW函数使用 */
int getFlag(int opdkind);
/* 函数功能：生成.text、write函数、read函数等公共文件首部分 */
void genMipsHead();
/* 函数功能：生产MIPS代码全文（不输出） */
void genMips(struct CodeSegment seg);

/* 调试函数 */
/* 函数功能：输出一个函数静态活动记录中参数链表/局部变量链表的信息 */
void dbg_printFunclvList(struct FuncLocalVar* funclvlist);
/* 函数功能：输出所有函数活动记录的信息 */
void dbg_printFuncList();
