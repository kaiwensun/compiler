#include"translate.h"
#define dbg_constprt printf
/* 全局变量 */
/* 已生成的全部代码 */
struct CodeSegment global_code_segment;
/* 输出中间代码的目标文件 */
FILE* file;
/* extern 类型表 */
extern struct Type* type_stack_top;
/* extern 变量和函数表 */
extern struct Prop* prop_stack_top;
/* 新label名的统一前缀 */
char str_LABEL_NAME[64] =  "label";
/* 新tmp变量名的统一前缀 */
char str_TMP_NAME[64] =  "tmp";

/* 中间代码结构函数 */
struct CodeSegment linkCodeSegment(struct CodeSegment seg1,struct CodeSegment seg2)
{
	/* 函数功能：
	 * 	将两块中间代码连接起来
	 * 参数：
	 * 	要被连接的两个中间代码段
	 * 返回值：
	 * 	被连接后的完整的中间代码段
	 */
	if(seg1.head==NULL && seg1.tail==NULL)
		return seg2;
	if(seg2.head==NULL && seg2.tail==NULL)
		return seg1;
	if(seg1.head==NULL || seg1.tail==NULL || seg2.head==NULL || seg2.tail==NULL)
	{
		dbg_constprt("程序内部错误（%s,%d）：被连接的代码段不合法！\r\n",__FILE__,__LINE__);
		exit(-1);
	}
	struct CodeSegment seg_rtn;
	seg_rtn.head = seg1.head;	//构造大的CodeSegment
	seg_rtn.tail = seg2.tail;
	if(seg1.tail->next!=NULL || seg1.head->prev!=NULL || seg2.head->prev!=NULL || seg2.tail->next!=NULL)
	{
		dbg_constprt("程序内部警告（%s,%d）：代码段被从内部断裂\r\n",__FILE__,__LINE__);
	}
	seg1.tail->next = seg2.head;	//把Code链表从中间连起来
	seg2.head->prev = seg1.tail;
	return seg_rtn;	
}
struct CodeSegment linkTwoCode(struct Code* code1,struct Code* code2)
{
	/* 函数功能：
	 * 	将两行中间代码连接起来
	 * 参数：
	 * 	要被连接的两个中间代码行
	 * 返回值：
	 * 	被连接后的完整的中间代码段
	 */
	struct CodeSegment seg_rtn;
	seg_rtn.head = NULL;
	seg_rtn.tail = NULL;
	if(code1==NULL||code2==NULL)
	{
		dbg_constprt("程序内部错误(%s,%d)：无法连接空代码行\r\n",__FILE__,__LINE__);
		return seg_rtn;
	}
	if(code1==code2)
	{
		seg_rtn.head = code1;
		seg_rtn.tail = code1;
		return seg_rtn;
	}
	code1->next = code2;
	code2->prev = code1;
	seg_rtn.head = code1;
	seg_rtn.tail = code2;
	return seg_rtn;
}
void prtOperand(struct Operand* operand)
{
	switch(operand->kind)
	{
		case OPD_LAB:	//label
			{
				break;
			}
		case OPD_FUN:	//function
			{
				break;
			}
		case OPD_VAR:	//variable
			{
				break;
			}
		case OPD_CON:	//constant
			{
				break;
			}
		case OPD_REL:	//relation token
			{
				break;
			}
		case OPD_GCN:	//get content (*)
			{
				fprintf(file,"*");
				break;
			}
		case OPD_GAD:	//get address (&)
			{
				fprintf(file,"&");
				break;
			}
		default:
			{
				dbg_constprt("程序内部警告(%s,%d)：%s的意料外的操作数类型%d\r\n",__FILE__,__LINE__,operand->name,operand->kind);
			}
	}
	fprintf(file,"%s",operand->name);
}
void outputCode(const struct Code* code)
{
	/* 函数功能：
	 * 	打印一条代码到文件
	 */
	if(code==NULL)
	{
		dbg_constprt("程序内部错误（%s,%d）\r\n",__FILE__,__LINE__);
		return;
	}
	switch(code->op)
	{
		case OP_LAB:
			{
				//fprintf(file,"LABEL %s :\r\n",code->op1->name);
				fprintf(file,"LABEL ");
				prtOperand(code->op1);
				fprintf(file," :");
				break;
			}
		case OP_FUN:
			{
				//fprintf(file,"FUNCTION %s :\r\n",code->op1->name);
				fprintf(file,"FUNCTION ");
				prtOperand(code->op1);
				fprintf(file," :");
				break;
			}
		case OP_ASS:
			{
				//fprintf(file,"%s := %s\r\n",code->op1->name,code->op2->name);
				prtOperand(code->op1);
				fprintf(file," := ");
				prtOperand(code->op2);
				break;
			}
		case OP_ADD:
			{
				//fprintf(file,"%s := %s + %s\r\n",code->op1->name,code->op2->name,code->op3->name);
				prtOperand(code->op1);
				fprintf(file," := ");
				prtOperand(code->op2);
				fprintf(file," + ");
				prtOperand(code->op3);
				break;
			}
		case OP_SUB:
			{
				//fprintf(file,"%s := %s - %s\r\n",code->op1->name,code->op2->name,code->op3->name);
				prtOperand(code->op1);
				fprintf(file," := ");
				prtOperand(code->op2);
				fprintf(file," - ");
				prtOperand(code->op3);
				break;
			}
		case OP_MUL:
			{
				//fprintf(file,"%s := %s * %s\r\n",code->op1->name,code->op2->name,code->op3->name);
				prtOperand(code->op1);
				fprintf(file," := ");
				prtOperand(code->op2);
				fprintf(file," * ");
				prtOperand(code->op3);
				break;
			}
		case OP_DIV:
			{
				//fprintf(file,"%s := %s / %s\r\n",code->op1->name,code->op2->name,code->op3->name);
				prtOperand(code->op1);
				fprintf(file," := ");
				prtOperand(code->op2);
				fprintf(file," / ");
				prtOperand(code->op3);
				break;
			}
		case OP_GTO:
			{
				//fprintf(file,"GOTO %s\r\n",code->op1->name);
				fprintf(file,"GOTO ");
				prtOperand(code->op1);
				break;
			}
		case OP_IFJ:
			{
				//fprintf(file,"IF %s %s %s GOTO %s\r\n",code->op1->name,code->op2->name,code->op3->name,code->op4->name);
				fprintf(file,"IF ");
				prtOperand(code->op1);
				fprintf(file," ");
				prtOperand(code->op2);
				fprintf(file," ");
				prtOperand(code->op3);
				fprintf(file," GOTO ");
				prtOperand(code->op4);
				break;
			}
		case OP_RET:
			{
				//fprintf(file,"RETURN %s\r\n",code->op1->name);
				fprintf(file,"RETURN ");
				prtOperand(code->op1);
				break;
			}
		case OP_DEC:
			{
				//fprintf(file,"DEC %s %s\r\n",code->op1->name,code->op2->name);
				fprintf(file,"DEC ");
				prtOperand(code->op1);
				fprintf(file," ");
				prtOperand(code->op2);
				break;
			}
		case OP_ARG:
			{
				//fprintf(file,"ARG %s\r\n",code->op1->name);
				fprintf(file,"ARG ");
				prtOperand(code->op1);
				break;
			}
		case OP_CAL:
			{
				//fprintf(file,"%s := CALL %s\r\n",code->op1->name,code->op2->name);
				prtOperand(code->op1);
				fprintf(file," := CALL ");
				prtOperand(code->op2);
				break;
			}
		case OP_PAR:
			{
				//fprintf(file,"PARAM %s\r\n",code->op1->name);
				fprintf(file,"PARAM ");
				prtOperand(code->op1);
				break;
			}
		case OP_RED:
			{
				//fprintf(file,"READ %s\r\n",code->op1->name);
				fprintf(file,"READ ");
				prtOperand(code->op1);
				break;
			}
		case OP_WRI:
			{
				//fprintf(file,"WRITE %s\r\n",code->op1->name);
				fprintf(file,"WRITE ");
				prtOperand(code->op1);
				break;
			}
		default:
			{
				dbg_constprt("程序内部错误：未知的code->op类型%d(%s,%d)\r\n",code->op,__FILE__,__LINE__);
				break;
			}
	}
	fprintf(file,"\r\n");
	return;
}
unsigned int outputCodeSegment(const struct CodeSegment seg_code)
{
	/*
	 * 函数功能：
	 * 	输出代码段到文件
	 * 参数：
	 * 	要被输出的代码段
	 * 返回值：
	 * 	输出的代码总条数
	 * 其他说明：
	 * 	输出文件的文件表示符为全局变量file
	 */
	unsigned int code_cnt = 0;
	struct Code* code = seg_code.head;
	while(code!=seg_code.tail)
	{
		outputCode(code);
		code_cnt++;
		code = code->next;
	}
	if(seg_code.tail != NULL)
	{
		outputCode(seg_code.tail);
		code_cnt++;
	}
	return code_cnt;
}
int freeOperand(struct Operand* operand)
{
	/* 函数功能：
	 * 	释放Operand空间
	 * 参数：
	 * 	要被释放的Operand
	 * 返回值：
	 * 	正常情况下均返回0
	 */
	if(operand==NULL)
	{
		dbg_constprt("程序内部错误(%s,%d)\r\n",__FILE__,__LINE__);
		exit(-1);
	}
	free(operand);
	operand=NULL;
	return 0;
}
int freeCode(struct Code* code)
{
	/* 函数功能：
	 * 	释放一行Code的空间
	 * 参数：
	 * 	要被释放的Operand
	 * 返回值：
	 * 	正常情况下均返回0
	 * 其他说明：
	 * 	一并释放code内operand的空间
	 */
	if(code==NULL)
		return 0;
	struct Operand* operand = NULL;
	operand = code->op1;
	if(operand!=NULL) freeOperand(operand);
	operand = code->op2;
	if(operand!=NULL) freeOperand(operand);
	operand = code->op3;
	if(operand!=NULL) freeOperand(operand);
	operand = code->op4;
	if(operand!=NULL) freeOperand(operand);
	free(code);
	code==NULL;
	return 0;
	
}
struct Operand* newOperand()
{
	/*
	 * 函数功能：
	 * 	为Code申请一个新的Operand空间，并初始化
	 * 返回值：
	 * 	指向申请好的空间的指针
	 */
	struct Operand* newoperand = (struct Operand*)malloc(sizeof(struct Operand));
	newoperand->kind = -1;
	strcpy(newoperand->name,"");
	return newoperand;
}
struct Code* newCode(int operand_num)
{
	/*
	 * 函数功能：
	 * 	申请一行Code的空间，并初始化
	 * 参数：
	 * 	Code的operand的个数（1~4）
	 * 返回值：
	 * 	指向申请好的空间的指针
	 */
	struct Code* newcode = (struct Code*)malloc(sizeof(struct Code));
	while(newcode->op>=OP_LAB && newcode->op<=OP_WRI)
	{
		//to solve an awkward bug.
		newcode = (struct Code*)malloc(sizeof(struct Code));
	}
	newcode->op = -1;
	newcode->op1 = newcode->op2 = newcode->op3 = newcode->op4 = NULL;
	switch(operand_num)
	{
		case 4:newcode->op4 = newOperand();
		case 3:newcode->op3 = newOperand();
		case 2:newcode->op2 = newOperand();
		case 1:newcode->op1 = newOperand();
		       break;
		default:
			dbg_constprt("程序内部错误(%s,%d)\r\n",__FILE__,__LINE__);
			break;
	}
	newcode->prev = NULL;
	newcode->next = NULL;
	return newcode;	
}

