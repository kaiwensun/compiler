#include"iropt.h"
#include"translate.h"

#define dbg_constprt printf

extern struct CodeSegment global_code_segment;

void iroptimizer()
{
	/* 函数功能：
	 * 	驱动整个中间代码的优化过程
	 */
	int flag=1;	//一次循环中是否优化了代码
	unsigned int optcnt_usagecheck=0;
	unsigned int optcnt_tracerightopd=0;
	unsigned int optcnt_calculatefouroperatopnconst = 0;
	unsigned int optcnt_ignoreinvalidfouroperationconst = 0;
	unsigned int optcnt_duplexcheck = 0;
	unsigned int optcnt_clearjump = 0;
	while(flag>0)
	{
		flag=0;
		int rtn=0;
		rtn = opt_usageCheck();
		flag += rtn;
		optcnt_usagecheck += rtn;
		rtn = opt_traceRightOpd();
		flag += rtn;
		optcnt_tracerightopd += rtn;
		rtn = opt_calculateFourOperatopnConst();
		flag += rtn;
		optcnt_calculatefouroperatopnconst += rtn;
		rtn = opt_ignoreInvalidFourOperationConst();
		flag += rtn;
		optcnt_ignoreinvalidfouroperationconst += rtn;
		rtn = opt_duplexCheck();
		flag += rtn;
		optcnt_duplexcheck += rtn;
		rtn = opt_clearJump();
		flag += rtn;
		optcnt_clearjump += rtn;
	}
/*
 	printf("opt_usageCheck()优化删除了%u条代码\r\n",optcnt_usagecheck);
	printf("opt_traceRightOpd()优化计算了%u条代码\r\n",optcnt_tracerightopd);
	printf("opt_calculateFourOperatopnConst()优化计算了%u条代码\r\n",optcnt_calculatefouroperatopnconst);
	printf("opt_ignoreInvalidFourOperationConst()优化计算了%u条代码\r\n",optcnt_ignoreinvalidfouroperationconst);
	printf("opt_duplexCheck()优化删除了%u条代码\r\n",optcnt_duplexcheck);
	printf("opt_clearJump()优化删除了%u条代码\r\n",optcnt_clearjump);
*/
}
//=========================================================================
int operandIsUsedInCodeSegment(const struct Operand* opd,const struct CodeSegment seg_code)
{
	/* 函数功能：
	 * 	判断操作数是否以右值身份在代码段中被用过
	 * 参数：
	 * 	opd：要被判断的操作数
	 * 	seg_code：在该代码段内被判断（通常为全部代码段）
	 * 返回值：
	 * 	0：未被用过
	 * 	1：被用过
	 */
	if(seg_code.head==NULL)
		return 0;
	struct Code* p_code = seg_code.head;
	while(1)
	{
		if(operandIsUsedInCode(opd,p_code))
		{
			return 1;
		}
		if(p_code==seg_code.tail)
			break;
		p_code = p_code->next;
	}
	return 0;
}
int operandIsUsedInCode(const struct Operand* opd,const struct Code* code)
{
	/* 函数功能：
	 * 	判断操作数是否以右值身份在代码行中份被用过
	 * 参数：
	 * 	opd：要被判断的操作数
	 * 	code：被判断的该代行
	 * 返回值：
	 * 	0：未被用过
	 * 	1：被用过
	 */
	if(opd==NULL || code==NULL)
	{
		dbg_constprt("程序内部错误(%s,%d)：操作数或中间代码是空指针\r\n",__FILE__,__LINE__);
		return 1;
	}
	const char* name = opd->name;
	switch(code->op)
	{
		case OP_LAB:
		case OP_FUN:
		case OP_DEC:
		case OP_PAR:
		case OP_RED:
			{
				return 0;
			}
		case OP_ASS:
			{
				if(0 == strcmp(name,code->op1->name) && code->op1->kind==OPD_GCN)
					return 1;	//如果是对左值以Get CoNtent 的方式使用，也算被使用过。下同。
				if(0 == strcmp(name,code->op2->name))
					return 1;
				return 0;
			}
		case OP_ADD:
		case OP_SUB:
		case OP_MUL:
		case OP_DIV:
			{
				if(0 == strcmp(name,code->op1->name) && code->op1->kind==OPD_GCN)
					return 1;
				if(0 == strcmp(name,code->op2->name))
					return 1;
				if(0 == strcmp(name,code->op3->name))
					return 1;
				return 0;
			}
		case OP_GTO:
		case OP_RET:
		case OP_ARG:
		case OP_WRI:
			{
				if(0 == strcmp(name,code->op1->name))
					return 1;
				return 0;
			}
		
		case OP_IFJ:
			{
				if(0 == strcmp(name,code->op1->name))
					return 1;
				if(0 == strcmp(name,code->op3->name))
					return 1;
				if(0 == strcmp(name,code->op4->name))
					return 1;
				return 0;
			}
		case OP_CAL:
			{
				if(0 == strcmp(name,code->op1->name) && code->op1->kind==OPD_GCN)
					return 1;
				if(0 == strcmp(name,code->op2->name))
					return 1;
				return 0;
			}
		default:
			{
				dbg_constprt("程序内部错误(%s,%d)：未知的中间代码类型%d\r\n",__FILE__,__LINE__,code->op);
				return 0;
			}
	}
}
void deleteOneCode(struct Code* code)
{
	/* 函数功能：
	 * 	在全部代码段中删除一行中间代码
	 * 参数：
	 * 	要被删除的一行代码
	 */
	if(global_code_segment.head==NULL)
		return;
	if(code==NULL)
		return;
	if(global_code_segment.head==code && global_code_segment.tail ==code)
	{
		global_code_segment.head=NULL;
		global_code_segment.tail=NULL;
		freeCode(code);
		return;
	}
	if(global_code_segment.head==code)
	{
		global_code_segment.head = code->next;
		freeCode(code);
		return;
	}
	if(global_code_segment.tail==code)
	{
		global_code_segment.tail = code->prev;
		freeCode(code);
		return;
	}
	code->prev->next = code->next;
	code->next->prev = code->prev;
	freeCode(code);
	return;
}
struct Operand* getLeftOperand(struct Code* code)
{
	/* 函数功能：
	 * 	获取中间代码的左值操作数
	 * 参数：
	 * 	要被检查的一行中间代码
	 * 返回值：
	 * 	NULL：函数出错，或该中间代码没有左值操作数
	 * 	not NULL：中间代码的左值操作数
	 */
	if(code==NULL)
		return NULL;
	switch(code->op)
	{
		case OP_LAB:
		case OP_ASS:
		case OP_ADD:
		case OP_SUB:
		case OP_MUL:
		case OP_DIV:
		case OP_RED:
			{
				return code->op1;
			}
		case OP_FUN:
		case OP_GTO:
		case OP_IFJ:
		case OP_RET:
		case OP_DEC:
		case OP_ARG:
		case OP_CAL:
		case OP_PAR:
		case OP_WRI:
			{
				return NULL;
			}
		default:
			{
				dbg_constprt("程序内部错误(%s,%d)：未知的中间代码类型%d\r\n",__FILE__,__LINE__,code->op);
				return NULL;
			}
	}
}
int opt_usageCheck()
{
	/*
	 * 函数功能：
	 * 	检查并删除中间代码中生成左值后不再被用过的变量
	 * 返回值：
	 * 	被删除的代码的数量
	 * 其他说明：
	 * 	从第一条代码向最后一条代码遍历
	 */
	int cnt=0;
	struct Code* code=global_code_segment.head;
	while(code!=NULL)
	{
		if(code->op!=OP_RED)	//要保持用户体验，所以不能删除OP_RED命令。
		{
			struct Operand* opd = getLeftOperand(code);
			if(opd!=NULL)
			{
				if(opd->kind==OPD_GCN)
				{
					code = code->next;
					continue;
				}
				else if(operandIsUsedInCodeSegment(opd,global_code_segment)==0)
				{
					struct Code* next_code = code->next;
					deleteOneCode(code);
					code = next_code;
					cnt++;
					continue;	//因为code被清除了，所以这里必须专门准备下一条code，然后continue
				}
			}
		}
		code = code->next;
	}
	return cnt;
}
//===============================================
int opt_calculateFourOperatopnConst()
{
	/* 函数功能：
	 * 	把x = #const [+ - * /] #const 改写为x = #const
	 * 返回值：
	 * 	优化了多少条代码
	 */
	int cnt = 0;
	struct Code* code = global_code_segment.head;
	while(code!=NULL)
	{
		if(code->op==OP_ADD || code->op==OP_SUB || code->op==OP_MUL || code->op==OP_DIV)
		{
			struct Operand* opd2 = code->op2;
			struct Operand* opd3 = code->op3;
			if(opd2->kind==OPD_CON && opd3->kind==OPD_CON)
			{
				int value2 = atoi((opd2->name)+1);
				int value3 = atoi((opd3->name)+1);
				int value1 = 0;
				switch(code->op)
				{
					case OP_ADD:	value1=value2+value3;	break;
					case OP_SUB:	value1=value2-value3;	break;
					case OP_MUL:	value1=value2*value3;	break;
					case OP_DIV:	value1=value2/value3;	break;
				}
				code->op = OP_ASS;
				code->op2->kind = OPD_CON;	//没改变其值
				sprintf(code->op2->name,"#%d",value1);
				freeOperand(code->op3);
				code->op3=NULL;
				cnt++;
			}
		}
		code = code->next;
	}
	return cnt;
}
//==================================================================================
struct Code* findSourceAssCode(const struct Operand* opd,const struct Code* code)
{
	/* 函数功能：
	 * 	对于一个右值操作数，希望能追根溯源找到它的被OP_ASS等代码赋值的地方
	 * 参数：
	 * 	opd：用于查询的右值操作数
	 * 	code：opd所在的那一行中间代码
	 * 返回值：
	 * 	查找到的OP_ASS等给opd赋值的代码（代码左值对应于opd）
	 * 	NULL：没找到
	 * 其他说明：
	 * 	逆序查找
	 * 	遇到func/label/中间代码开头，则停止
	 * 	opd与lftopd必须完全一样！
	 * 	如果opd是OPD_GCN指针类型(*)的，则只查上方紧邻的赋值（大类）语句，并停止。因为再往上难以判断opd所指向的内容具体是谁。
	 * 	如果opd是OPD_LAB/OPD_FUN/OPD_CON/OPD_REL/OPD_GAD类型的，则直接返回NULL，因为它们不可能为左值。即只处理OPD_VAR和OPD_GCN
		虽然返回了OP_ASS/OP_ADD/OP_SUB/OP_MUL/OP_DIV，但建议上层只是用OP_ASS
	 */
	if(code==NULL)
		return NULL;
	if(opd==NULL)
		return NULL;
	if(opd->kind!=OPD_VAR && opd->kind!=OPD_GCN)
		return NULL;
	struct Code* code_rtn = NULL;	//函数返回值
	struct Code* p_code = code->prev;
	while(p_code!=NULL)
	{
		if(p_code->op==OP_FUN || p_code->op==OP_LAB)
			break;
		struct Operand* lftopd = getLeftOperand(p_code);
		if(lftopd!=NULL)
		{
			if(opd->kind==OPD_GCN)
			{
				if(lftopd->kind!=OPD_GCN)
				{
					code_rtn = NULL;
					break;
				}
				/*
				 *	*opd/y:=...
				 *	.........
				 *	...:=*opd
				 */
				else if(0==strcmp(opd->name,lftopd->name))
				{
					code_rtn = p_code;
					break;
				}
				else
				{
					code_rtn = NULL;
					break;
				}
				break;
			}
			else if(opd->kind==OPD_VAR)
			{
				if(lftopd->kind==OPD_GCN)
				{
					//无法预测指针指向的内容是不是opd
					code_rtn = NULL;
					break;
				}
				else if(lftopd->kind==OPD_VAR)
				{
					if(0==strcmp(opd->name,lftopd->name))
					{
						code_rtn = p_code;
						break;
					}
					else
						;
				}
				else
				{
					dbg_constprt("程序内部错误(%s,%d)：意料外的操作数类型%d\r\n",__FILE__,__LINE__,lftopd->kind);
					code_rtn = NULL;
					break;
				}

			}
			else
			{
				dbg_constprt("程序内部错误(%s,%d)：意料外的操作数类型%d\r\n",__FILE__,__LINE__,opd->kind);
				code_rtn = NULL;
				break;
			}
		}
		p_code = p_code->prev;
	}
	//不能返回OP_RED代码，因为用户只会执行一次！
	//也不应该返回OP_LAB代码
	//虽然返回了OP_ASS/OP_ADD/OP_SUB/OP_MUL/OP_DIV，但建议上层只是用OP_ASS
	if(code_rtn!=NULL &&(code_rtn->op==OP_RED||code_rtn->op==OP_LAB))
		return code_rtn = NULL;
	return code_rtn;
}
int opt_traceRightOpd()
{
	/* 函数功能：
	 * 	对于所有...:=...[op ...]形式的中间代码，如果等号右侧的变量x是之前由s赋值过的一个变量x，则尝试在此直接使用s
	 * 返回值：
	 * 	更改的中间代码的数量
	 */
	int cnt=0;
	struct Code* code = global_code_segment.tail;
	while(code!=NULL)
	{
		switch(code->op)
		{
			case OP_LAB:
			case OP_FUN:
			case OP_GTO:
			case OP_DEC:
			case OP_CAL:
			case OP_PAR:
			case OP_RED:
				{
					break;
				}
			case OP_ASS:
				{
					struct Operand* opd2 = code->op2;
					if(opd2->kind==OPD_VAR || opd2->kind==OPD_GCN)
					{
						struct Code* code_src = findSourceAssCode(opd2,code);
						if(code_src!=NULL)
						{
							if(code_src->op==OP_ASS)
							{
								opd2->kind = code_src->op2->kind;
								strcpy(opd2->name,code_src->op2->name);
								cnt++;
							}
						}
					}
					break;
				}
			case OP_ADD:
			case OP_SUB:
			case OP_MUL:
			case OP_DIV:
				{
					struct Operand* opd2 = code->op2;
					if(opd2->kind==OPD_VAR || opd2->kind==OPD_GCN)
					{
						struct Code* code_src = findSourceAssCode(opd2,code);
						if(code_src!=NULL)
						{
							if(code_src->op==OP_ASS)
							{
								opd2->kind = code_src->op2->kind;
								strcpy(opd2->name,code_src->op2->name);
								cnt++;
							}
						}
					}
					struct Operand* opd3 = code->op3;
					if(opd3->kind==OPD_VAR || opd3->kind==OPD_GCN)
					{
						struct Code* code_src = findSourceAssCode(opd3,code);
						if(code_src!=NULL)
						{
							if(code_src->op==OP_ASS)
							{
								opd3->kind = code_src->op2->kind;
								strcpy(opd3->name,code_src->op2->name);
								cnt++;
							}
						}
					}
					break;
				}
			case OP_RET:
			case OP_ARG:
			case OP_WRI:				
				{
					struct Operand* opd1 = code->op1;
					if(opd1->kind==OPD_VAR || opd1->kind==OPD_GCN)
					{
						struct Code* code_src = findSourceAssCode(opd1,code);
						if(code_src!=NULL)
						{
							if(code_src->op==OP_ASS)
							{
								opd1->kind = code_src->op2->kind;
								strcpy(opd1->name,code_src->op2->name);
								cnt++;
							}
						}
					}
					break;
				}
			case OP_IFJ:
				{
					struct Operand* opd1 = code->op1;
					if(opd1->kind == OPD_VAR)
					{
						struct Code* code_src = findSourceAssCode(opd1,code);
						if(code_src!=NULL)
						{
							if(code_src->op == OP_ASS)
							{
								if(opd1->kind==code_src->op2->kind)
								{
									strcpy(opd1->name,code_src->op2->name);
									cnt++;
								}
								else if(code_src->op2->kind==OPD_CON)		//冒险的处理：允许IF *** GOTO ***中出现对常数的比较，经测试正常
								{
									opd1->kind == OPD_CON;
									strcpy(opd1->name,code_src->op2->name);
									cnt++;
								}
							}
						}
					}
					struct Operand* opd3 = code->op3;
					if(opd3->kind == OPD_VAR)
					{
						struct Code* code_src = findSourceAssCode(opd3,code);
						if(code_src!=NULL)
						{
							if(code_src->op == OP_ASS)
							{
								if(opd3->kind == code_src->op2->kind)
								{
									strcpy(opd3->name,code_src->op2->name);
									cnt++;
								}
								else if(code_src->op2->kind==OPD_CON)		//冒险的处理：允许IF *** GOTO ***中出现对常数的比较，经测试正常
								{
									opd3->kind == OPD_CON;
									strcpy(opd3->name,code_src->op2->name);
									cnt++;
								}
							}
						}
					}
					break;
				}
			default:
				{
					dbg_constprt("程序内部错误(%s,%d)：未知的中间代码类型%d\r\n",__FILE__,__LINE__,code->op);
				}
		}
		code = code->prev;
	}
	return cnt;
}
//===============================================
int opt_ignoreInvalidFourOperationConst()
{
	/* 函数功能：
	 * 	把无效的四则运算变为赋值语句
	 * 返回值：
	 * 	优化改动的代码数量
	 */

	int cnt=0;
	struct Code* code = global_code_segment.head;
	while(code!=NULL)
	{
		switch(code->op)
		{
			case OP_ADD:
				{
					if(code->op2->kind==OPD_CON && 0==strcmp("#0",code->op2->name))
					{
						// a := #0 + b 的情况
						code->op=OP_ASS;
						freeOperand(code->op2);
						code->op2=code->op3;
						code->op3=NULL;
						cnt++;
					}
					else if(code->op3->kind==OPD_CON && 0==strcmp("#0",code->op3->name))
					{
						// a := b + #0 的情况
						code->op = OP_ASS;
						freeOperand(code->op3);
						code->op3=NULL;
						cnt++;
					}
					break;
				}
			case OP_SUB:
				{
					if(code->op3->kind==OPD_CON && 0==strcmp("#0",code->op3->name))
					{
						// a := b - #0 的情况
						code->op = OP_ASS;
						freeOperand(code->op3);
						code->op3=NULL;
						cnt++;
					}
					if(code->op2->kind == code->op3->kind && 0==strcmp(code->op2->name,code->op3->name))
					{
						code->op=OP_ASS;
						freeOperand(code->op3);
						code->op3=NULL;
						code->op2->kind=OPD_CON;
						strcpy(code->op2->name,"#0");
					}
					break;
				}
			case OP_MUL:
				{
					if(code->op2->kind==OPD_CON && 0==strcmp("#1",code->op2->name))
					{
						// a := #1 * b 的情况
						code->op=OP_ASS;
						freeOperand(code->op2);
						code->op2=code->op3;
						code->op3=NULL;
						cnt++;
					}
					else if(code->op3->kind==OPD_CON && 0==strcmp("#0",code->op3->name))
					{
						// a := b * #1 的情况
						code->op = OP_ASS;
						freeOperand(code->op3);
						code->op3=NULL;
						cnt++;
					}
					break;
				}
			case OP_DIV:
				{
					if(code->op3->kind==OPD_CON && 0==strcmp("#0",code->op3->name))
					{
						// a := b / #1 的情况
						code->op = OP_ASS;
						freeOperand(code->op3);
						code->op3=NULL;
						cnt++;
					}
					break;
				}
		}
		code=code->next;
	}
	return cnt;
}
//===================================================================================
int opt_duplexCheck()
{
	/* 函数功能：
	 * 	若由相邻的a := Exp; b := a且a没再被用，把两行代码转换为一行 b := Exp
	 * 返回值：
	 * 	优化删除的代码数量
	 * 其他说明：
	 * 	因虚拟机限制，如果b为指针OPG_GCN类型，则Exp中不能有运算符
	 * 	即不允许出现*t := .. op ..语句
	 */
	struct Code* code1 = global_code_segment.head;
	if(code1==NULL)
		return 0;
	struct Code* code2 = code1->next;
	int cnt = 0;
	while(code2!=NULL)
	{
		if(code2->op==OP_ASS)
		{
			if(code2->op1->kind == OPD_GCN && code1->op2!=NULL)
			{
	 			//因虚拟机限制，如果b为指针OPG_GCN类型，则Exp中不能有运算符，不能是CALL
				code1 = code1->next;
				code2 = code2->next;
				continue;
			}
			if(code2->op1->kind == OPD_GCN && code1->op==OP_RED)
			{
	 			//因虚拟机限制，如果b为指针OPG_GCN类型，则Exp不能来自READ
				code1 = code1->next;
				code2 = code2->next;
				continue;
			}
			struct Operand* opd_a1 = getLeftOperand(code1);
			if(opd_a1!=NULL)
			{
				if(opd_a1->kind == code2->op2->kind && 0==strcmp(opd_a1->name,code2->op2->name))
				{
					//至此已经获得了符合形式的两行相邻代码，下面检查a是否被其他地方以右值方式使用
					//先构造前方的代码段
					if(code1->prev!=NULL)
					{
						struct CodeSegment seg_former;
						seg_former.head = global_code_segment.head;
						seg_former.tail = code1->prev;
						if(operandNameExistInCodeSegment(opd_a1->name,seg_former)==1)
						{
							code1 = code1->next;
							code2 = code2->next;
							continue;
						}
					}
					if(code2->next!=NULL)
					{
						struct CodeSegment seg_later;
						seg_later.head = code2->next;
						seg_later.tail = global_code_segment.tail;
						if(operandNameExistInCodeSegment(opd_a1->name,seg_later)==1)
						{
							code1 = code1->next;
							code2 = code2->next;
							continue;
						}
					}
					//检查发现没有其他地方用到a，开始合并两条代码
					code1->op1->kind = code2->op1->kind;
					strcpy(code1->op1->name,code2->op1->name);
					deleteOneCode(code2);
					//合并完成，准备下一次循环
					code2 = code1->next;
					cnt++;
					continue;
				}
			}
		}
		code1 = code1->next;
		code2 = code2->next;
	}
	return cnt;
}
int operandNameExistInCodeSegment(const char* name,struct CodeSegment seg_code)
{
	/* 函数功能：
	 * 	检查变量名是否在给定的代码段中的代码行里出现过
	 * 参数：
	 * 	name：变量名
	 * 	seg_code：给定的代码段范围
	 * 返回值：
	 * 	0：没出现过
	 * 	1：出现过
	 */
	struct Code* code = seg_code.head;
	if(code==NULL)
		return 0;
	while(code!=seg_code.tail->next)
	{
		if(code->op1!=NULL && 0==strcmp(name,code->op1->name))
			return 1;
		if(code->op2!=NULL && 0==strcmp(name,code->op2->name))
			return 1;
		if(code->op3!=NULL && 0==strcmp(name,code->op3->name))
			return 1;
		if(code->op4!=NULL && 0==strcmp(name,code->op4->name))
			return 1;
		code = code->next;
	}
	return 0;
}
//===================================================================================
int opt_clearJump()
{
	/* 函数功能：
	 * 	对紧邻label的jump和goto语句进行优化
	 * 返回值：
	 * 	优化删除的代码数量
	 * 其他说明：
	 * 	从首代码向尾代码顺序遍历
	 */
	struct Code* code = global_code_segment.head;
	int cnt=0;
	while(code!=NULL)
	{
		if(code->op==OP_GTO)
		{
			if(code->next!=NULL && code->next->op==OP_LAB && 0==strcmp(code->op1->name,code->next->op1->name))
			{
				//如果goto 到的就是下一条代码，则goto无用。删除之
				struct Code* code_next = code->next;
				deleteOneCode(code);
				code = code_next;
				cnt++;
				continue;
			}
		}

		if(code->op==OP_IFJ)
		{
			if(code->next!=NULL && code->next->op==OP_LAB && 0==strcmp(code->op4->name,code->next->op1->name))
			{
				//如果ifjump 到的就是下一条代码，则ifjump无用。删除ifjmp（暂保留目标label）
				struct Code* code_next = code->next;
				deleteOneCode(code);
				code = code_next;
				cnt++;
				continue;
			}
		}
		if(code->op==OP_LAB)
		{
			//如果目标label没有跳转源，则删除之
			char* name = code->op1->name;
			//先搜之前部分
			if(code->prev!=NULL)
			{
				struct CodeSegment seg_former;
				seg_former.head = global_code_segment.head;
				seg_former.tail =  code->prev;
				if(labelSorceExistInCodeSegment(name,seg_former))
				{
					goto next_check;
				}
			}
			if(code->next!=NULL)
			{
				struct CodeSegment seg_later;
				seg_later.head = code->next;
				seg_later.tail = global_code_segment.tail;
				if(labelSorceExistInCodeSegment(name,seg_later))
				{
					goto next_check;
				}
			}
			struct Code* code_next = code->next;
			deleteOneCode(code);
			code = code_next;
			cnt++;
			continue;
		}
	next_check:
		if(code->op==OP_LAB)
		{
			if(code->next!=NULL && code->next->op==OP_LAB)
			{
				//如果连着两个目标label，则这两个label可以合并。这里选择保留第一个，删除第二个
				char name2[64]="";
				strcpy(name2,code->next->op1->name);
				deleteOneCode(code->next);	//删除第二个label
				struct Code* labsrc_code = labelSorceExistInCodeSegment(name2,global_code_segment);
				while(labsrc_code!=NULL)
				{
					if(labsrc_code->op==OP_GTO)
					{
						strcpy(labsrc_code->op1->name,code->op1->name);
						cnt++;
					}
					else if(labsrc_code->op==OP_IFJ)
					{
						strcpy(labsrc_code->op4->name,code->op1->name);
						cnt++;
					}
					else
					{
						dbg_constprt("程序内部错误(%s,%d)：意料外的代码类型%d\r\n",__FILE__,__LINE__,labsrc_code->op);
					}
					labsrc_code = labelSorceExistInCodeSegment(name2,global_code_segment);
				}
			}
		}
		if(code->op==OP_LAB)
		{
			//如果目标label1的下一条代码就是一个goto label2，则可以删除掉目标label1并把所有的label2发起源改为跳转到label2
			if(code->next!=NULL && code->next->op==OP_GTO)
			{
				char label1[64]="";
				char label2[64]="";
				strcpy(label1,code->op1->name);
				strcpy(label2,code->next->op1->name);
				struct Code* labsrc_code = labelSorceExistInCodeSegment(label1,global_code_segment);
				while(labsrc_code!=NULL)
				{
					if(labsrc_code->op==OP_GTO)
					{
						strcpy(labsrc_code->op1->name,label2);
						cnt++;
					}
					else if(labsrc_code->op==OP_IFJ)
					{
						strcpy(labsrc_code->op4->name,label2);
						cnt++;
					}
					else
					{
						dbg_constprt("程序内部错误(%s,%d)：意料外的代码类型%d\r\n",__FILE__,__LINE__,labsrc_code->op);
					}
					labsrc_code = labelSorceExistInCodeSegment(label1,global_code_segment);
				}
				struct Code* code_next = code->next;
				deleteOneCode(code);
				code = code_next;
				continue;
			}
		}
		if(code->op==OP_IFJ)
		{
			/*
			 * IF(......)GOTO label1
			 * GOTO label2
			 * LABEL label1:
			 * 改为
			 * IF(not ......)GOTO label2
			 * LABEL label1:
			 */
			if(code->next!=NULL && code->next->next!=NULL)
			{
				struct Code* code2 = code->next;
				struct Code* code3 = code2->next;
				if(code2->op==OP_GTO && code3->op==OP_LAB)
				{
					if(0==strcmp(code->op4->name,code3->op1->name))
					{
						//改code
						if(0==strcmp(code->op2->name,"=="))
						{
							strcpy(code->op2->name,"!=");
						}
						else if(0==strcmp(code->op2->name,"!="))
						{
							strcpy(code->op2->name,"==");
						}
						else if(0==strcmp(code->op2->name,">"))
						{
							strcpy(code->op2->name,"<=");
						}
						else if(0==strcmp(code->op2->name,"<="))
						{
							strcpy(code->op2->name,">");
						}
						else if(0==strcmp(code->op2->name,"<"))
						{
							strcpy(code->op2->name,">=");
						}
						else if(0==strcmp(code->op2->name,">="))
						{
							strcpy(code->op2->name,"<");
						}
						else
						{
							dbg_constprt("程序内部错误(%s,%d)：意料之外的关系比较符%s\r\n",__FILE__,__LINE__,code->op2->name);
							code = code->next;
							continue;
						}
						strcpy(code->op4->name,code2->op1->name);
						//删除code2
						deleteOneCode(code2);
						cnt++;
					}
				}
			}
		}
		code = code->next;
	}
	return cnt;
}
struct Code* labelTargetExistInCodeSegment(const char* labelname,struct CodeSegment seg_code)
{
	/* 函数功能：
	 * 	在给定的代码段中寻找名为labelname的目标label
	 * 参数：
	 * 	labelname：label名
	 * 	seg_code：在这个代码段中查询
	 * 返回值：
	 * 	NULL：没找到对应名称的label
	 * 	非NULL：存在该目标label的代码行（如有多个则返回第一个发现的）
	 * 其他说明：
	 * 	从首代码向尾代码顺序查找
	 */
	struct Code* code = seg_code.head;
	if(code==NULL)
		return NULL;
	while(code=seg_code.tail->next)
	{
		if(code->op==OP_LAB)
		{
			if(0==strcmp(labelname,code->op1->name))
			{
				return code;
			}
		}
		code = code->next;
	}
	return NULL;
}
struct Code* labelSorceExistInCodeSegment(const char* labelname,struct CodeSegment seg_code)
{
	/* 函数功能：
	 * 	在给定的代码段中寻找名为labelname的跳转主动发起方label
	 * 参数：
	 * 	labelname：label名
	 * 	seg_code：在这个代码段中查询
	 * 返回值：
	 * 	NULL：没找到对应名称的label
	 * 	非NULL：存在该跳转发起label的代码行（如有多个则返回第一个发现的）
	 * 其他说明：
	 * 	从首代码向尾代码顺序查找
	 */
	struct Code* code = seg_code.head;
	if(code==NULL)
		return NULL;
	while(code!=seg_code.tail->next)
	{
		switch(code->op)
		{
			case OP_GTO:
				{
					if(0==strcmp(labelname,code->op1->name))
						return code;
					break;
				}
			case OP_IFJ:
				{
					if(0==strcmp(labelname,code->op4->name))
						return code;
					break;
				}
		}
		code = code->next;
	}
	return NULL;
}
