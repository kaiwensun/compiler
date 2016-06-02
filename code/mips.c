#include"mips.h"
struct MIPSCode* global_mipscode_head = NULL;
struct MIPSCode* global_mipscode_tail = NULL;
struct FuncLocalvarList* global_funclvlist = NULL;
void appendMipsCode(char* str)
{
	/* 函数功能：
	 * 	添加一行MIPS代码
	 * 参数：
	 * 	要添加的代码文本
	 * 其他说明：
	 * 	直接被添加到了全局代码链表中
	 */
	struct MIPSCode* newcode = (struct MIPSCode*)malloc(sizeof(struct MIPSCode)); 
	strcpy(newcode->mcode,str);
	newcode->next = NULL;
	if(global_mipscode_head == NULL)
	{
		global_mipscode_head = newcode;
		global_mipscode_tail = newcode;
	}
	else
	{
		global_mipscode_tail->next = newcode;
		global_mipscode_tail = newcode;
	}
}
int freeMIPSCodeList(struct MIPSCode* mcode)
{
	/* 函数功能：
	 * 	释放MIPSCode链表空间
	 * 参数：
	 * 	要被释放的MIPSCode链表
	 * 返回值：
	 * 	释放的MIPSCode节点个数
	 */
	struct MIPSCode* p = mcode;
	int cnt = 0;
	while(mcode!=NULL)
	{
		struct MIPSCode* n = p->next;
		free(p);
		p = n;
		cnt++;
	}
	return cnt;
}
struct FuncLocalVar* searchLocalvarByName(struct FuncLocalVar* varlist, const char* name)
{
	/* 函数功能：
	 * 	在局部变量/形参链表中搜索制定名称的变量/形参
	 * 参数：
	 * 	varlist：局部变量/形参链表
	 * 	name：变量名
	 * 返回值：
	 * 	非NULL：找到的局部变量/形参
	 * 	NULL：没找到
	 */
	struct FuncLocalVar* curvar = varlist;
	while(curvar!=NULL)
	{
		if(0==strcmp(name,curvar->varname))
			break;
		curvar = curvar->next;
	}
	return curvar;
}
struct FuncLocalvarList* searchFuncLocalvarListByName(const char* name,struct FuncLocalvarList* funclist)
{
	/* 函数功能：
	 * 	根据函数名再静态活动记录链表中搜索对应的活动记录
	 * 参数：
	 * 	name：函数名
	 * 	funclist：被搜索的静态活动记录链表
	 * 返回值：
	 * 	非NULL：搜索到的静态活动记录节点
	 * 	NULL：没有搜索到
	 */
	struct FuncLocalvarList* cur_func = funclist;
	while(cur_func!=NULL)
	{
		if(0==strcmp(name,cur_func->funcname))
			break;
		cur_func = cur_func->next;
	}
	return cur_func;
}
int addOpdToList(const struct Operand* opd,struct FuncLocalVar** varlist,int offset)
{
	/* 函数功能：
	 * 	把opd里的变量名放到varlist里
	 * 参数：
	 * 	opd：中间代码中的一个操作数
	 * 	varlist：局部变量/形参链表
	 * 	newoffset：该变量地址相对于SP的偏移量
	 * 返回值：
	 * 	0：加入成功
	 * 	-1：拒绝加入
	 * 其他说明：
	 * 	注意加入链表的顺序(加在链表首)
	 * 	不负责查重
	 */
	if(opd->kind == OPD_CON)
	{
		return -1;
	}
	if(opd->kind == OPD_LAB || opd->kind == OPD_FUN || opd->kind == OPD_REL || opd->kind < 0)
	{
		dbg_constprt("程序内部错误(%s,%d)：局部变量/参数列表中意料外的操作数类型%d\r\n",__FILE__,__LINE__,opd->kind);
		return -1;
	}
	const char* name = opd->name;
	struct FuncLocalVar* var;
	var = (struct FuncLocalVar*)malloc(sizeof(struct FuncLocalVar));
	strcpy(var->varname,name);
	var->offset = offset;
	var->regnum = -1;
	var->next = (*varlist);
	*varlist = var;
	return 0;
}
int countLVStack(struct FuncLocalvarList* funcvars,struct Code* code,int oldoffset)
{
	/* 函数功能：
	 * 	计算code中涉及到的局部变量在栈中的偏移量
	 * 参数：
	 * 	funclv：所在的函数
	 * 	code：要处理的code
	 * 	oldoffset：之前已经计算得的压栈到的偏移量
	 * 返回值：
	 * 	新的压栈偏移量
	 * 其他说明：
	 * 	可以处理形参code
	 */
	int newoffset = oldoffset;
	char* name = NULL;
	switch(code->op)
	{
		case OP_ASS:
			{
				name = code->op1->name;
				if(searchLocalvarByName(funcvars->arglist,name)==NULL && searchLocalvarByName(funcvars->varlist,name)==NULL)
				{
					if(0==addOpdToList(code->op1,&(funcvars->varlist),newoffset))
						newoffset += 4;
				}
				name = code->op2->name;
				if(searchLocalvarByName(funcvars->arglist,name)==NULL && searchLocalvarByName(funcvars->varlist,name)==NULL)
				{
					if(0==addOpdToList(code->op2,&(funcvars->varlist),newoffset))
						newoffset += 4;
				}
				break;
			}
		case OP_ADD:
		case OP_SUB:
		case OP_MUL:
		case OP_DIV:
			{
				name = code->op1->name;
				if(searchLocalvarByName(funcvars->arglist,name)==NULL && searchLocalvarByName(funcvars->varlist,name)==NULL)
				{
					if(0==addOpdToList(code->op1,&(funcvars->varlist),newoffset))
						newoffset += 4;
				}
				name = code->op2->name;
				if(searchLocalvarByName(funcvars->arglist,name)==NULL && searchLocalvarByName(funcvars->varlist,name)==NULL)
				{
					if(0==addOpdToList(code->op2,&(funcvars->varlist),newoffset))
						newoffset += 4;
				}
				name = code->op3->name;
				if(searchLocalvarByName(funcvars->arglist,name)==NULL && searchLocalvarByName(funcvars->varlist,name)==NULL)
				{
					if(0==addOpdToList(code->op3,&(funcvars->varlist),newoffset))
						newoffset += 4;
				}
				break;
			}
		case OP_IFJ:
			{
				name = code->op1->name;
				if(searchLocalvarByName(funcvars->arglist,name)==NULL && searchLocalvarByName(funcvars->varlist,name)==NULL)
				{
					if(0==addOpdToList(code->op1,&(funcvars->varlist),newoffset))
						newoffset += 4;
				}
				name = code->op3->name;
				if(searchLocalvarByName(funcvars->arglist,name)==NULL && searchLocalvarByName(funcvars->varlist,name)==NULL)
				{
					if(0==addOpdToList(code->op3,&(funcvars->varlist),newoffset))
					{
						newoffset += 4;
					}
				}
				break;
		       	}
		case OP_RET:
			{
				name = code->op1->name;
				if(searchLocalvarByName(funcvars->arglist,name)==NULL && searchLocalvarByName(funcvars->varlist,name)==NULL)
				{
					if(0==addOpdToList(code->op1,&(funcvars->varlist),newoffset))
					{
						dbg_constprt("程序内部警告(%s,%d)：返回值%s既不是形参也不是局部变量\r\n",__FILE__,__LINE__,name);	//（可能是常量）
						newoffset += 4;
					}
				}
				break;
			}
		case OP_DEC:
			{
				name = code->op1->name;
				if(searchLocalvarByName(funcvars->arglist,name)==NULL && searchLocalvarByName(funcvars->varlist,name)==NULL)
				{
					int decsize = atoi(code->op2->name);	//DEC 后面的常量不带井号#
					if(0==addOpdToList(code->op1,&(funcvars->varlist),newoffset))
						newoffset += decsize;
					else
						dbg_constprt("程序内部警告(%s,%d)：变量%s的DEC不合法\r\n",__FILE__,__LINE__,name);
				}
				else
				{
					dbg_constprt("程序内部警告(%s,%d)：变量%s在空间申请前就被加入了局部变量/形参链表\r\n",__FILE__,__LINE__,name);
				}
				break;
			}
		case OP_ARG:
			{
				name = code->op1->name;
				if(searchLocalvarByName(funcvars->arglist,name)==NULL && searchLocalvarByName(funcvars->varlist,name)==NULL)
				{
					if(0==addOpdToList(code->op1,&(funcvars->varlist),newoffset))
					{
						newoffset += 4;
						dbg_constprt("程序内部警告(%s,%d)：变量%s的ARG不合法\r\n",__FILE__,__LINE__,name);
					}
				}
				break;
			}
		case OP_CAL:
		case OP_RED:
		case OP_WRI:
			{
				name = code->op1->name;
				if(searchLocalvarByName(funcvars->arglist,name)==NULL && searchLocalvarByName(funcvars->varlist,name)==NULL)
				{
					if(0==addOpdToList(code->op1,&(funcvars->varlist),newoffset))
						newoffset += 4;
				}
				break;
			}
		case OP_PAR:
			{
				name = code->op1->name;
				if(searchLocalvarByName(funcvars->arglist,name)==NULL && searchLocalvarByName(funcvars->varlist,name)==NULL)
				{
					if(0==addOpdToList(code->op1,&(funcvars->arglist),newoffset))
					{
						newoffset +=4;
					}
					else
					{
						dbg_constprt("程序内部错误(%s,%d)\r\n",__FILE__,__LINE__);
					}
				}
				else
				{
					dbg_constprt("程序内部警告(%s,%d)：形参%s提前出现过\r\n",__FILE__,__LINE__,name);
				}
				break;
			}
		default:
			{
				dbg_constprt("程序内部错误(%s,%d)：未知的中间代码类型%d\r\n",__FILE__,__LINE__,code->op);
			}
	}
	return newoffset;
}
void prepareFuncLocalVarList(struct CodeSegment seg)
{
	/* 函数功能：
	 * 	在把中间代码翻译为MIPS代码之前，数出每一个函数中的局部变量的个数，计算局部变量的偏移位置，以为函数调用压栈时做准备
	 */
	struct Code* code = seg.head;
	struct FuncLocalvarList* cur_funclv = NULL;
	while(1)
	{
		switch(code->op)
		{
			case OP_LAB:
			case OP_GTO:
				{
					break;
				}
			case OP_FUN:
				{
					cur_funclv = (struct FuncLocalvarList*)malloc(sizeof(struct FuncLocalvarList));
					strcpy(cur_funclv->funcname,code->op1->name);
					cur_funclv->localvar_cnt = 0;
					cur_funclv->varlist = NULL;
					cur_funclv->localarg_cnt = 0;
					cur_funclv->arglist = NULL;
					cur_funclv->next = global_funclvlist;
					global_funclvlist = cur_funclv;
					break;
				}
			case OP_ASS:
			case OP_ADD:
			case OP_SUB:
			case OP_MUL:
			case OP_DIV:
			case OP_IFJ:
			case OP_RET:
			case OP_DEC:
			case OP_ARG:
			case OP_CAL:
			case OP_RED:
			case OP_WRI:
				{
					cur_funclv->localvar_cnt = countLVStack(cur_funclv,code,cur_funclv->localvar_cnt);
					break;
				}
			case OP_PAR:
				{
					cur_funclv->localarg_cnt = countLVStack(cur_funclv,code,cur_funclv->localarg_cnt);
					break;
				}
			default:
				{
					dbg_constprt("程序内部错误(%s,%d)：未知的中间代码类型%d\r\n",__FILE__,__LINE__,code->op);
				}
		}
		if(code==seg.tail)
			break;
		code = code->next;
	}
	//更新形参链表相对于$sp的偏移量
	cur_funclv = global_funclvlist;
	while(cur_funclv!=NULL)
	{
		int addoffset = cur_funclv->localvar_cnt + 0;	//+0：在形参和局部变量之间有0个byte间隔
		struct FuncLocalVar* argv = cur_funclv->arglist;
		while(argv!=NULL)
		{
			argv->offset += addoffset;
			argv = argv->next;
		}
		cur_funclv->localarg_cnt += addoffset;
		cur_funclv = cur_funclv->next;
	}
	return;
}
void genSW(int reg_id,const char* varname,struct FuncLocalvarList* funclv,int flag)
{
	/* 函数功能：
	 * 	产生把指定寄存器的值存入指定变量内存中的MIPS代码，并直接加入全局代码链表中
	 * 参数：
	 * 	reg_id：寄存器编号
	 * 	varname：变量名
	 * 	lvlist：局部变量链表
	 * 	flag：
	 * 		0-取内存中存储的值
	 * 		1-取该变量的值所指向的内存的值，并使用$11寄存器辅助
	 * 其他说明：
	 * 	生成的MIPS会被加入全局代码链表中
	 */
	char mcode[256]="UNINATIALIZED\r\n";
	struct FuncLocalVar* lv = searchLocalvarByName(funclv->arglist,varname);	//先从形参链表中搜索
	if(lv==NULL)
	{
		//再从局部变量链表中搜索
		lv = searchLocalvarByName(funclv->varlist,varname);
	}
	if(lv==NULL)
	{
		dbg_constprt("程序内部错误(%s,%d)：在局部变量链表中未找到变量%s\r\n",__FILE__,__LINE__,varname);
		return;
	}
	int offset = lv->offset;
	switch(flag)
	{
		case 0:
			{
				sprintf(mcode,"sw $%d,%d($sp)",reg_id,offset);
				appendMipsCode(mcode);
				break;
			}
		case 1:
			{
				sprintf(mcode,"lw $11,%d($sp)",offset);
				appendMipsCode(mcode);
				sprintf(mcode,"sw $%d,0($11)",reg_id);
				appendMipsCode(mcode);
				break;
			}
		default:
			{
				dbg_constprt("程序内部错误(%s,%d)：非法的参数flag=%d\r\n",__FILE__,__LINE__,flag);
			}
	}
	return;
}
void genLW(int reg_id,const char* varname,struct FuncLocalvarList* funclv,int flag)
{
	/* 函数功能：
	 * 	产生把指定变量内存中的 值/地址/指针内容 存入指定寄存器的MIPS代码，并直接加入全局代码链表中
	 * 参数：
	 * 	reg_id：寄存器编号
	 * 	varname：变量名
	 * 	lvlist：局部变量链表
	 * 	flag：
	 * 		0-取内存中存储的值
	 * 		1-取该变量的值所指向的内存的值，并使用$11寄存器辅助
	 * 		2-取该变量的内存地址
	 * 		3-取常量（不期待使用这个flag值，由bug隐患）
	 * 其他说明：
	 * 	生成的MIPS会被加入全局代码链表中
	 */
	char mcode[256]="UNINATIALIZED\r\n";
	if(flag==3)
	{
		dbg_constprt("程序内部警告(%s,%d)：用genLW()函数对寄存器$%d赋常量值%s\r\n",__FILE__,__LINE__,reg_id,varname);
		sprintf(mcode,"li $%d,%d",reg_id,atoi(varname+1));
		appendMipsCode(mcode);
		return;

	}
	struct FuncLocalVar* lv = searchLocalvarByName(funclv->arglist,varname);	//先从形参链表中搜索
	if(lv==NULL)
	{
		//再从局部变量链表中搜索
		lv = searchLocalvarByName(funclv->varlist,varname);
	}
	if(lv==NULL)
	{
		dbg_constprt("程序内部错误(%s,%d)：在局部变量链表中未找到变量%s\r\n",__FILE__,__LINE__,varname);
		return;
	}
	int offset = lv->offset;
	switch(flag)
	{
		case 0:
			{
				sprintf(mcode,"lw $%d,%d($sp)",reg_id,offset);
				appendMipsCode(mcode);
				break;
			}
		case 1:
			{
				sprintf(mcode,"lw $11,%d($sp)",offset);
				appendMipsCode(mcode);
				sprintf(mcode,"lw $%d,0($11)",reg_id);
				appendMipsCode(mcode);
				break;
			}
		case 2:
			{
				sprintf(mcode,"addi $%d,$sp,%d",reg_id,offset);
				appendMipsCode(mcode);
				break;
			}
		default:
			{
				dbg_constprt("程序内部错误(%s,%d)：非法的参数flag=%d\r\n",__FILE__,__LINE__,flag);
			}
	}
	return;
}
void mipsCode(struct Code* code)
{
	/* 函数功能：
	 * 	把一行中间代码翻译成MIPS代码，并加入MIPS代码链表
	 */
	static struct FuncLocalvarList* funclist = NULL;	//函数静态活动记录
	static int parameter_offset = -4;			//实参压栈偏移量，-4是因为要保存$ra
	char mipscode[256]="UNINATIALIZED\r\n";
	char* name1 = NULL;
	char* name2 = NULL;
	char* name3 = NULL;
	char* name4 = NULL;
	switch(code->op)
	{
		case OP_LAB:
			{
				name1 = code->op1->name;
				sprintf(mipscode,"%s:",name1);
				appendMipsCode(mipscode);
				break;
			}
		case OP_FUN:
			{
				name1 = code->op1->name;
				sprintf(mipscode,"%s:",name1);
				appendMipsCode(mipscode);
				//记录：已进入这个函数
				funclist = global_funclvlist;
				while(funclist!=NULL)
				{
					if(0==strcmp(name1,funclist->funcname))
						break;
					funclist = funclist->next;
				}
				if(funclist==NULL)
				{
					dbg_constprt("程序内部错误(%s,%d)：在函数名链表中没有找到函数%s\r\n",__FILE__,__LINE__,name1);
					break;
				}
				else if(0==strcmp("main",name1))
				{
					//如果是main函数，则要在自己内部准备自己的局部变量栈空间，以及保存$ra的空间
					int mainoffset = funclist->localvar_cnt;
					sprintf(mipscode,"addi $sp,$sp,%d",-mainoffset-4);
					appendMipsCode(mipscode);
				}
				//初始化parameter_offset
				parameter_offset = -4;
				break;
			}
		case OP_ASS:
			{
				name1 = code->op1->name;
				name2 = code->op2->name;
				int flag1 = getFlag(code->op1->kind);
				int flag2 = getFlag(code->op2->kind);
				//准备右值，并赋给左值寄存器$8
				if(3==flag2)
				{
					sprintf(mipscode,"li $8,%d",atoi(name2+1));
					appendMipsCode(mipscode);
				}
				else
				{
					genLW(8,name2,funclist,flag2);
				}
				//把左值寄存器$8的值存入内存
				genSW(8,name1,funclist,flag1);
				break;
			}
		case OP_ADD:
		case OP_SUB:
		case OP_MUL:
		case OP_DIV:
			{
				name1 = code->op1->name;
				name2 = code->op2->name;
				name3 = code->op3->name;
				int flag1 = getFlag(code->op1->kind);
				int flag2 = getFlag(code->op2->kind);
				int flag3 = getFlag(code->op3->kind);
				//准备$9:=op2
				if(3==flag2)
				{
					sprintf(mipscode,"li $9,%d",atoi(name2+1));
					appendMipsCode(mipscode);
				}
				else
				{
					genLW(9,name2,funclist,flag2);
				}
				//准备$10:=op3
				if(3==flag3)
				{
					sprintf(mipscode,"li $10,%d",atoi(name3+1));
					appendMipsCode(mipscode);
				}
				else
				{
					genLW(10,name3,funclist,flag3);
				}
				//计算$8:=$9 op $10
				char opbuf[4]="";
				strcpy(opbuf,(
							code->op==OP_ADD?"add":
							(
								 code->op==OP_SUB?"sub":
								 (
								  	code->op==OP_MUL?"mul":"div"
								 )
							 )
					     ));
				sprintf(mipscode,"%s $8,$9,$10",opbuf);
				appendMipsCode(mipscode);
				//把$8存入内存
				genSW(8,name1,funclist,flag1);
				break;
			}
		case OP_GTO:
			{
				name1 = code->op1->name;
				sprintf(mipscode,"j %s",name1);
				appendMipsCode(mipscode);
				break;
			}
		case OP_IFJ:
			{
				name1 = code->op1->name;
				name2 = code->op2->name;
				name3 = code->op3->name;
				name4 = code->op4->name;
				int flag1 = getFlag(code->op1->kind);
				int flag3 = getFlag(code->op3->kind);
				//准备$9:=op1
				if(3==flag1)
				{
					sprintf(mipscode,"li $9,%d",atoi(name1+1));
					appendMipsCode(mipscode);
				}
				else
				{
					genLW(9,name1,funclist,flag1);
				}
				//准备$10:=op3
				if(3==flag3)
				{
					sprintf(mipscode,"li $10,%d",atoi(name3+1));
					appendMipsCode(mipscode);
				}
				else
				{
					genLW(10,name3,funclist,flag3);
				}
				//准备条件判断符号
				char cond[4]="";
				if(0==strcmp("==",name2))
					strcpy(cond,"beq");
				else if(0==strcmp("!=",name2))
					strcpy(cond,"bne");
				else if(0==strcmp(">",name2))
					strcpy(cond,"bgt");
				else if(0==strcmp("<",name2))
					strcpy(cond,"blt");
				else if(0==strcmp(">=",name2))
					strcpy(cond,"bge");
				else if(0==strcmp("<=",name2))
					strcpy(cond,"ble");
				else
					dbg_constprt("程序内部错误(%s.%d)：非法的条件判断符%s\r\n",__FILE__,__LINE__,name2);
				//生产MIPS代码
				sprintf(mipscode,"%s $9,$10,%s",cond,name4);
				appendMipsCode(mipscode);
				break;
			}
		case OP_RET:
			{
				//向$v0填写返回值
				name1 = code->op1->name;
				int flag1 = getFlag(code->op1->kind);
				if(3==flag1)
				{
					sprintf(mipscode,"li $2,%d",atoi(name1+1));
					appendMipsCode(mipscode);
				}
				else
				{
					genLW(2,name1,funclist,flag1);	//$v0是2号寄存器
				}
				//跳回caller
				sprintf(mipscode,"jr $ra");
				appendMipsCode(mipscode);
				break;
			}
		case OP_DEC:
			{
				/* 似乎不需要做什么事情 */
				break;
			}
		case OP_ARG:
			{
				//把caller要压的实参从内存载入寄存器$8
				name1 = code->op1->name;
				int flag1 = getFlag(code->op1->kind);
				if(3==flag1)
				{
					sprintf(mipscode,"li $8,%d",atoi(name1+1));
					appendMipsCode(mipscode);
				}
				else
				{
					genLW(8,name1,funclist,flag1);
				}
				//把左值寄存器$8的值压入栈中
				parameter_offset -= 4;
				sprintf(mipscode,"sw $8,%d($sp)",parameter_offset);
				appendMipsCode(mipscode);
				break;
			}
		case OP_CAL:
			{
				name1 = code->op1->name;
				name2 = code->op2->name;
				int flag1 = getFlag(code->op1->kind);
				//保存$ra
				sprintf(mipscode,"sw $ra, -4($sp)");
				appendMipsCode(mipscode);
				//为callee准备空间（$ra,callee的形参空间和局部变量空间）
				struct FuncLocalvarList* callee = searchFuncLocalvarListByName(name2,global_funclvlist);
				int totaloffset = callee->localarg_cnt + 4;	//+4：在形参与上一个$sp之间由4个byte的间隔（用于存储$ra）
				sprintf(mipscode,"addi $sp,$sp,%d",-totaloffset);
				appendMipsCode(mipscode);
				//跳转
				sprintf(mipscode,"jal %s",name2);
				appendMipsCode(mipscode);
				//恢复$sp
				sprintf(mipscode,"addi $sp,$sp,%d",totaloffset);
				appendMipsCode(mipscode);
				//恢复$ra
				sprintf(mipscode,"lw $ra, -4($sp)");
				appendMipsCode(mipscode);
				//获得返回值，存入内存
				genSW(2,name1,funclist,flag1);	//$v0是2号寄存器
				//更新static
				parameter_offset = -4;
				break;
			}
		case OP_PAR:
			{
				/* 似乎不需要做什么事情 */
				break;
			}
		case OP_RED:
			{
				name1 = code->op1->name;
				//调用read()函数
				//保存$ra
				sprintf(mipscode,"sw $ra, -4($sp)");
				appendMipsCode(mipscode);
				//为callee准备空间（$ra,callee的形参空间和局部变量空间）
				sprintf(mipscode,"addi $sp,$sp,-4");
				appendMipsCode(mipscode);
				//跳转到read
				sprintf(mipscode,"jal read");
				appendMipsCode(mipscode);
				//恢复$sp
				sprintf(mipscode,"addi $sp,$sp,4");
				appendMipsCode(mipscode);
				//恢复$ra
				sprintf(mipscode,"lw $ra, -4($sp)");
				appendMipsCode(mipscode);
				//获得返回值，存入内存
				int flag1 = getFlag(code->op1->kind);
				genSW(2,name1,funclist,flag1);	//$v0是2号寄存器
				break;
			}
		case OP_WRI:
			{
				//把要写的数写入$a0，即$4
				name1 = code->op1->name;
				int flag1 = getFlag(code->op1->kind);
				if(flag1==3)
				{
					sprintf(mipscode,"li $4,%d",atoi(name1+1));
					appendMipsCode(mipscode);
				}
				else
				{
					genLW(4,name1,funclist,flag1);
				}
				//压栈保存$ra
				sprintf(mipscode,"addi $sp, $sp, -4");
				appendMipsCode(mipscode);
				sprintf(mipscode,"sw $ra, 0($sp)");
				appendMipsCode(mipscode);
				//跳转到write
				sprintf(mipscode,"jal write");
				appendMipsCode(mipscode);
				//恢复$sp
				sprintf(mipscode,"addi $sp,$sp,4");
				appendMipsCode(mipscode);
				//恢复$ra
				sprintf(mipscode,"lw $ra, -4($sp)");
				appendMipsCode(mipscode);
				break;
			}
		default:
			{
				dbg_constprt("程序内部错误(%s,%d)：未知的中间代码类型%d\r\n",__FILE__,__LINE__,code->op);
			}
	}
	return;
}
int getFlag(int opdkind)
{
	/* 函数功能：
	 * 	翻译opd类型，供genLW和genSW函数使用
	 * 参数：
	 * 	opdkind：opd->kind
	 * 返回值：
	 * 	翻译好的flag
	 */
	int flag = -1;
	switch(opdkind)
	{
		case OPD_VAR:	flag = 0;break;
		case OPD_GCN:	flag = 1;break;
		case OPD_GAD:	flag = 2;break;
		case OPD_CON:	flag = 3;break;
		default:	dbg_constprt("程序内部错误(%s,%d)：意料外的操作数类型%d\r\n",__FILE__,__LINE__,opdkind);
	}
	return flag;
}
void genMipsHead()
{
	/* 函数功能：
	 * 	生程.text、write函数、read函数等公共文件首部分
	 */
	appendMipsCode(".data");
	appendMipsCode("_prompt: .asciiz \"Enter an integer:\"");
	appendMipsCode("_ret: .asciiz \"\\n\"");
	appendMipsCode(".globl main");
	appendMipsCode(".text");
	appendMipsCode("read:");
	appendMipsCode("li $v0, 4");
	appendMipsCode("la $a0, _prompt");
	appendMipsCode("syscall");
	appendMipsCode("li $v0, 5");
	appendMipsCode("syscall");
	appendMipsCode("jr $ra");
	appendMipsCode("write:");
	appendMipsCode("li $v0, 1");
	appendMipsCode("syscall");
	appendMipsCode("li $v0, 4");
	appendMipsCode("la $a0, _ret");
	appendMipsCode("syscall");
	appendMipsCode("move $v0, $0");
	appendMipsCode("jr $ra");
	return;
}
void genMips(struct CodeSegment seg)
{
	/* 函数功能：
	 * 	生产MIPS代码全文（不输出）
	 * 参数：
	 * 	seg：中间代码段
	 */
	prepareFuncLocalVarList(seg);
	genMipsHead();
	struct Code* cur_code = seg.head;
	while(cur_code!=NULL)
	{
		mipsCode(cur_code);
		if(cur_code==seg.tail)
			break;
		cur_code = cur_code->next;
	}
	return;
}
void outputMips(FILE* f)
{
	/* 函数功能：
	 * 	输出MIPS代码到文件
	 * 参数：
	 * 	f：输出文件附
	 */
	struct MIPSCode* cur_mcode = global_mipscode_head;
	while(cur_mcode!=NULL)
	{
		fprintf(f,"%s\n",cur_mcode->mcode);
		if(cur_mcode==global_mipscode_tail)
			break;
		cur_mcode = cur_mcode->next;
	}
	return;
}