/* 其它函数 */
void initTranslate(FILE* fp)
{
	/* 函数功能：
	 * 	对Lab3中的各种操作进行初始化，包括：
	 * 	- 初始化输出文件指针FILE* file
	 * 	- 在符号表中加入read和write函数
	 * 	- 初始化全局代码段
	 * 参数：
	 * 	输出文件指针
	 */
	//初始化输出文件指针
	file = fp;
	//添加read函数
	struct Prop* prop_read = (struct Prop*)malloc(sizeof(struct Prop));
	prop_read->kind = 1;
	prop_read->un.funprop = (struct FunProp*)malloc(sizeof(struct FunProp));
	prop_read->un.funprop->f_rtn_type = getAnIntType();
	prop_read->un.funprop->paracnt = 0;
	prop_read->un.funprop->paraprop = NULL;
	prop_read->un.funprop->declared = 0;
	prop_read->un.funprop->defined = 0;
	strcpy(prop_read->name,"read");
	prop_read->layer = 0;
	prop_read->next = NULL;
	push_prop_stack(prop_read);
	//添加write函数
	struct Prop* prop_write = (struct Prop*)malloc(sizeof(struct Prop));
	prop_write->kind = 1;
	prop_write->un.funprop = (struct FunProp*)malloc(sizeof(struct FunProp));
	prop_write->un.funprop->f_rtn_type = getAnIntType();
	prop_write->un.funprop->paracnt = 1;
	struct Prop* para1 = (struct Prop*)malloc(sizeof(struct Prop));
	para1->kind = 0;
	para1->un.varprop = (struct VarProp*)malloc(sizeof(struct VarProp));
	para1->un.varprop->type = getAnIntType();
	para1->un.varprop->def_lineno = 0;
	strcpy(para1->name,"write_argss");
	para1->layer = 0;
	para1->next = NULL;
	prop_write->un.funprop->paraprop = para1;
	prop_write->un.funprop->declared = 0;
	prop_write->un.funprop->defined = 0;
	strcpy(prop_write->name,"write");
	prop_write->layer = 0;
	prop_write->next = NULL;
	push_prop_stack(prop_write);
	//初始化global_code_segment
	global_code_segment.head=NULL;
	global_code_segment.tail=NULL;
}
char* newTmpName()
{
	/* 函数功能：
	 * 	生成一个全局唯一的临时变量名^tmp%u
	 * 返回值：
	 * 	生成的新的临时变量名
	 * 其他说明：
	 * 	用static实现，没有申请新空间。
	 * 	调用者获得新名字后，必须及时strcpy！！！
	 */
	static unsigned int new_tmp_var_counter;
	static char new_tmp_name[256]="";
	sprintf(new_tmp_name,"%s%u",str_TMP_NAME,new_tmp_var_counter);
	new_tmp_var_counter++;
	return new_tmp_name;
}
char* newLabelName()
{
	/* 函数功能：
	 * 	生成一个全局唯一的临时Label名^label%u
	 * 返回值：
	 * 	生成的新的临时label名
	 * 其他说明：
	 * 	用static实现，没有申请新空间。
	 * 	调用者获得新名字后，必须及时strcpy！！！
	 */
	static unsigned int new_label_counter;
	static char new_label_name[256]="";
	sprintf(new_label_name,"%s%u",str_LABEL_NAME,new_label_counter);
	new_label_counter++;
	return new_label_name;
}
void generateIrCode(struct Node* root)
{
	/* 函数功能：
	 * 	驱动整个中间代码生成过程，遍历整个语法树
	 */
	global_code_segment = translate_node_Program(root);
}
struct CodeSegment translate_allocateSpaceForArrayOrStruct(struct Prop* varprop)
{
	/* 函数功能：
	 * 	生产为数组或结构体分配空间的中间代码
	 * 参数：
	 * 	数组变量再符号表中的拷贝
	 * 返回值：
	 * 	生产的申请空间的DEC指令代码段
	 */
	struct CodeSegment seg_rtn;
	struct Code* newcode = newCode(2);
	newcode->op = OP_DEC;
	newcode->op1->kind = OPD_VAR;
	strcpy(newcode->op1->name,varprop->name);
	newcode->op2->kind = OPD_CON;
	unsigned int size = calculateTypeTotalSize(varprop->un.varprop->type);
	sprintf(newcode->op2->name,"%u",size*4);	//Lab3中，DEC空间大小为4n，前缀无井号	
	seg_rtn.head = newcode;
	seg_rtn.tail = newcode;
	return seg_rtn;
}
unsigned int calculateTypeTotalSize(struct Type* type)
{
	/* 函数功能：
	 * 	计算基本类型、数组类型的或结构体类型的所占用空间的大小（单位：4字节，不是字节）
	 * 参数：
	 * 	要被计算大小的数据类型Type
	 * 返回值：
	 * 	数据类型的大小（单位：4字节）
	 * 其他说明：
	 * 	如果要转换为字节个数，需要再返回值结果上乘以4！
	 */
	unsigned int size=0;
	switch(type->kind)
	{
		case 0:
			{
				size = 1;
				break;
			}
		case 1:
			{
				unsigned int len = (unsigned int)type->un.array.arrsize;
				size = len * calculateTypeTotalSize(type->un.array.arreletype);
				break;
			}
		case 2:
			{
				struct FieldList* fld_lst = type->un.structure.struct_field;
				while(fld_lst!=NULL)
				{
					size += calculateTypeTotalSize(fld_lst->type);
					fld_lst = fld_lst->next;
				}
				break;				
			}
		default:
			{
				dbg_constprt("程序内部错误(%s,%d)\r\n",__FILE__,__LINE__);
			}
	}
	return size;
}
unsigned int freeArgList(struct ArgList* arg_list)
{
	/* 函数功能：
	 * 	释放ArgList链表空间
	 * 返回值：
	 * 	ArgList中被释放的节点的总个数
	 */
	unsigned int cnt = 0;
	struct ArgList* curr=arg_list;
	struct ArgList* next=NULL;
	while(curr!=NULL)
	{
		next = curr->next;
		free(curr);
		curr = next;
		cnt++;
	}
	return cnt;
}
struct CodeSegment calculateArrOrStructOffset(struct Node* node_Exp,struct Type** calleeType,const char* place)
{
	/* 函数功能：
	 * 	计算数组变量的偏移地址，并给出计算偏移地址的中间代码
	 * 参数：
	 * 	node_Exp：语法树节点
	 * 	calleeType：作为返回值，告诉上层调用者它（被调用者）的数据类型（以计算单位偏移量）
	 * 	place：记录偏移地址的临时变量名
	 * 返回值：
	 * 	计算偏移地址的中间代码
	 * 其他说明：
	 * 	通过参数指针返回的Type* calleeType是用copyType复制的，上层调用者可随意释放
	 */
	struct CodeSegment seg_rtn;
	seg_rtn.head = NULL;
	seg_rtn.tail = NULL;
	switch(node_Exp->info.ruleId)
	{
		case 56:	/* Exp -> Exp LB Exp RB */
			{
				//计算本层的基地址
				char c_tmp1[16] = "";
			       	strcpy(c_tmp1,newTmpName());
				struct Type* mytype = NULL;
				struct CodeSegment seg_code1 = calculateArrOrStructOffset(node_Exp->child[0],&mytype,c_tmp1);
				if(mytype==NULL)
				{
					dbg_constprt("程序内部错误(%s,%d)：数组层间嵌套的类型分析错误\r\n",__FILE__,__LINE__);
					break;
				}
				//计算本层单位变量的空间大小（单位：4字节）
				unsigned int type_size = calculateTypeTotalSize(mytype->un.array.arreletype);
				//计算本层单位类型的个数
				char c_tmp2[16] = "";		//c_tmp2将记录本层的单位类型的个数
				strcpy(c_tmp2,newTmpName());
				int opd_tmp2 = -1;
				struct CodeSegment seg_code2 = translate_node_Exp(node_Exp->child[2],c_tmp2,&opd_tmp2);
				//生产计算本层的偏移量的代码
				char c_tmp3[16] = "";		//c_tmp3将记录本层的偏移量
				strcpy(c_tmp3,newTmpName());
				struct Code* code3 = newCode(3);
				code3->op = OP_MUL;
				code3->op1->kind = OPD_VAR;
				strcpy(code3->op1->name,c_tmp3);
				code3->op2->kind = opd_tmp2;
				strcpy(code3->op2->name,c_tmp2);
				code3->op3->kind = OPD_CON;
				sprintf(code3->op3->name,"#%u",type_size*4);	//单位由4字节转换为1字节
				struct CodeSegment seg_code3 = linkTwoCode(code3,code3);
				//计算目前可定位的总的偏移地址，赋值到place
				struct Code* code4 = newCode(3);
				code4->op = OP_ADD;
				code4->op1->kind = OPD_VAR;
				strcpy(code4->op1->name,place);
				code4->op2->kind = OPD_VAR;
				strcpy(code4->op2->name,c_tmp1);
				code4->op3->kind = OPD_VAR;
				strcpy(code4->op3->name,c_tmp3);
				struct CodeSegment seg_code4 = linkTwoCode(code4,code4);
				//生成作为返回值的代码段
				seg_rtn = linkCodeSegment(seg_code1,seg_code2);
				seg_rtn = linkCodeSegment(seg_rtn,seg_code3);
				seg_rtn = linkCodeSegment(seg_rtn,seg_code4);
				//构造函数调用者所需的calleeType
				*calleeType = copyType(mytype->un.array.arreletype);
//				freeType(mytype);	//enable this line may cause awkward and mysterious bug!!!
				mytype = NULL;
				break;
			}
		case 57:	/* Exp -> Exp DOT ID */
			{
				//计算结构体中DOT左侧的基地址
				char c_tmp1[16] = "";		//c_tmp1将记录DOT左侧的基地址
			       	strcpy(c_tmp1,newTmpName());
				struct Type* mytype = NULL;
				struct CodeSegment seg_code1 = calculateArrOrStructOffset(node_Exp->child[0],&mytype,c_tmp1);
				//计算结构体DOT右侧的变量在数据结构基地址中的偏移地址（常量）
				unsigned offset = 0;		//将记录域ID的地址偏移量(单位4字节)
				if(mytype==NULL || mytype->kind!=2)
				{
					dbg_constprt("程序内部错误(%s,%d)：结构体嵌套的类型分析错误\r\n",__FILE__,__LINE__);
					break;
				}
				char* name = node_Exp->child[2]->info.notation;	//ID域的域名
				struct FieldList* p_fldlst = mytype->un.structure.struct_field;
				while(p_fldlst!=NULL)
				{
					if(strcmp(name,p_fldlst->name)==0)
					{
						break;
					}
					offset += calculateTypeTotalSize(p_fldlst->type);
					p_fldlst = p_fldlst->next;
				}
				if(p_fldlst==NULL)
				{
					dbg_constprt("程序内部错误(%s,%d)：计算域%s的偏移地址时没有在结构体%s中找到此域名\r\n",__FILE__,__LINE__,mytype->un.structure.struct_name,name);
					break;
				}
				//生成总的偏移地址的代码，赋值到place
				struct Code* code2 = newCode(3);
				code2->op = OP_ADD;
				code2->op1->kind = OPD_VAR;
				strcpy(code2->op1->name,place);
				code2->op2->kind = OPD_VAR;
				strcpy(code2->op2->name,c_tmp1);
				code2->op3->kind = OPD_CON;
				sprintf(code2->op3->name,"#%u",offset*4);	//单位由4字节转换为1字节
				struct CodeSegment seg_code2 = linkTwoCode(code2,code2);
				//生产作为返回值的代码段
				seg_rtn = linkCodeSegment(seg_code1,seg_code2);
				//构造函数调用者所需的calleeType
				*calleeType = copyType(p_fldlst->type);
//				freeType(mytype);	//enable this line may cause awkward and mysterious bug!!!
				mytype = NULL;
				break;
			}
		case 58:	/* Exp -> ID */
			{
				//获得ID的name
				char* name = node_Exp->child[0]->info.notation;
				//构造取ID的地址的代码，赋值给place
				struct Code* code1 = newCode(2);
				code1->op = OP_ASS;
				code1->op1->kind = OPD_VAR;
				strcpy(code1->op1->name,place);
				if(idnameIsAParameter(name))
				{
					code1->op2->kind = OPD_VAR;	//如果op2是形参，那么传进来的不是变量而是变量地址，就无需再Gat ADdress了
				}
				else
				{
					code1->op2->kind = OPD_GAD;
				}
				strcpy(code1->op2->name,name);
				//生产作为返回值的代码段
				seg_rtn = linkTwoCode(code1,code1);
				//构造函数调用者所需的calleeType
				struct Prop* varprop = searchPropStackByPropName(name,0,1,0);
				if(varprop==NULL)
				{
					dbg_constprt("程序内部错误(%s,%d)：构建变量%s的基地址时找不到该变量\r\n",__FILE__,__LINE__,name);
					break;
				}
				*calleeType = copyType(varprop->un.varprop->type);
			struct Type* dbgtype = varprop->un.varprop->type;
				break;
			}
		default:
			{
				dbg_constprt("程序内部错误(%s,%d)\r\n",__FILE__,__LINE__);
			}
	}
	return seg_rtn;
}
int whichKindOfExp(const struct Node* node_Exp,char* idname)
{
	/*
	 * 函数功能：
	 * 	判断node_Exp是由ID/数组/还是结构体/其他生成的
	 * 参数：
	 * 	node_Exp：要被判断的Exp节点
	 * 	idname：用于返回值，如果node_Exp生成的是ID，则用strcpy告诉调用者ID的名字。否则不予处理。
	 *
	 * 返回值：
	 * 	1：ID
	 * 	2：其他
	 * 	3：数组
	 * 	4：结构体
	 * 	-1：本函数出错
	 */
	if(node_Exp==NULL)
	{
		dbg_constprt("程序内部错误(%s,%d)：传入了空的Exp节点\r\n",__FILE__,__LINE__);
		return -1;
	}
	if(node_Exp->info.ruleId<43 || node_Exp->info.ruleId>60)
	{
		dbg_constprt("程序内部错误(%s,%d)：ruleId=%d不是Exp节点\r\n",__FILE__,__LINE__,node_Exp->info.ruleId);
		return -1;
	}
	switch(node_Exp->info.ruleId)
	{
		case 56:
			return 3;
		case 57:
			return 4;
		case 58:
			{
				strcpy(idname,node_Exp->child[0]->info.notation);
				return 1;
			}
		case 51:
			return whichKindOfExp(node_Exp->child[1],idname);
		default:
			return 2;
	}
}
int idnameIsAParameter(char* name)
{
	/*
	 * 函数功能：
	 * 	判断符号表中的char* name是不是函数的形参列表中的形参
	 * 参数：
	 * 	要被判断的名字
	 * 返回值：
	 * 	1：是形参
	 * 	0：函数形参列表中没有匹配名称的形参
	 * 其他说明：
	 * 	只查询符号表中的函数变量的形参列表，不查询符号表中的变量符号
	 */
	extern struct Prop* prop_stack_top;
	struct Prop* p_prop = prop_stack_top;
	while(p_prop!=NULL)
	{
		if(p_prop->kind==0)
		{
			//如果当前节点是变量符号，则忽略
			p_prop = p_prop->next;
			continue;
		}
		else if(p_prop->kind==1)
		{
			//如果当前节点是函数符号，则处理
			struct Prop* arg_prop_list = p_prop->un.funprop->paraprop;
			while(arg_prop_list!=NULL)
			{
				if(strcmp(name,arg_prop_list->name)==0)
					return 1;
				arg_prop_list = arg_prop_list->next;
			}
			p_prop = p_prop->next;
			continue;
		}
		else
		{
			dbg_constprt("程序内部警告(%s,%d)：符号表中未知的符号类型%d\r\n",__FILE__,__LINE__,p_prop->kind);
			p_prop = p_prop->next;
		}
	}
	return 0;

	
}
void avoidTmpAndLabelNameConflict()
{
	/* 函数功能：
	 * 	扫描符号表，避免label、tmp与符号表中的变量重名
	 */
	struct Prop* prop = NULL;
	//检查label
	prop = prop_stack_top;
	int llen = strlen(str_LABEL_NAME);
	while(prop!=0)
	{
		if(strncmp(str_LABEL_NAME,prop->name,llen)==0)
		{
			str_LABEL_NAME[llen]='l';
			str_LABEL_NAME[llen+1]='\0';
			llen++;
			prop = prop_stack_top;
			continue;
		}
		prop = prop->next;
	}
	prop = prop_stack_top;
	int tlen = strlen(str_TMP_NAME);
	while(prop!=0)
	{
		if(strncmp(str_TMP_NAME,prop->name,tlen)==0)
		{
			str_TMP_NAME[tlen]='p';
			str_TMP_NAME[tlen+1]='\0';
			tlen++;
			prop = prop_stack_top;
			continue;
		}
		prop = prop->next;
	}
}

