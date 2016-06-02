#ifndef _IRPOT_H_
#define _IROPT_H_
#include"translate.h"

/* 函数功能：判断操作数是否以右值身份在代码段中被用过 */
int operandIsUsedInCodeSegment(const struct Operand* opd,const struct CodeSegment seg_code);
/* 函数功能：判断操作数是否以右值身在代码行中份被用过 */
int operandIsUsedInCode(const struct Operand* opd,const struct Code* code);
/* 函数功能：在全部代码段中删除一行中间代码 */
void deleteOneCode(struct Code* code);
/* 函数功能：获取中间代码的左值操作数 */
struct Operand* getLeftOperand(struct Code* code);
/* 函数功能：检查并删除中间代码中生成左值后不再被用过的变量 */
int opt_usageCheck();
/* 函数功能：驱动整个中间代码的优化过程 */
void iroptimizer();
/* 函数功能：把x = #const [+ - * /] #const 改写为x = #const */
int opt_calculateFourOperatopnConst();
/* 函数功能：对于一个右值操作数，希望能追根溯源找到它的被OP_ASS等代码赋值的地方 */
struct Code* findSourceAssCode(const struct Operand* opd,const struct Code* code);
/* 函数功能：对于所有...:=...形式的中间代码，如果等号右侧的变量x是之前由s赋值过的一个变量x，则尝试在此直接使用s */
int opt_traceRightOpd();
/* 函数功能：把无效的四则运算变为赋值语句 */
int opt_ignoreInvalidFourOperationConst();
/* 函数功能：若由相邻的a := Exp; b := a且a没再被用，把两行代码转换为一行 b := Exp */
int opt_duplexCheck();
/* 函数功能：检查变量名是否在给定的代码段中的代码行里出现过 */
int operandNameExistInCodeSegment(const char* name,struct CodeSegment seg_code);
/* 函数功能：在给定的代码段中寻找名为labelname的跳转主动发起方label */
struct Code* labelSorceExistInCodeSegment(const char* labelname,struct CodeSegment seg_code);
/* 函数功能：在给定的代码段中寻找名为labelname的目标label */
struct Code* labelTargetExistInCodeSegment(const char* labelname,struct CodeSegment seg_code);
/* 函数功能：对紧邻label的jump和goto语句进行优化 */
int opt_clearJump();


#endif