/* 调试函数 */
void dbg_printFunclvList(struct FuncLocalVar* funclvlist)
{
	/* 函数功能：
	 * 	输出一个函数静态活动记录中参数链表/局部变量链表的信息
	 * 参数：
	 * 	funclvlist：要输出的链表
	 */
	struct FuncLocalVar* p = funclvlist;
	while(p!=NULL)
	{
		dbg_constprt("%s\t%d\r\n",p->varname,p->offset);
		p = p->next;
	}
	return;
}
void dbg_printFuncList()
{
	/* 函数功能：
	 * 	输出所有函数活动记录的信息
	 */
	dbg_constprt("===========================================\r\n");
	dbg_constprt("开始输出函数静态活动记录\r\n");
	dbg_constprt("===========================================\r\n");
	struct FuncLocalvarList* p = global_funclvlist;
	while(p!=NULL)
	{
		dbg_constprt("函数%s：\r\n",p->funcname);
		dbg_constprt("形参偏移 %d\t局部变量偏移 %d\r\n",p->localarg_cnt,p->localvar_cnt);
		dbg_constprt("形参列表：\r\n");
		dbg_printFunclvList(p->arglist);
		dbg_constprt("局部变量列表：\r\n");
		dbg_printFunclvList(p->varlist);
		dbg_constprt("===========================================\r\n");
		p = p->next;
	}
	dbg_constprt("函数静态活动记录输出结束\r\n");
	dbg_constprt("===========================================\r\n");

}