/* 节点翻译函数 */
struct CodeSegment translate_node_Program(struct Node* node_Program)
{
	
	/* Program	:	ExtDefList (1)
	 */
	struct CodeSegment seg_rtn;
	seg_rtn.head = NULL;
	seg_rtn.tail = NULL;
	switch(node_Program->info.ruleId)
	{
		case 1:
			{
				seg_rtn = translate_node_ExtDefList(node_Program->child[0]);
				break;
			}
		default:
			{
				dbg_constprt("程序内部错误（%s,%d）\r\n",__FILE__,__LINE__);
			}
	}
	return seg_rtn;
}
struct CodeSegment translate_node_ExtDefList(struct Node* node_ExtDefList)
{
	/*
	 * ExtDefList	:	ExtDef ExtDefList (2)
	 *		|	epsilon (3)
	 */
	struct CodeSegment seg_rtn;
	seg_rtn.head = NULL;
	seg_rtn.tail = NULL;
	switch(node_ExtDefList->info.ruleId)
	{
		case 2:
			{
				struct CodeSegment seg1 = translate_node_ExtDef(node_ExtDefList->child[0]);
				struct CodeSegment seg2 = translate_node_ExtDefList(node_ExtDefList->child[1]);
				seg_rtn = linkCodeSegment(seg1,seg2);
				break;
			}
		case 3:
			{
				seg_rtn.head = NULL;
				seg_rtn.tail = NULL;
				break;
			}
		default:
			{
				dbg_constprt("程序内部错误（%s,%d）\r\n",__FILE__,__LINE__);
			}
	}
	return seg_rtn;

}
struct CodeSegment translate_node_ExtDef(struct Node* node_ExtDef)
{
	/*
	 * ExtDef	:	Specifier ExtDecList SEMI (4)
	 *		|	Specifier SEMI (5)
	 * 		|	Specifier FunDec CompSt (6)
	 */
	struct CodeSegment seg_rtn;
	seg_rtn.head = NULL;
	seg_rtn.tail = NULL;
	switch(node_ExtDef->info.ruleId)
	{
		case 4:
			{
				/*
				 * Lab3的假设4说，可以假设不存在全局变量
				 * 故不理会全局声明
				 */
				seg_rtn.head = NULL;
				seg_rtn.tail = NULL;
				break;
			}
		case 5:
			{
				/* 翻译中间代码，无需处理结构体定义 */
				seg_rtn.head = NULL;
				seg_rtn.tail = NULL;
				break;
			}
		case 6:
			{
				//这里翻译Specifier似乎没什么用处
				struct Type* t_Specifier = translate_node_Specifier(node_ExtDef->child[0]);
				if(typeIsBasic(t_Specifier,0)==0)
				{
					dbg_constprt("程序内部警告：似乎出现了非INT型返回值\r\n");
				}
//				freeType(t_Specifier);	//enable this line may cause awkward and mysterious bug!!!
				t_Specifier = NULL;
				struct CodeSegment seg_FunDec = translate_node_FunDec(node_ExtDef->child[1]);
				struct CodeSegment seg_CompSt = translate_node_CompSt(node_ExtDef->child[2]);
				seg_rtn = linkCodeSegment(seg_FunDec,seg_CompSt);
				break;
			}
		default:
			{
				dbg_constprt("程序内部错误(%s,%d)\r\n",__FILE__,__LINE__);
			}
	}
	return seg_rtn;
}
struct Type* translate_node_Specifier(struct Node* node_Specifier)
{
	/*
	 * Specifier	:	TYPE
	 *		|	StructSpecifier	
	*/
	struct Type* t_rtn = NULL;
	switch(node_Specifier->info.ruleId)
	{
		case 11:
			{
				t_rtn = translate_node_TYPE(node_Specifier->child[0]);
				break;
			}
		case 12:
			{
				t_rtn = translate_node_StructSpecifier(node_Specifier->child[0]);
				break;
			}
		default:
			{
				dbg_constprt("程序内部错误(%s,%d)\r\n",__FILE__,__LINE__);
			}
	}
	return t_rtn;
}
struct Type* translate_node_TYPE(struct Node* node_TYPE)
{
	struct Type* t_rtn = getAnIntType();
	t_rtn->un.basic = (node_TYPE->info.value.INT==1)?0:1;
	return t_rtn;
}
struct Type* translate_node_StructSpecifier(struct Node* node_StructSpecifier)
{
	/*
	 * StructSpecifier	:	STRUCT OptTag LC DefList RC (13)
	 *			|	STRUCT Tag (14)
	 */
	struct Type* t_rtn = NULL;
	switch(node_StructSpecifier->info.ruleId)
	{
		case 13:
			{
				t_rtn = translate_node_OptTag(node_StructSpecifier->child[1]);
				break;
			}
		case 14:
			{
				t_rtn = translate_node_Tag(node_StructSpecifier->child[1]);
				break;
			}
		default:
			{
				dbg_constprt("程序内部错误(%s,%d)\r\n",__FILE__,__LINE__);
			}
	}
	return t_rtn;
}
struct Type* translate_node_OptTag(struct Node* node_OptTag)
{
	/*
	 * OptTag	:	ID (15)
	 * 		|	epsilon （16)
	 */
	struct Type* t_rtn = NULL;
	switch(node_OptTag->info.ruleId)
	{
		case 15:
			{
				struct Node* node_ID = node_OptTag->child[0];
				struct Type* exist_type = searchTypeStackByStructName(node_ID->info.notation,0);
				t_rtn = copyType(exist_type);
				break;
			}
		case 16:
			{
				dbg_constprt("程序内部警告(%s,%d)：似乎有匿名结构体\r\n",__FILE__,__LINE__);
				static int unnamed_struct_counter;
				char name[256]="";
				sprintf(name,"#UNNAMED%d",unnamed_struct_counter);
				unnamed_struct_counter++;
				struct Node* node_ID = node_OptTag->child[0];
				struct Type* exist_type = searchTypeStackByStructName(node_ID->info.notation,0);
				t_rtn = copyType(exist_type);
				break;
			}
		default:
			{
				dbg_constprt("程序内部错误(%s,%d)\r\n",__FILE__,__LINE__);
			}
	}
	return t_rtn;
}
struct Type* translate_node_Tag(struct Node* node_Tag)
{

	/*
	 *  Tag->ID
	 */
	struct Type* t_rtn = NULL;
	switch(node_Tag->info.ruleId)
	{
		case 17:
			{
				struct Node* node_ID = node_Tag->child[0];
				struct Type* exist_type = searchTypeStackByStructName(node_ID->info.notation,0);
				t_rtn = copyType(exist_type);
				break;
			}
		default:
			{
				dbg_constprt("程序内部错误(%s,%d)\r\n",__FILE__,__LINE__);
			}
	}
	return t_rtn;
}
struct CodeSegment translate_node_FunDec(struct Node* node_FunDec)
{
	/* 
	 * FunDec	:	ID LP VarList RP (20)
	 * 		|	ID LP RP (21)
	 */
	struct CodeSegment seg_rtn;
	seg_rtn.head = NULL;
	seg_rtn.tail = NULL;
	struct Node* node_ID = node_FunDec->child[0];
	struct Prop* fun_prop = searchPropStackByPropName(node_ID->info.notation,0,0,1);
	if(fun_prop==NULL)
	{
		dbg_constprt("程序内部错误(%s,%d)：在符号表中找不到函数%s的记录\r\n",__FILE__,__LINE__,node_ID->info.notation);
		return seg_rtn;
	}
	switch(node_FunDec->info.ruleId)
	{
		case 20:
			{
				struct CodeSegment seg1;
				struct Code* code_fun = newCode(1);
				code_fun->op = OP_FUN;
				code_fun->op1->kind = OPD_FUN;
				strcpy(code_fun->op1->name,fun_prop->name);
				seg1.head = code_fun;
				seg1.tail = code_fun;

				struct CodeSegment seg2 = translate_node_VarList(node_FunDec->child[2]);
				seg_rtn = linkCodeSegment(seg1,seg2);
				break;
			}
		case 21:
			{
				struct Code* code_fun = newCode(1);
				code_fun->op = OP_FUN;
				code_fun->op1->kind = OPD_FUN;
				strcpy(code_fun->op1->name,fun_prop->name);
				seg_rtn.head = code_fun;
				seg_rtn.tail = code_fun;
				break;
			}
		default:
			{
				dbg_constprt("程序内部错误（%s,%d）\r\n",__FILE__,__LINE__);
			}
	}
	return seg_rtn;
}
struct CodeSegment translate_node_VarList(struct Node* node_VarList)
{
	/* VarList	:	ParamDec COMMA VarList (22)
	 * 		|	ParamDec (23)
	 * 其他说明：
	 * 	采取参数压栈先压入右侧再压入左侧的策略，即printf要让字符串再栈顶的策略
	 * 	所以被调用者在取参数时，先取到的是左边的参数。
	 */
	struct CodeSegment seg_rtn;
	seg_rtn.head = NULL;
	seg_rtn.tail = NULL;
	switch(node_VarList->info.ruleId)
	{
		case 22:
			{
				struct CodeSegment seg_ParamDec,seg_VarList;
				seg_ParamDec = translate_node_ParamDec(node_VarList->child[0]);
				seg_VarList = translate_node_VarList(node_VarList->child[2]);
				//这句话决定了取参数的顺序！！！
				seg_rtn = linkCodeSegment(seg_ParamDec,seg_VarList);
				break;
			}
		case 23:
			{
				seg_rtn = translate_node_ParamDec(node_VarList->child[0]);
				break;
			}
		default:
			{
				dbg_constprt("程序内部错误（%s,%d）：被连接的代码段不合法！\r\n",__FILE__,__LINE__);
			}
	}
	return seg_rtn;	
}
struct CodeSegment translate_node_ParamDec(struct Node* node_ParamDec)
{
	/* ParamDec	:	Specifier VarDec (24)
	 */
	struct Code* newcode = newCode(1);
	struct CodeSegment seg_rtn;
	seg_rtn.head = newcode;
	seg_rtn.tail = newcode;
	switch(node_ParamDec->info.ruleId)
	{
		case 24:
			{
				struct Prop* p_VarDec = translate_node_VarDec(node_ParamDec->child[1]);
				if(p_VarDec==NULL)
					break;
				newcode->op = OP_PAR;
				newcode->op1->kind = OPD_VAR;
				strcpy(newcode->op1->name,p_VarDec->name);
				freeProp(p_VarDec);
				p_VarDec=NULL;
				break;
			}
		default:
			{
				dbg_constprt("程序内部错误(%s,%d)\r\n",__FILE__,__LINE__);
			}
	}
	return seg_rtn;

	
}
struct Prop* translate_node_VarDec(struct Node* node_VarDec)
{
	/*
	 * VarDec	:	ID (18)
	 *	 	|	VarDec LB INT RB (19)
	 * 其他说明：
	 * 	返回的Prop已经经过复制，是新的空间，不是符号表中的。
	 */
	struct Prop* p_rtn = NULL;
	switch(node_VarDec->info.ruleId)
	{
		case 18:
			{
				struct Node* node_ID = node_VarDec->child[0];
				struct Prop* p_prop = searchPropStackByPropName(node_ID->info.notation,0,1,0);
				if(p_prop==NULL)
				{
					dbg_constprt("程序内部错误(%s,%d)：在符号表中找不到变量%s\r\n",__FILE__,__LINE__,node_ID->info.notation);
					break;
				}
				p_rtn = copyVarProp(p_prop);
				break;
			}
		case 19:
			{
				p_rtn = translate_node_VarDec(node_VarDec->child[0]);
				break;
			}
		default:
			{
				dbg_constprt("程序内部错误(%s,%d)\r\n",__FILE__,__LINE__);
			}
	}
	return p_rtn;
}
struct CodeSegment translate_node_CompSt(struct Node* node_CompSt)
{
	/*
	 * CompSt	:	LC DefList StmtList RC (25)
	 */
	
	struct CodeSegment seg_rtn;
	seg_rtn.head = NULL;
	seg_rtn.tail = NULL;
	switch(node_CompSt->info.ruleId)
	{
		case 25:
			{
				struct CodeSegment seg_DefList = translate_node_DefList(node_CompSt->child[1]);
				struct CodeSegment seg_StmtList = translate_node_StmtList(node_CompSt->child[2]);
				seg_rtn = linkCodeSegment(seg_DefList,seg_StmtList);
				break;
			}
		default:
			{
				dbg_constprt("程序内部错误(%s,%d)\r\n",__FILE__,__LINE__);
			}
	}
	return seg_rtn;
}
struct CodeSegment translate_node_DefList(struct Node* node_DefList)
{
	/*
	 * DefList	:	Def DefList (34)
	 * 		|	epsilon (35)
	 */
	struct CodeSegment seg_rtn;
	seg_rtn.head = NULL;
	seg_rtn.tail = NULL;
	switch(node_DefList->info.ruleId)
	{
		case 34:
			{
				struct CodeSegment seg_Def = translate_node_Def(node_DefList->child[0]);
				struct CodeSegment seg_DefList = translate_node_DefList(node_DefList->child[1]);
				seg_rtn = linkCodeSegment(seg_Def,seg_DefList);
				break;
			}
		case 35:
			{
				seg_rtn.head = NULL;
				seg_rtn.tail = NULL;
				break;
			}
		default:
			{
				dbg_constprt("程序内部错误(%s,%d)\r\n",__FILE__,__LINE__);
			}
	}
	return seg_rtn;
}
struct CodeSegment translate_node_Def(struct Node* node_Def)
{
	/*
	 * Def	:	Specifier DecList SEMI (36)
	 */
	struct CodeSegment seg_rtn;
	seg_rtn.head = NULL;
	seg_rtn.tail = NULL;
	switch(node_Def->info.ruleId)
	{
		case 36:
			{
				seg_rtn = translate_node_DecList(node_Def->child[1]);
				break;
			}
		default:
			{
				dbg_constprt("程序内部错误(%s,%d)\r\n",__FILE__,__LINE__);
			}
	}
	return seg_rtn;

	
}
struct CodeSegment translate_node_DecList(struct Node* node_DecList)
{
	/*
	 * DecList	:	Dec (37)
	 * 		|	Dec COMMA DecList (38)
	 */
	struct CodeSegment seg_rtn;
	seg_rtn.head = NULL;
	seg_rtn.tail = NULL;
	switch(node_DecList->info.ruleId)
	{
		case 37:
			{
				seg_rtn = translate_node_Dec(node_DecList->child[0]);
				break;			
			}
		case 38:
			{
				struct CodeSegment seg_Dec = translate_node_Dec(node_DecList->child[0]);
				struct CodeSegment seg_DecList = translate_node_DecList(node_DecList->child[2]);
				seg_rtn = linkCodeSegment(seg_Dec,seg_DecList);
				break;
			}
		default:
			{
				dbg_constprt("程序内部错误(%s,%d)\r\n",__FILE__,__LINE__);
			}
	}
	return seg_rtn;	
}
struct CodeSegment translate_node_Dec(struct Node* node_Dec)
{
	/*
	 * Dec	:	VarDec (41)
	 * 	|	VarDec ASSIGNOP Exp (42)
	 */
	struct CodeSegment seg_rtn;
	seg_rtn.head = NULL;
	seg_rtn.tail = NULL;
	switch(node_Dec->info.ruleId)
	{
		case 41:
			{
				struct Prop* p_VarDec = translate_node_VarDec(node_Dec->child[0]);
				if(p_VarDec==NULL)
				{
					//在下层已用dbg_constprt报过错
					break;
				}
				struct Type* t_VarDec = p_VarDec->un.varprop->type;
				//根据VarDec的数据类型决定是否/怎样使用DEC指令
				if(t_VarDec->kind==0)
				{	//基本类型
					seg_rtn.head = NULL;
					seg_rtn.tail = NULL;
				}
				else if(t_VarDec->kind==1)
				{
					//数组类型
					seg_rtn = translate_allocateSpaceForArrayOrStruct(p_VarDec);
				}
				else if(t_VarDec->kind==2)
				{
					//结构体类型
					seg_rtn = translate_allocateSpaceForArrayOrStruct(p_VarDec);
				}
				freeProp(p_VarDec);
				p_VarDec = NULL;
				break;
			}
		case 42:
			{
				struct Prop* p_VarDec = translate_node_VarDec(node_Dec->child[0]);
				if(p_VarDec==NULL)
				{
					//在下层已用dbg_constprt报过错
					break;
				}
				struct Type* t_VarDec = p_VarDec->un.varprop->type;
				if(node_Dec->info.ruleId==42 && t_VarDec->kind!=0)
				{
					dbg_constprt("程序内部警告(%s,%d)：无法处理对非基本类型的初始化\r\n",__FILE__,__LINE__);
				}
				//code1(左值)
				//根据左值VarDec的数据类型决定是否/怎样使用DEC指令
				struct CodeSegment seg_code1;
				seg_code1.head = NULL;
				seg_code1.tail = NULL;
				if(t_VarDec->kind==0)
				{	//基本类型
					seg_code1.head = NULL;
					seg_code1.tail = NULL;
				}
				else if(t_VarDec->kind==1)
				{
					//数组类型
					seg_code1 = translate_allocateSpaceForArrayOrStruct(p_VarDec);
				}
				else if(t_VarDec->kind==2)
				{
					//结构体类型
					seg_code1 = translate_allocateSpaceForArrayOrStruct(p_VarDec);
				}
				//code2(右值Exp)
				//tmp1
				char c_tmp1[16] = "";
				strcpy(c_tmp1,newTmpName());
				int opd_tmp1 = -1;
				struct CodeSegment seg_code2 = translate_node_Exp(node_Dec->child[2],c_tmp1,&opd_tmp1);
				//code3：赋值
				struct Code* code3 = newCode(2);
				code3->op = OP_ASS;
				code3->op1->kind = OPD_VAR;	//无法处理左值为非基本类型的赋值
				strcpy(code3->op1->name,p_VarDec->name);
				code3->op2->kind = opd_tmp1;
				strcpy(code3->op2->name,c_tmp1);
				struct CodeSegment seg_code3 = linkTwoCode(code3,code3);
				//连接code1~code3
				seg_rtn = linkCodeSegment(seg_code1,seg_code2);
				seg_rtn = linkCodeSegment(seg_rtn,seg_code3);
				//释放空间
				freeProp(p_VarDec);
				p_VarDec = NULL;
				break;
			}
		default:
			{
				dbg_constprt("程序内部错误(%s,%d)\r\n",__FILE__,__LINE__);
			}
	}
	return seg_rtn;
}
struct CodeSegment translate_node_StmtList(struct Node* node_StmtList)
{
	/* StmtList	:	Stmt StmtList (26)
	 *		|	epsilon (27)
	 */
	struct CodeSegment seg_rtn;
	seg_rtn.head = NULL;
	seg_rtn.tail = NULL;
	switch(node_StmtList->info.ruleId)
	{
		case 26:
			{
				struct CodeSegment seg_Stmt = translate_node_Stmt(node_StmtList->child[0]);
				struct CodeSegment seg_StmtList = translate_node_StmtList(node_StmtList->child[1]);
				seg_rtn = linkCodeSegment(seg_Stmt,seg_StmtList);
				break;
			}
		case 27:
			{
				seg_rtn.head = NULL;
				seg_rtn.tail = NULL;
				break;
			}
		default:
			{
				dbg_constprt("程序内部错误(%s,%d)\r\n",__FILE__,__LINE__);
			}
	}
	return seg_rtn;
}
struct CodeSegment translate_node_Stmt(struct Node* node_Stmt)
{
	/*
	 * Stmt	:	Exp SEMI (28)
	 *    	|	CompSt (29)
	 * 	|	RETURN Exp SEMI (30)
	 * 	|	IF LP Exp RP Stmt (31)
	 * 	|	IF LP Exp RP Stmt ELSE Stmt (32)
	 * 	|	WHILE LP Exp RP Stmt (33)
	 */
	struct CodeSegment seg_rtn;
	seg_rtn.head = NULL;
	seg_rtn.tail = NULL;
	switch(node_Stmt->info.ruleId)
	{
		case 28:
			{
				char c_tmp1[16] = "";
				strcpy(c_tmp1,newTmpName());
				int opd_tmp1 = -1;
				seg_rtn = translate_node_Exp(node_Stmt->child[0],c_tmp1,&opd_tmp1);
				break;
			}
		case 29:
			{
				seg_rtn = translate_node_CompSt(node_Stmt->child[0]);
				break;
			}
		case 30:
			{
				//code1
				char c_tmp1[16] = "";
				strcpy(c_tmp1,newTmpName());
				int opd_tmp1 = -1;
				struct CodeSegment seg_code1 = translate_node_Exp(node_Stmt->child[1],c_tmp1,&opd_tmp1);
				//code2
				struct Code* code2 = newCode(1);
				code2->op = OP_RET;
				code2->op1->kind = opd_tmp1;
				strcpy(code2->op1->name,c_tmp1);
				struct CodeSegment seg_code2 = linkTwoCode(code2,code2);
				//连接code1~code2
				seg_rtn = linkCodeSegment(seg_code1,seg_code2);
				break;
			}
		case 31:
			{
				//label1和label2
				char c_label1[16] = "";
				strcpy(c_label1,newLabelName());
				char c_label2[16] = "";
				strcpy(c_label2,newLabelName());
				//code1
				struct CodeSegment seg_code1 = translate_Cond(node_Stmt->child[2],c_label1,c_label2);
				//code2
				struct CodeSegment seg_code2 = translate_node_Stmt(node_Stmt->child[4]);
				//code3:label1
				struct Code* code3 = newCode(1);
				code3->op = OP_LAB;
				code3->op1->kind = OPD_LAB;
				strcpy(code3->op1->name,c_label1);
				struct CodeSegment seg_code3 = linkTwoCode(code3,code3);
				//code4:label2
				struct Code* code4 = newCode(1);
				code4->op = OP_LAB;
				code4->op1->kind = OPD_LAB;
				strcpy(code4->op1->name,c_label2);
				struct CodeSegment seg_code4 = linkTwoCode(code4,code4);
				//连接code1+code3+code2+code4
				seg_rtn = linkCodeSegment(seg_code1,seg_code3);
				seg_rtn = linkCodeSegment(seg_rtn,seg_code2);
				seg_rtn = linkCodeSegment(seg_rtn,seg_code4);
				break;
			}
		case 32:
			{
				//label1、label2和label3
				char c_label1[16] = "";
				strcpy(c_label1,newLabelName());
				char c_label2[16] = "";
				strcpy(c_label2,newLabelName());
				char c_label3[16] = "";
				strcpy(c_label3,newLabelName());
				//code1
				struct CodeSegment seg_code1 = translate_Cond(node_Stmt->child[2],c_label1,c_label2);
				//code2
				struct CodeSegment seg_code2 = translate_node_Stmt(node_Stmt->child[4]);
				//code3
				struct CodeSegment seg_code3 = translate_node_Stmt(node_Stmt->child[6]);
				//code4:label1
				struct Code* code4 = newCode(1);
				code4->op = OP_LAB;
				code4->op1->kind = OPD_LAB;
				strcpy(code4->op1->name,c_label1);
				struct CodeSegment seg_code4 = linkTwoCode(code4,code4);
				//code5:goto label3
				struct Code* code5 = newCode(1);
				code5->op = OP_GTO;
				code5->op1->kind = OPD_LAB;
				strcpy(code5->op1->name,c_label3);
				struct CodeSegment seg_code5 = linkTwoCode(code5,code5);				
				//code6:label2
				struct Code* code6 = newCode(1);
				code6->op = OP_LAB;
				code6->op1->kind = OPD_LAB;
				strcpy(code6->op1->name,c_label2);
				struct CodeSegment seg_code6 = linkTwoCode(code6,code6);
				//code7
				struct Code* code7 = newCode(1);
				code7->op = OP_LAB;
				code7->op1->kind = OPD_LAB;
				strcpy(code7->op1->name,c_label3);
				struct CodeSegment seg_code7 = linkTwoCode(code7,code7);
				//连接code1+code4+code2+code5+code6+code3+code7
				seg_rtn = linkCodeSegment(seg_code1,seg_code4);
				seg_rtn = linkCodeSegment(seg_rtn,seg_code2);
				seg_rtn = linkCodeSegment(seg_rtn,seg_code5);
				seg_rtn = linkCodeSegment(seg_rtn,seg_code6);
				seg_rtn = linkCodeSegment(seg_rtn,seg_code3);
				seg_rtn = linkCodeSegment(seg_rtn,seg_code7);
				break;
			}
		case 33:
			{
				//label1、label2和label3
				char c_label1[16] = "";
				strcpy(c_label1,newLabelName());
				char c_label2[16] = "";
				strcpy(c_label2,newLabelName());
				char c_label3[16] = "";
				strcpy(c_label3,newLabelName());
				//code1
				struct CodeSegment seg_code1 = translate_Cond(node_Stmt->child[2],c_label2,c_label3);
				//code2
				struct CodeSegment seg_code2 = translate_node_Stmt(node_Stmt->child[4]);
				//code3:label1
				struct Code* code3 = newCode(1);
				code3->op = OP_LAB;
				code3->op1->kind = OPD_LAB;
				strcpy(code3->op1->name,c_label1);
				struct CodeSegment seg_code3 = linkTwoCode(code3,code3);
				//code4:label2
				struct Code* code4 = newCode(1);
				code4->op = OP_LAB;
				code4->op1->kind = OPD_LAB;
				strcpy(code4->op1->name,c_label2);
				struct CodeSegment seg_code4 = linkTwoCode(code4,code4);
				//code5:goto label1
				struct Code* code5 = newCode(1);
				code5->op = OP_GTO;
				code5->op1->kind = OPD_LAB;
				strcpy(code5->op1->name,c_label1);
				struct CodeSegment seg_code5 = linkTwoCode(code5,code5);
				//code6:label3
				struct Code* code6 = newCode(1);
				code6->op = OP_LAB;
				code6->op1->kind = OPD_LAB;
				strcpy(code6->op1->name,c_label3);
				struct CodeSegment seg_code6 = linkTwoCode(code6,code6);
				//连接code3+code1+code4+code2+code5+code6
				seg_rtn = linkCodeSegment(seg_code3,seg_code1);
				seg_rtn = linkCodeSegment(seg_rtn,seg_code4);
				seg_rtn = linkCodeSegment(seg_rtn,seg_code2);
				seg_rtn = linkCodeSegment(seg_rtn,seg_code5);
				seg_rtn = linkCodeSegment(seg_rtn,seg_code6);
				break;
			}
		default:
			{
				dbg_constprt("程序内部错误(%s,%d)\r\n",__FILE__,__LINE__);
			}
	}
	return seg_rtn;
}
struct CodeSegment translate_node_Exp(struct Node* node_Exp,const char* place,int* opd_place)
{
	/*
	 * Exp	:	Exp ASSIGNOP Exp (43)
	 * 	|	Exp AND Exp (44)
	 * 	|	Exp OR Exp (45)
	 * 	|	Exp RELOP Exp (46)
	 * 	|	Exp PLUS Exp (47)
	 * 	|	Exp MINUS Exp (48)
	 * 	|	Exp STAR Exp (49)
	 * 	|	Exp DIV Exp (50)
	 * 	|	LP Exp RP (51)
	 * 	|	MINUS Exp (52)
	 * 	|	NOT Exp (53)
	 * 	|	ID LP Args RP (54)
	 * 	|	ID LP RP (55)
	 * 	|	Exp LB Exp RB (56)
	 * 	|	Exp DOT ID (57)
	 * 	|	ID (58)
	 * 	|	INT (59)
	 * 	|	FLOAT (60)
	 */

	struct CodeSegment seg_rtn;
	seg_rtn.head = NULL;
	seg_rtn.tail = NULL;
	switch(node_Exp->info.ruleId)
	{
		case 43:	/* Exp -> Exp ASSIGNOP Exp	*/
			{
				char idname[256]="";
				int lft_exp_kind = whichKindOfExp(node_Exp->child[0],idname);
				if(1==lft_exp_kind)
				{
					//如果等号左侧是ID
					//tmp1
					char c_tmp1[16] = "";
					strcpy(c_tmp1,newTmpName());
					int opd_tmp1 = -1;
					//code1
					struct CodeSegment seg_code1 = translate_node_Exp(node_Exp->child[2],c_tmp1,&opd_tmp1);
					//code2_1
					struct Code* code2_1 = newCode(2);
					code2_1->op = OP_ASS;
					code2_1->op1->kind = OPD_VAR;
					strcpy(code2_1->op1->name,idname);
					code2_1->op2->kind = opd_tmp1;
					strcpy(code2_1->op2->name,c_tmp1);
					//code2_2
					struct Code* code2_2 = newCode(2);
					code2_2->op = OP_ASS;
					code2_2->op1->kind = OPD_VAR;
					strcpy(code2_2->op1->name,place);
					code2_2->op2->kind = OPD_VAR;
					strcpy(code2_2->op2->name,idname);
					//code2
					struct CodeSegment seg_code2 = linkTwoCode(code2_1,code2_2);
					//连接code1~code2
					seg_rtn = linkCodeSegment(seg_code1,seg_code2);
					//填写opd_place
					*opd_place = opd_tmp1;
					break;
				}
				else if(3==lft_exp_kind || 4 == lft_exp_kind)
				{
					//如果等号左侧是数组元素或结构体的域
					//tmp1和tmp2
					char c_tmp1[16] = "";
					strcpy(c_tmp1,newTmpName());
					int opd_tmp1 = -1;
					char c_tmp2[16] = "";
					strcpy(c_tmp2,newTmpName());
					int opd_tmp2 = -1;
					//code1
					struct CodeSegment seg_code1 = translate_node_Exp(node_Exp->child[0],c_tmp1,&opd_tmp1);
					//code2
					struct CodeSegment seg_code2 = translate_node_Exp(node_Exp->child[2],c_tmp2,&opd_tmp2);
					//code3: 赋值ass
					struct Code* code3 = newCode(2);
					code3->op = OP_ASS;
					code3->op1->kind = opd_tmp1;
					strcpy(code3->op1->name,c_tmp1);
					code3->op2->kind = opd_tmp2;
					strcpy(code3->op2->name,c_tmp2);
					struct CodeSegment seg_code3 = linkTwoCode(code3,code3);
					//code4：选择右边的赋值给place，以期可以优化:
					struct Code* code4 = newCode(2);
					code4->op = OP_ASS;
					code4->op1->kind = OPD_VAR;
					strcpy(code4->op1->name,place);
					code4->op2->kind = opd_tmp2;
					strcpy(code4->op2->name,c_tmp2);
					struct CodeSegment seg_code4 = linkTwoCode(code4,code4);
					//连接code1~code4
					seg_rtn = linkCodeSegment(seg_code1,seg_code2);
					seg_rtn = linkCodeSegment(seg_rtn,seg_code3);
					seg_rtn = linkCodeSegment(seg_rtn,seg_code4);
					//填写opd_place
					*opd_place = opd_tmp2;
					break;
				}
				else
				{
					dbg_constprt("程序内部错误(%s,%d)：似乎有非法的左值\r\n",__FILE__,__LINE__);
					break;
				}
				break;
			}
		case 44:	/* Exp -> Exp AND Exp	*/
		case 45:	/* Exp -> Exp OR Exp 	*/
		case 46:	/* Exp -> Exp RELOP Exp	*/
		case 53:	/* Exp -> NOT Exp	*/
			{
				//label1和label2
				char c_label1[16] = "";
				strcpy(c_label1,newLabelName());
				char c_label2[16] = "";
				strcpy(c_label2,newLabelName());
				//code0
				struct Code* code0 = newCode(2);
				code0->op = OP_ASS;
				code0->op1->kind = OPD_VAR;
				strcpy(code0->op1->name,place);
				code0->op2->kind = OPD_CON;
				strcpy(code0->op2->name,"#0");
				struct CodeSegment seg_code0 = linkTwoCode(code0,code0);
				//code1
				struct CodeSegment seg_code1 = translate_Cond(node_Exp,c_label1,c_label2);
				//code2
				struct Code* code2_1 = newCode(1);
				code2_1->op = OP_LAB;
				code2_1->op1->kind = OPD_LAB;
				strcpy(code2_1->op1->name,c_label1);
				struct Code* code2_2 = newCode(2);
				code2_2->op = OP_ASS;
				code2_2->op1->kind = OPD_VAR;
				strcpy(code2_2->op1->name,place);
				code2_2->op2->kind = OPD_CON;
				strcpy(code2_2->op2->name,"#1");
				struct CodeSegment seg_code2 = linkTwoCode(code2_1,code2_2);
				//code3
				struct Code* code3 = newCode(1);
				code3->op = OP_LAB;
				code3->op1->kind = OPD_LAB;
				strcpy(code3->op1->name,c_label2);
				struct CodeSegment seg_code3 = linkTwoCode(code3,code3);
				//把code0~code3连起来
				seg_rtn = linkCodeSegment(seg_code0,seg_code1);
				seg_rtn = linkCodeSegment(seg_rtn,seg_code2);
				seg_rtn = linkCodeSegment(seg_rtn,seg_code3);
				//填写opd_place
				*opd_place = OPD_VAR;
				break;
			}
		case 47:	/* Exp -> Exp PLUS Exp	*/
		case 48:	/* Exp -> Exp MINUS Exp	*/
		case 49:	/* Exp -> Exp STAR Exp	*/
		case 50:	/* Exp -> Exp DIV Exp	*/
			{
				//tmp1和tmp2
				char c_tmp1[16] = "";
				strcpy(c_tmp1,newTmpName());
				int opd_tmp1 = -1;
				char c_tmp2[16] = "";
				strcpy(c_tmp2,newTmpName());
				int opd_tmp2 = -1;
				//code1
				struct CodeSegment seg_code1 = translate_node_Exp(node_Exp->child[0],c_tmp1,&opd_tmp1);
				//code2
				struct CodeSegment seg_code2 = translate_node_Exp(node_Exp->child[2],c_tmp2,&opd_tmp2);
				//code3
				struct Code* code3 = newCode(3);
				int ruleId = node_Exp->info.ruleId;
				code3->op = (ruleId==47)?(OP_ADD):((ruleId==48)?(OP_SUB):((ruleId==49)?(OP_MUL):(OP_DIV)));

				code3->op1->kind = OPD_VAR;
				strcpy(code3->op1->name,place);
				code3->op2->kind = opd_tmp1;
				strcpy(code3->op2->name,c_tmp1);
				code3->op3->kind = opd_tmp2;
				strcpy(code3->op3->name,c_tmp2);
				struct CodeSegment seg_code3 = linkTwoCode(code3,code3);
				//连接code1~code3
				seg_rtn = linkCodeSegment(seg_code1,seg_code2);
				seg_rtn = linkCodeSegment(seg_rtn,seg_code3);
				//填写opd_place
				*opd_place = OPD_VAR;
				break;

			}
		case 51:	/* Exp -> LP Exp RP	*/
			{
				seg_rtn = translate_node_Exp(node_Exp->child[1],place,opd_place);
				break;
			}
		case 52:	/* Exp -> MINUS Exp	*/
			{
				//tmp1
				char c_tmp1[16] = "";
				strcpy(c_tmp1,newTmpName());
				int opd_tmp1 = -1;
				//code1
				struct CodeSegment seg_code1 = translate_node_Exp(node_Exp->child[1],c_tmp1,&opd_tmp1);
				//code2
				struct Code* code2 = newCode(3);
				code2->op = OP_SUB;
				code2->op1->kind = OPD_VAR;
				strcpy(code2->op1->name,place);
				code2->op2->kind = OPD_CON;
				strcpy(code2->op2->name,"#0");
				code2->op3->kind = opd_tmp1;
				strcpy(code2->op3->name,c_tmp1);
				struct CodeSegment seg_code2 = linkTwoCode(code2,code2);
				//连接code1~code2
				seg_rtn = linkCodeSegment(seg_code1,seg_code2);
				//填写opd_place
				*opd_place = OPD_VAR;
				break;
			}
		case 54:	/* Exp -> ID LP Args RP	*/
			{
				char* name = node_Exp->child[0]->info.notation;
				struct ArgList* arg_list = NULL;
				struct CodeSegment seg_code1 = translate_node_Args(node_Exp->child[2],&arg_list);
				if(arg_list==NULL)
				{
					dbg_constprt("程序内部错误(%s,%d)：获取参数列表失败\r\n",__FILE__,__LINE__);
					break;
				}
				if(strcmp("write",name)==0)
				{
					//对write函数做特殊处理
					struct Code* code2 = newCode(1);
					code2->op = OP_WRI;
					code2->op1->kind = arg_list->opd_kind;
					strcpy(code2->op1->name,arg_list->argname);
					freeArgList(arg_list);	//释放arg_list空间
					arg_list = NULL;
					struct CodeSegment seg_code2 = linkTwoCode(code2,code2);
					seg_rtn = linkCodeSegment(seg_code1,seg_code2);
					//填写opd_place
					*opd_place = OPD_VAR;
					break;
				}
				else
				{
					//不是write函数，正常处理
					//code2：获取形参ARG
					struct Code* code2 = newCode(1);
					code2->op = OP_ARG;
//					code2->op1->kind = arg_list->opd_kind;
					if(arg_list->opd_kind==OPD_GCN)
						code2->op1->kind = OPD_GCN;
					else
						code2->op1->kind = OPD_VAR;	//压参数你就别管什么类型了，让你压什么你就压什么
					strcpy(code2->op1->name,arg_list->argname);
					struct CodeSegment seg_code2 = linkTwoCode(code2,code2);
					struct ArgList* p_arglst = arg_list->next;
					while(p_arglst!=NULL)
					{
						struct Code* code2_2 = newCode(1);
						code2_2->op = OP_ARG;
//						code2_2->op1->kind = p_arglst->opd_kind;
						if(p_arglst->opd_kind==OPD_GCN)
							code2_2->op1->kind = OPD_GCN;
						else
							code2_2->op1->kind = OPD_VAR;	//压参数你就别管什么类型了，让你压什么你就压什么
						strcpy(code2_2->op1->name,p_arglst->argname);
						struct CodeSegment seg_code2_2 = linkTwoCode(code2_2,code2_2);
						//这句决定了传参数的顺序！！！
						seg_code2 = linkCodeSegment(seg_code2_2,seg_code2);
						p_arglst = p_arglst->next;
					}
					freeArgList(arg_list);	//释放arg_list空间
					arg_list = NULL;
					//code3：CALL
					struct Code* code3 = newCode(2);
					code3->op = OP_CAL;
					code3->op1->kind = OPD_VAR;
					strcpy(code3->op1->name,place);
					code3->op2->kind = OPD_FUN;
					strcpy(code3->op2->name,name);
					struct CodeSegment seg_code3 = linkTwoCode(code3,code3);
					seg_rtn = linkCodeSegment(seg_code1,seg_code2);
					seg_rtn = linkCodeSegment(seg_rtn,seg_code3);
					//填写opd_place
					*opd_place = OPD_VAR;
					break;
				}
				break;
			}
		case 55:	/* Exp -> ID LP RP	*/
			{
				char* name = node_Exp->child[0]->info.notation;
				if(strcmp("read",name)==0)
				{
					//对read函数做特殊处理
					struct Code* code1 = newCode(1);
					code1->op = OP_RED;
					code1->op1->kind = OPD_VAR;
					strcpy(code1->op1->name,place);
					seg_rtn = linkTwoCode(code1,code1);
				}
				else
				{
					//不是read函数，正常处理
					struct Code* code1 = newCode(2);
					code1->op = OP_CAL;
					code1->op1->kind = OPD_VAR;
					strcpy(code1->op1->name,place);
					code1->op2->kind = OPD_FUN;
					strcpy(code1->op2->name,name);
					seg_rtn = linkTwoCode(code1,code1);
				}
				//填写opd_place
				*opd_place = OPD_VAR;
				break;
			}
		case 56:	/* Exp -> Exp LB Exp RB	*/
		case 57:	/* Exp -> Exp DOT ID	*/
			{
				//计算偏移的1字节数
				char c_tmp1[16] = "";	//c_tmp1将记录所访问的空间的地址（单位1字节）
				strcpy(c_tmp1,newTmpName());
				struct Type* mytype = NULL;
				struct CodeSegment seg_code1 = calculateArrOrStructOffset(node_Exp,&mytype,c_tmp1);
				if(mytype==NULL)
				{
					dbg_constprt("程序内部错误(%s,%d)：没有成功获取到基本类型，calculateArrOrStructOffset()函数可能存在逻辑层次错误\r\n",__FILE__,__LINE__);
					break;
				}
/*				if(typeIsBasic(mytype,0)==0 && typeIsBasic(mytype,1)==0)
				{
					dbg_constprt("程序内部错误(%s,%d)：顶层获取到的不是基本类型，calculateArrOrStructOffset()函数可能存在逻辑层次错误\r\n",__FILE__,__LINE__);
					freeType(mytype);	//enable this line may cause awkward and mysterious bug!!!
					mytype=NULL;
					break;
				}
*/
//				freeType(mytype);	//enable this line may cause awkward and mysterious bug!!!
				mytype=NULL;
				//实际的偏移地址赋给place
				struct Code* code2 = newCode(2);
				code2->op = OP_ASS;
				code2->op1->kind = OPD_VAR;
				strcpy(code2->op1->name,place);
				code2->op2->kind = OPD_VAR;
				strcpy(code2->op2->name,c_tmp1);
				struct CodeSegment seg_code2 = linkTwoCode(code2,code2);
				seg_rtn = linkCodeSegment(seg_code1,seg_code2);
				//填写opd_place
				*opd_place = OPD_GCN;		//是OPD_GCN吗???
				break;
			}
		case 58:	/* Exp -> ID	*/
			{
				char* name = node_Exp->child[0]->info.notation;
				struct Code* code1 = newCode(2);
				code1->op = OP_ASS;
				code1->op1->kind = OPD_VAR;
				strcpy(code1->op1->name,place);
//				code1->op2->kind = OPD_VAR;
				strcpy(code1->op2->name,name);
				seg_rtn = linkTwoCode(code1,code1);
				//填写opd_place
				struct Prop* id_prop = searchPropStackByPropName(name,0,1,0);
				if(id_prop==NULL)
				{
					dbg_constprt("程序内部错误(%s,%d)：找不到名为%s的变量\r\n",__FILE__,__LINE__,name);
					break;
				}
				if(typeIsBasic(id_prop->un.varprop->type,0))
				{
					*opd_place = OPD_VAR;
					code1->op2->kind = OPD_VAR;
				}
				else if(typeIsBasic(id_prop->un.varprop->type,1))
				{
					dbg_constprt("程序内部警告(%s,%d)：似乎出现了浮点型变量%s\r\n",__FILE__,__LINE__,name);
					*opd_place = OPD_VAR;
					code1->op2->kind = OPD_VAR;
				}
				else
				{
					//*opd_place = OPD_GCN;Exp上层直接使用一个不为基本类型的ID的情形，只有再函数调用传参时
					*opd_place = OPD_VAR;
					code1->op2->kind = OPD_GAD;
				}

//				*opd_place = OPD_VAR;
				break;
			}
		case 59:
			{
				unsigned int value = node_Exp->child[0]->info.value.INT;
				struct Code* code1 = newCode(2);
				code1->op=OP_ASS;
				code1->op1->kind = OPD_VAR;
				strcpy(code1->op1->name,place);
				code1->op2->kind = OPD_CON;
				sprintf(code1->op2->name,"#%u",value);
				seg_rtn = linkTwoCode(code1,code1);
				//填写opd_place
				*opd_place = OPD_VAR;
				break;
			}
		case 60:
			{
				float value = node_Exp->child[0]->info.value.FLOAT;
				dbg_constprt("程序内部警告(%s,%d)：读入代码中出现了浮点数%f\r\n",__FILE__,__LINE__,value);
				struct Code* c_place = newCode(2);
				c_place->op=OP_ASS;
				c_place->op1->kind = OPD_VAR;
				strcpy(c_place->op1->name,place);
				c_place->op2->kind = OPD_CON;
				sprintf(c_place->op2->name,"#%f",value);
				seg_rtn.head = c_place;
				seg_rtn.tail = c_place;
				//填写opd_place
				*opd_place = OPD_VAR;
				break;
			}
		default:
			{
				dbg_constprt("程序内部警告(%s,%d)\r\n",__FILE__,__LINE__);
			}
	}
	return seg_rtn;
	
}
struct CodeSegment translate_node_Args(struct Node* node_Args,struct ArgList** arg_list)
{
	/* Args	:	Exp COMMA Args (61)
	 * 	|	Exp (62)
	 */
	struct CodeSegment seg_rtn;
	seg_rtn.head = NULL;
	seg_rtn.tail = NULL;
	switch(node_Args->info.ruleId)
	{
		case 61:
			{
				char c_tmp1[16] = "";
				strcpy(c_tmp1,newTmpName());
				int opd_tmp1 = -1;
				struct CodeSegment seg_code1 = translate_node_Exp(node_Args->child[0],c_tmp1,&opd_tmp1);
				*arg_list = (struct ArgList*)malloc(sizeof(struct ArgList));
				strcpy((*arg_list)->argname,c_tmp1);
				struct Type* type_Exp = analyze_node_Exp(node_Args->child[0],0);
				if(typeIsBasic(type_Exp,0) || typeIsBasic(type_Exp,1))
				{
					(*arg_list)->opd_kind = opd_tmp1;
				}
				else
				{
					(*arg_list)->opd_kind = OPD_VAR;
				}
//				freeType(type_Exp);	//enable this line may cause awkward and mysterious bug!!!
				type_Exp=NULL;
				struct ArgList* next_arg_list = NULL;
				struct CodeSegment seg_code2 = translate_node_Args(node_Args->child[2],&(next_arg_list));
				(*arg_list)->next = next_arg_list;
				seg_rtn = linkCodeSegment(seg_code1,seg_code2);
				break;
			}
		case 62:
			{
				char c_tmp1[16] = "";
				strcpy(c_tmp1,newTmpName());
				int opd_tmp1 = -1;
				struct CodeSegment seg_code1 = translate_node_Exp(node_Args->child[0],c_tmp1,&opd_tmp1);
				*arg_list = (struct ArgList*)malloc(sizeof(struct ArgList));
				strcpy((*arg_list)->argname,c_tmp1);
//				(*arg_list)->opd_kind = opd_tmp1;
				struct Type* type_Exp = analyze_node_Exp(node_Args->child[0],0);
				if(typeIsBasic(type_Exp,0) || typeIsBasic(type_Exp,1))
				{
					(*arg_list)->opd_kind = opd_tmp1;
				}
				else
				{
					(*arg_list)->opd_kind = OPD_VAR;
				}
//				freeType(type_Exp);	//enable this line ALWAYS cause awkward and mysterious bug!!!
				type_Exp=NULL;
				(*arg_list)->next = NULL;
				seg_rtn = seg_code1;
				break;
			}
		default:
			{
				dbg_constprt("程序内部警告(%s,%d)\r\n",__FILE__,__LINE__);
			}
	}
	return seg_rtn;
}
struct CodeSegment translate_Cond(struct Node* node_Exp,const char* label_true,const char* label_false)
{
	/*
	 * Exp(Cond)	:
	 * 	|	Exp AND Exp (44)
	 * 	|	Exp OR Exp (45)
	 * 	|	Exp RELOP Exp (46)
	 * 	|	NOT Exp (53)
	 * 	|	default -- other cases
	 */
	struct CodeSegment seg_rtn;
	seg_rtn.head = NULL;
	seg_rtn.tail = NULL;
	switch(node_Exp->info.ruleId)
	{
		case 44:
			{
				//label1
				char c_label1[16] = "";
				strcpy(c_label1,newLabelName());
				//code1
				struct CodeSegment seg_code1 = translate_Cond(node_Exp->child[0],c_label1,label_false);
				//code2
				struct CodeSegment seg_code2 = translate_Cond(node_Exp->child[2],label_true,label_false);
				//code3
				struct Code* code3 = newCode(1);
				code3->op = OP_LAB;
				code3->op1->kind = OPD_LAB;
				strcpy(code3->op1->name,c_label1);
				struct CodeSegment seg_code3 = linkTwoCode(code3,code3);
				//连接code1+code3+code2
				seg_rtn = linkCodeSegment(seg_code1,seg_code3);
				seg_rtn = linkCodeSegment(seg_rtn,seg_code2);
				break;
			}
		case 45:
			{
				//label1
				char c_label1[16] = "";
				strcpy(c_label1,newLabelName());
				//code1
				struct CodeSegment seg_code1 = translate_Cond(node_Exp->child[0],label_true,c_label1);
				//code2
				struct CodeSegment seg_code2 = translate_Cond(node_Exp->child[2],label_true,label_false);
				//code3
				struct Code* code3 = newCode(1);
				code3->op = OP_LAB;
				code3->op1->kind = OPD_LAB;
				strcpy(code3->op1->name,c_label1);
				struct CodeSegment seg_code3 = linkTwoCode(code3,code3);
				//连接code1+code3+code2
				seg_rtn = linkCodeSegment(seg_code1,seg_code3);
				seg_rtn = linkCodeSegment(seg_rtn,seg_code2);
				break;
			}
		case 46:
			{
				//tmp1和tmp2
				char c_tmp1[16] = "";
			       	strcpy(c_tmp1,newTmpName());
				int opd_tmp1 = -1;
				char c_tmp2[16] = "";
			       	strcpy(c_tmp2,newTmpName());
				int opd_tmp2 = -2;
				//code1
				struct CodeSegment seg_code1 = translate_node_Exp(node_Exp->child[0],c_tmp1,&opd_tmp1);
				//code2
				struct CodeSegment seg_code2 = translate_node_Exp(node_Exp->child[2],c_tmp2,&opd_tmp2);
				//code3
				struct Code* code3 = newCode(4);
				code3->op = OP_IFJ;
				code3->op1->kind = opd_tmp1;
				strcpy(code3->op1->name,c_tmp1);
				code3->op2->kind = OPD_REL;
				strcpy(code3->op2->name,node_Exp->child[1]->info.notation);
				code3->op3->kind = opd_tmp2;
				strcpy(code3->op3->name,c_tmp2);
				code3->op4->kind = OPD_LAB;
				strcpy(code3->op4->name,label_true);
				struct CodeSegment seg_code3 = linkTwoCode(code3,code3);
				//code4
				struct Code* code4 = newCode(1);
				code4->op = OP_GTO;
				code4->op1->kind = OPD_LAB;
				strcpy(code4->op1->name,label_false);
				struct CodeSegment seg_code4 = linkTwoCode(code4,code4);
				//连接code1~code4
				seg_rtn = linkCodeSegment(seg_code1,seg_code2);
				seg_rtn = linkCodeSegment(seg_rtn,seg_code3);
				seg_rtn = linkCodeSegment(seg_rtn,seg_code4);
				break;
			}
		case 53:
			{
				seg_rtn = translate_Cond(node_Exp->child[1],label_false,label_true);
				break;
			}
		default:
			{
				//tmp1
				char c_tmp1[16] = "";
				strcpy(c_tmp1,newTmpName());
				int opd_tmp1 = -1;
				//code1
				struct CodeSegment seg_code1 = translate_node_Exp(node_Exp,c_tmp1,&opd_tmp1);
				//code2
				struct Code* code2 = newCode(4);
				code2->op = OP_IFJ;
				code2->op1->kind = opd_tmp1;
				strcpy(code2->op1->name,c_tmp1);
				code2->op2->kind = OPD_REL;
				strcpy(code2->op2->name,"!=");
				code2->op3->kind = OPD_CON;
				strcpy(code2->op3->name,"#0");
				code2->op4->kind = OPD_LAB;
				strcpy(code2->op4->name,label_true);
				struct CodeSegment seg_code2 = linkTwoCode(code2,code2);
				//code3
				struct Code* code3 = newCode(1);
				code3->op = OP_GTO;
				code3->op1->kind = OPD_LAB;
				strcpy(code3->op1->name,label_false);
				struct CodeSegment seg_code3 = linkTwoCode(code3,code3);
				//连接code1~code3
				seg_rtn = linkCodeSegment(seg_code1,seg_code2);
				seg_rtn = linkCodeSegment(seg_rtn,seg_code3);
				break;
			}
	}
	return seg_rtn;
}

/* 调试函数 */
void dbg_printCode(const struct Code* code)
{
	/* 函数功能：
	 * 	输出一行代码信息
	 * 参数：
	 * 	要被输出的一行代码
	 */
	char opdkind[7][8]={
		"OPD_LAB",	//label
		"OPD_FUN",	//function
		"OPD_VAR",	//variable
		"OPD_CON",	//constant
		"OPD_REL",	//relation token
		"OPD_GCN",	//get content (*)
		"OPD_GAD"		//get address (&)
	};
	char opkind[19][7]={
		"OP_LAB","OP_FUN","OP_ASS","OP_ADD","OP_SUB",
		"OP_MUL","OP_DIV",
		"OP_GTO","OP_IFJ","OP_RET","OP_DEC","OP_ARG",
		"OP_CAL","OP_PAR","OP_RED","OP_WRI"};
	printf("op-kind=%s\r\n",opkind[code->op]);
	if(code->op1!=NULL)
		printf("op1.kind=%s\top1.name=%s\r\n",opdkind[code->op1->kind],code->op1->name);
	if(code->op2!=NULL)
		printf("op2.kind=%s\top2.name=%s\r\n",opdkind[code->op2->kind],code->op2->name);
	if(code->op3!=NULL)
		printf("op3.kind=%s\top3.name=%s\r\n",opdkind[code->op3->kind],code->op3->name);
	if(code->op4!=NULL)
		printf("op4.kind=%s\top4.name=%s\r\n",opdkind[code->op4->kind],code->op4->name);
	printf("=================================\r\n");
}
void dbg_printCodeSegment(const struct CodeSegment seg_code)
{
	/* 函数功能：
	 * 	输出代码段中的信息
	 * 参数：
	 * 	要被输出的代码段
	 */
	struct Code* code = seg_code.head;
	printf("=================================\r\n");
	printf("开始输出代码段\r\n");
	printf("=================================\r\n");
	while(code!=seg_code.tail)
	{
		dbg_printCode(code);
		code = code->next;
	}
	if(seg_code.tail != NULL)
		dbg_printCode(seg_code.tail);
	printf("代码段输出结束\r\n");
	printf("=================================\r\n");
}






int translate_patch1()
{
	/* 函数功能：
	 * 	把操作数名称为带井号常量的操作数的kind修正为OPD_CON
	 * 返回值：
	 * 	修改的个数
	 * 其他说明：
	 * 	这是一个补丁函数。测试时发现有部分中间代码的操作数kind为OPD_VAR，但名称为#开头的常量。本函数遍历所有中间代码，并把误设为OPD_VAR的kind改为OPD_CON。
	 * 	本函数应中间代码优化之后调用
	 */
	int cnt = 0;
	struct Code* code = global_code_segment.head;
	while(code!=NULL)
	{
		int i;
		for(i=1;i<=4;i++)
		{
			struct Operand* opd = i==1?code->op1:
				(i==2?code->op2:(
				  i==3?code->op3:code->op4
				 ));
			if(opd==NULL)
			{
				break;
			}
			else
			{
				if(opd->name[0]=='#' && opd->kind==OPD_VAR)
				{
					opd->kind = OPD_CON;
					cnt++;
				}
			}

		}
		if(code==global_code_segment.tail)
			break;
		code = code->next;
	}
	return;
}
