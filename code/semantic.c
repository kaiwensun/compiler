/*
 * semantic.c
 */
#include"semantic.h"

/* 全局变量 */
int semantic_layer = 0;					/* 当前定义域的层数 */
int unnamed_struct_counter = 0;				/* 匿名结构体计数器，用为不同匿名结构体命名 */
struct Type* type_stack_top = NULL;	/* 用户自己定义的类型的栈，包括数组和结构体，不含int和float */
struct Prop* prop_stack_top = NULL;	/* 用户自己定义的变量（基本变量、数组、结构体）和函数的栈 */
extern struct Node* syntaxTreeRoot;			/* extern 语法树根节点 */

/* 函数声明 */
/* 函数声明-基本元素操作 */
void push_type_stack(struct Type* type)
{
	/* 函数功能：
	 * 	向type_stack_top中压栈
	 * 参数：
	 * 	需要压入栈中的Type（允许是链表）
	 */
	if(type==NULL)
		return;
	struct Type* tail = type;
	while(tail->next!=NULL)
	{
		tail = tail->next;
	}
	tail->next = type_stack_top;
	type_stack_top = type;
}
void pop_type_stack(int until_layer)
{
	/* 函数功能：
	 * 	从type_stack_top栈中弹栈并释放空间，直到发现栈顶元素的layer不大于until_layer
	 * 参数：
	 * 	弹出所有layer大于until_layer的元素
	 * 其他说明：
	 * 	为了适应Lab3，故改为空
	 */
	struct Type* p_type = type_stack_top;
	while(p_type!=NULL)
	{
		if(p_type->layer<=until_layer)
			break;
		p_type = freeType(p_type);
	}
	type_stack_top = p_type;
}
void push_prop_stack(struct Prop* prop)
{
	/* 函数功能：
	 * 	向prop_stack_top中压栈
	 * 参数：
	 * 	需要压入栈的Prop（允许是链表）
	 * 其他说明：
	 * 	为了适应Lab3，故改为空
	 */
	if(prop==NULL)
		return;
	struct Prop* tail = prop;
	while(tail->next!=NULL)
	{
		tail = tail->next;
	}
	tail->next = prop_stack_top;
	prop_stack_top = prop;
}
void pop_prop_stack(int until_layer)
{
	/* 函数功能：
	 * 	从prop_stack_top栈中弹栈并释放空间，直到发现栈顶元素的layer不大于until_layer
	 * 参数：
	 * 	弹出所有layer大于until_layer的元素
	 */
	struct Prop* p_prop = prop_stack_top;
	while(p_prop!=NULL)
	{
		if(p_prop->layer<=until_layer)
			break;
		p_prop = freeProp(p_prop);
	}
	prop_stack_top = p_prop;
}
struct FieldList* copyField(struct FieldList* fld)
{
	/* 函数功能：
	 * 	复制一个结构体的一个域（申请了新空间）
	 * 参数：
	 * 	fld: 要被复制的域
	 * 返回值：
	 * 	指向新复制得的域的指针
	 * 其他说明：
	 * 	新Field结构中的next为NULL
	 */
	if(fld==NULL)
		return NULL;
	struct FieldList* newfld = (struct FieldList*)malloc(sizeof(struct FieldList));
	strcpy(newfld->name,fld->name);
	newfld->type = copyType(fld->type);
	newfld->def_lineno = fld->def_lineno;
	newfld->layer = fld->layer;
	newfld->next = NULL;
	return newfld;
}
struct Type* copyType(struct Type* type)
{
	/* 函数功能：
	 * 	复制一个数据类型（如int,float,array,struct...）（申请了新空间）
	 * 参数：
	 * 	type: 要被复制的数据类型
	 * 返回值：
	 * 	指向新复制得的数据类型的指针
	 * 其他说明：
	 * 	新Type结构中的next为NULL
	 */
	if(type==NULL)
		return NULL;


	struct Type* newtype = (struct Type*)malloc(sizeof(struct Type));

	newtype->kind = type->kind;

	switch(type->kind)	//判断是basic, array,struct中的哪一类数据类型
	{
		case 0:	/* basic */
			{
				newtype->un.basic = type->un.basic;
				break;
			}
		case 1:	/* array */
			{
				newtype->un.array. arreletype = copyType(type->un.array.arreletype);
				newtype->un.array.arrsize = type->un.array.arrsize;
				break;
			}
		case 2:	/* struct */
			{
				strcpy(newtype->un.structure.struct_name,type->un.structure.struct_name);
				newtype->un.structure.struct_field = copyField(type->un.structure.struct_field);
				if(newtype->un.structure.struct_field!=NULL)
				{
					struct FieldList* dst_fld_lst_p = newtype->un.structure.struct_field;
					struct FieldList* src_fld_lst_p = type->un.structure.struct_field;
					while(src_fld_lst_p->next!=NULL)
					{
						dst_fld_lst_p->next = copyField(src_fld_lst_p->next);
						dst_fld_lst_p = dst_fld_lst_p->next;
						src_fld_lst_p = src_fld_lst_p->next;
					}
				}
				break;
			}
	}
	newtype->def_lineno = type->def_lineno;
	newtype->layer = type->layer;
	newtype->next = NULL;





	return newtype;
}
struct Prop* copyVarProp(struct Prop* prop)
{
	/* 函数功能：
	 * 	复制一个变量Prop（申请了新空间）
	 * 参数：
	 * 	prop: 要被复制的变量
	 * 返回值：
	 * 	指向新复制得的变量Prop的指针
	 * 其他说明：
	 * 	如果prop=NULL,则返回NULL
	 * 	新Type结构中的next为NULL
	 * 	如果误传进来了函数prop，会用dbgprt报错并返回NULL
	 */
	if(prop==NULL)
		return NULL;
	if(prop->kind!=0)
	{
		dbgprt("内部错误：试图用copyVarProp复制一个函数Prop(%s,%d)\r\n",__FILE__,__LINE__);
		return NULL;
	}
	struct Prop* newprop = (struct Prop*)malloc(sizeof(struct Prop));
	newprop->kind = prop->kind;
	newprop->un.varprop = (struct VarProp*)malloc(sizeof(struct VarProp));
	newprop->un.varprop->type = copyType(prop->un.varprop->type);
	newprop->un.varprop->def_lineno = prop->un.varprop->def_lineno;
	strcpy(newprop->name,prop->name);
	newprop->layer = prop->layer;
	newprop->next = NULL;
	return newprop;
}
int countArrDim(struct Type* type)
{
	/* 函数功能：
	 * 	计算数组的维度数
	 * 返回值：
	 * 	数组的维度数
	 */
	int dim=0;
	struct Type* p_type=type;
	while(p_type->kind==1)
	{
		dim++;
		p_type = p_type->un.array.arreletype;
	}
	return dim;

}
int typeIsSame(struct Type* type1,struct Type* type2)
{
	/* 函数功能：
	 * 	判断两个数据类型是否是结构等价的
	 * 参数：
	 * 	type1,type2:要被判断的两个数据类型
	 * 返回值：
	 * 	0:不等价
	 * 	1:等价
	 * 其他说明：
	 * 	采用结构等价。如果是结构体，不会比较结构体名以及结构体中域的变量名是否相等。
	 */
	
	if(type1==NULL && type2==NULL)
		return 1;
	if(type1==NULL || type2==NULL)
		return 0;
	if(type1->kind!=type2->kind)	//连kind都不同，当然不等价
		return 0;
	switch(type1->kind)
	{
		case 0:	/* basic */
			{
				if(type1->un.basic == type2->un.basic)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
		case 1:	/* array */
			{
				/* 比较数组维数 */
				int dim1=countArrDim(type1);
				int dim2=countArrDim(type2);
				if(dim1!=dim2)
					return 0;
				/* 比较数组的基本类型 */
				struct Type* eletype1 = getArrEleType(type1);
				struct Type* eletype2 = getArrEleType(type2);
				if(typeIsSame(eletype1,eletype2))
				{
					freeType(eletype1);
					freeType(eletype2);
					return 1;
				}
				else
				{
					freeType(eletype1);
					freeType(eletype2);
					return 0;
				}
				break;
			}
		case 2:	/* struct */
			{
				return structureIsSame(type1,type2);
			}
	}
}
int typeIsBasic(struct Type* type,int basic)
{
	/* 函数功能：
	 * 	判断type是否为int型或float型
	 * 参数：
	 * 	type:要被判断的类型
	 * 	basic: 0:INT;1:FLOAT
	 * 返回值：
	 * 	1:是int/float
	 * 	0:不是int/float
	 */
	if(type==NULL)
		return 0;
	if(type->kind!=0)
		return 0;
	if(type->un.basic!=basic)
		return 0;
	return 1;
}

struct Type* getArrEleType(struct Type* type)
{
	/* 函数功能：
	 * 	取得数组类型的基本类型（INT、FLOAT、STRUCT）
	 * 参数：
	 * 	数组类型
	 * 返回值：
	 * 	数组类型的基本类型
	 * 其他说明：
	 * 	返回值申请了新的空间
	 * 	非数组类型将返回原类型
	 */
	struct Type* t_type = type;
	if(type==NULL)
		return NULL;
	while(t_type->kind==1)
	{
		t_type = t_type->un.array.arreletype;
	}
	return copyType(t_type);	//申请了新的空间
}

int structureIsSame(struct Type* struct1,struct Type* struct2)
{
	/* 函数功能：
	 * 	判断两个结构体数据类型是否是结构等价的
	 * 参数：
	 * 	要被判断的两个结构体数据类型
	 * 返回值：
	 * 	0:不等价
	 * 	1:等价
	 * 其他说明：
	 * 	采用结构等价，不会比较结构体名以及结构体中域的变量名是否相等。
	 * 	在typeIsSame()函数中被调用
	 * 	默认两个参都是结构体类型，不会再次检查
	 */

	struct FieldList* fld_lst1;
	struct FieldList* fld_lst2;
	for(	fld_lst1 = struct1->un.structure.struct_field,fld_lst2 = struct2->un.structure.struct_field;
		fld_lst1!=NULL && fld_lst2!=NULL;
		fld_lst1 = fld_lst1->next,fld_lst2 = fld_lst2->next	)
	{
		if(0==typeIsSame(fld_lst1->type,fld_lst2->type))
			return 0;	//如果对应的域有不同类型，则不满足结构等价
	}
	if(fld_lst1==fld_lst2)
		return 1;	//遍历完了两个struct的所有field，都遍历至了NULL，则结构等价
	else
		return 0;	//如果遍历完了一个struct的field而另一个没遍历完，则不结构等价
}
int funIsSame(struct Prop* prop1,struct Prop* prop2)
{
	/* 函数功能：
	 * 	判断两个函数Prop节点的结构是否等价（返回值类型、形参数量、形参类型）
	 * 参数：
	 * 	prop1、prop2：参与比较的两个函数节点
	 * 返回值：
	 * 	0:不等价
	 * 	1:等价
	 * 其他说明：
	 * 	不会比较两个函数的函数名。调用时通常是用于比较两个同名函数的结构是否等价
	 */
	struct FunProp* funprop1 = prop1->un.funprop;
	struct FunProp* funprop2 = prop2->un.funprop;
	if(funprop1->paracnt!=funprop2->paracnt)
	{	
		//参数个数不相等
		return 0;
	}
	if(typeIsSame(funprop1->f_rtn_type,funprop2->f_rtn_type)==0)
	{
		//返回值类型不等价
		return 0;
	}
	int i;
	struct Prop* para1 = funprop1->paraprop;
	struct Prop* para2 = funprop2->paraprop;
	for(i=0;i<funprop1->paracnt;i++)
	{
		if(typeIsSame(para1->un.varprop->type,para2->un.varprop->type)==0)
		{
			//参数类型不同
			return 0;
		}
		para1 = para1->next;
		para2 = para2->next;
	}
	return 1;
}
int funIsRedefined(struct Prop* prop1,struct Prop* prop2)
{
	/* 函数功能：
	 * 	判断两个函数Prop节点的结构是否同是函数定义（判断重定义）
	 * 参数：
	 * 	prop1、prop2：参与比较的两个函数节点
	 * 返回值：
	 * 	0:未重定义
	 * 	1:重定义
	 * 其他说明：
	 * 	不会比较两个函数的函数名。调用时通常是用于比较两个同名函数的结构是否等价
	 * 	判断为重定义时会输出错误信息
	 * 	如果定义在同一行，则不认为是重定义，以此不严格地处理来避免误伤
	 */
	struct FunProp* funprop1 = prop1->un.funprop;
	struct FunProp* funprop2 = prop2->un.funprop;
	if(funprop1->defined!=-1 && funprop2->defined!=-1 && funprop1->defined!=funprop2->defined)
	{
		//两个函数节点都被定义过，则是重定义。输出错误信息
		char info[256]="";
		int conflict_lineno = funprop1->defined;
		int lineno = funprop2->defined;
		
		if(conflict_lineno > lineno)
		{
			//保证conflict_lineno < lineno
			int tmp = conflict_lineno;conflict_lineno = lineno;lineno = tmp;
		}
		sprintf(info,"function \"%s\" is redefined. the previous defination is at line %d",prop1->name,conflict_lineno);
		semanticErrorPrint(4,lineno,info);
		return 1;
	}
	else
	{
		return 0;
	}
}
struct Type* searchTypeStackByStructName(char* name,int out_layer)
{
	/* 函数功能：
	 *	在type栈中根据类型名和定义域查找struct类型（定义过的结构体）
	 * 参数：
	 * 	name:要被查找的数据类型名
	 *	out_layer:如果在栈中查至定义域层数小于out_layer的struct，则认为没查到
	 * 返回值：
	 * 	NULL：没有查到名为name的数据类型的定义
	 * 	否则：指向栈中名为name的数据类型（注意没有申请新的空间）
	 */
	struct Type* p_type = type_stack_top;
	while(p_type!=NULL && p_type->layer >= out_layer)
	{
		if(0 == strcmp(name,p_type->un.structure.struct_name))
		{
			return p_type;
		}
		p_type = p_type->next;
	}
	return NULL;
}
struct Prop* searchPropStackByPropName(char* name,int out_layer,int search_var,int search_fun)
{
	/*
	 * 函数功能：
	 * 	在Prop栈中根据变量/函数名和定义域查找Prop节点
	 * 参数：
	 * 	name: 要查找的变量/函数名
	 * 	out_layer: 如果在栈中查至定义域层数小于out_layer的Prop，则认为没查到
	 * 	search_var: 0:不查找变量；1:查找变量
	 * 	search_fun: 0:不查找函数；1:查找函数
	 * 返回值：
	 * 	NULL：没有查到名为name的变量/函数
	 * 	否则：指向栈中名为name的Prop（注意没有申请新的空间）
	 * 其他说明：
	 * 	要查out_layer对应的这一层
	 */

	struct Prop* p_prop = prop_stack_top;
	while(p_prop!=NULL && p_prop->layer>=out_layer)
	{
		if(search_var==0 && p_prop->kind==0)
		{
			p_prop = p_prop->next;
			continue;	//如果不要求搜变量，则忽略变量
		}
		if(search_fun==0 && p_prop->kind==1)
		{
			p_prop = p_prop->next;
			continue;	//如果不要求搜函数，则忽略函数
		}
	
	
		if(0 == strcmp(name,p_prop->name))
		{

			return p_prop;
		}
		p_prop = p_prop->next;
	}

	return NULL;
}
struct FieldList* linkFieldList(struct FieldList* fldlst_head,struct FieldList* fldlst_tail)
{
	/* 函数功能：
	 * 	把两个FieldList域链表链接成一个链表
	 * 参数：
	 * 	fldlst_head：链接完成后在头部的链表
	 * 	fldlst_tail：链接完成后在尾部的链表
	 * 返回值：
	 * 	完成链接后的链表头
	 * 其他说明：
	 * 	若fldlst_head为空，则直接返回fldlst_tail
	 * 	没有申请新的链表空间，原链表在此函数外层也被链接在了一起
	 */
	if(fldlst_head==NULL)
		return fldlst_tail;
	struct FieldList* p_fldlst = fldlst_head;
	while(p_fldlst->next!=NULL)
	{
		p_fldlst = p_fldlst->next;
	}
	p_fldlst->next = fldlst_tail;
	return fldlst_head;
}
struct FieldList* searchFieldListByName(char* name,struct FieldList* fld_lst)
{
	/* 函数功能：
	 * 	根据name查找fld_lst中的一个域节点
	 * 参数：
	 * 	name：要查找的域的名字
	 * 	fld_lst：在fld_lst这个域链表中查找
	 * 返回值：
	 * 	NULL：没有找到名为name的域
	 * 	否则：指向名为name的域的指针
	 */
	struct FieldList* p_fldlst = fld_lst;
	while(p_fldlst!=NULL)
	{
		if(0==strcmp(name,p_fldlst->name))
		{
			return p_fldlst;
		}
		p_fldlst = p_fldlst->next;
	}
	return NULL;
}
struct FieldList* freeFieldList(struct FieldList* fld_lst,int only_free_node)
{
	/* 函数功能：
	 * 	释放fld_lst所带领的链表空间 或 单独的fld_lst的空间
	 * 参数：
	 * 	fld_lst:要被释放空间的链表头
	 * 	only_free_node: 0:释放整个链表；1:只释放fld_lst节点
	 * 返回值：
	 * 	如果只释放fld_lst节点，则返回指向下一个节点的指针
	 * 	如果释放整个fld_lst链表，则返回NULL
	 * 其他说明：
	 * 	FieldList节点内的空间也被释放了
	 */
	if(fld_lst==NULL)
		return NULL;
	if(only_free_node)
	{
		struct FieldList* rtn_fldlst = fld_lst->next;
		freeType(fld_lst->type);
		free(fld_lst);
		fld_lst = NULL;
		return rtn_fldlst;
	}
	else
	{
		struct FieldList* next_fldlst = fld_lst->next;
		freeType(fld_lst->type);
		free(fld_lst);
		fld_lst = NULL;
		return freeFieldList(next_fldlst,only_free_node);
	}
}
struct Type* freeType(struct Type* type)
{
	/* 函数功能：
	 * 	释放一个Type的空间
	 * 参数：
	 * 	type:要被释放空间的type节点
	 * 返回值：
	 * 	type->next
	 * 其他说明：
	 * 	只释放一个节点的空间。如果type是一个链表，则释放链表头空间，返回链表第二个节点
	 * 	如果type本身就是NULL，则返回NULL
	 */
	if(type==NULL)
		return NULL;
	struct Type* next_type = type->next;
	if(type->kind==2)
	{
		freeFieldList(type->un.structure.struct_field,0);
	}
	free(type);
	type=NULL;
	return next_type;
}
struct Prop* freeProp(struct Prop* prop)
{
	/* 函数功能：
	 * 	释放一个Prop的空间
	 * 参数：
	 * 	prop:要被释放空间的prop节点
	 * 返回值：
	 * 	prop->next
	 * 其他说明：
	 * 	只释放一个节点的空间。如果type是一个链表，则释放链表头空间，返回链表第二个节点
	 * 	如果type本身就是NULL，则返回NULL
	 */
	
	if(prop==NULL)
		return NULL;
	struct Prop* next_prop = prop->next;
	switch(prop->kind)
	{
		case 0:	/* 变量 */
			{
				
				;
				freeType(prop->un.varprop->type);
				free(prop->un.varprop);
				prop->un.varprop = NULL;
				break;
			}
		case 1:	/* 函数 */
			{
				freeType(prop->un.funprop->f_rtn_type);
				struct Prop* paraprop = prop->un.funprop->paraprop;
				
				
				
				while(paraprop!=NULL)
				{
					
					paraprop = freeProp(paraprop);
					
				}
				free(prop->un.funprop);
				prop->un.funprop = NULL;
				break;
			}
	}
	free(prop);
	prop = NULL;
	
	return next_prop;
}
struct Prop* transferFieldToProp(struct FieldList* fld_lst)
{
	/* 函数功能：
	 * 	把FieldList*类型的变量转换成Prop类型的变量
	 * 参数：
	 * 	fld_lst：要被转换的FieldList节点
	 * 返回值：
	 * 	转换后得到的Prop节点
	 * 其他说明：
	 * 	注意，转换的是变量节点，不是函数节点
	 * 	只转换单个节点，不转换链表
	 * 	为返回的Prop申请了新的空间
	 */
	if(fld_lst==NULL)
		return NULL;
	struct Prop* newprop = (struct Prop*)malloc(sizeof(struct Prop));
	newprop->kind = 0;

	newprop->un.varprop = (struct VarProp*)malloc(sizeof(struct VarProp));
	newprop->un.varprop->type = copyType(fld_lst->type);

	newprop->un.varprop->def_lineno = fld_lst->def_lineno;
	strcpy(newprop->name,fld_lst->name);	
	newprop->layer = fld_lst->layer;
	newprop->next = NULL;
	return newprop;
}
struct Type* getAnIntType()
{
	/*
	 * 函数功能：
	 * 	申请一个type空间作为INT型左值类型
	 * 返回值：
	 * 	申请到的空间
	 */
	struct Type* type = (struct Type*)malloc(sizeof(struct Type));
	type->kind = 0;
	type->un.basic = 0;
	type->def_lineno = -1;
	type->layer = -2;
	type->next = NULL;
	return type;
}

/* 函数声明-其他函数 */
void semanticAnalyze(struct Node* root)		/* go through syntax tree and do semantic analysis */
{
	/* 函数功能：
	 * 	驱动整个语法分析过程，遍历整个语法树，遇到ExtDef、Def、Exp、{、} 后调用专门的解析函数
	 */
	if(root==NULL)
		return;
	struct Node* p = root;	//p is now Node* root
	if(strcmp(p->info.name,"ExtDef")==0)
	{
		analyze_node_ExtDef(root);
	}
	else if(strcmp(root->info.name,"Def")==0)
	{
		dbgprt("内部错误：不应该有独立的Def节点。(%s,%d)\r\n",__FILE__,__LINE__);
	}
	else if (strcmp(root->info.name,"Exp")==0)
	{
		analyze_node_Exp(root,0);
	}
	else if(strcmp(root->info.name,"LC")==0)
	{
		analyze_node_LC();
       	}
	else if(strcmp(root->info.name,"RC")==0)
	{
		analyze_node_RC();
	}
	else
	{
		int i;
		for(i=0;i<root->numofchild;i++)
		{
			semanticAnalyze(root->child[i]);
		}
	}
}
void semanticErrorPrint(int type,int lineno,char* info)
{
	/* 函数功能：
	 * 	按照实验要求打印语义错误信息
	 * 参数：
	 * 	type:错误类型
	 * 	lineno:错误所在行号
	 * 	info：附加字符串信息
	 */
	printf("Error type %d at line %d:  %s\r\n",type,lineno,info);
}
int countFunParaList(struct Prop* paraprop)
{
	/* 函数功能：
	 * 	计算函数参数列表的个数
	 * 参数：
	 * 	要被计算个数的形参链表
	 * 返回值：
	 *
	 * 	形参链表里形参的个数
	 */
	int cnt = 0;
	struct Prop* p_prop = paraprop;
	while(p_prop!=NULL)
	{
		p_prop = p_prop->next;
		cnt++;
	}
	return cnt;
}
int findUndefinedFunc()
{
	/* 函数功能：
	 * 	检查未定义的函数，并输出错误信息
	 * 返回值：
	 * 	未定义的函数的个数
	 * 其他说明：
	 * 	通常在整个语义分析的最后调用此函数
	 * 	在prop_stack_top中查找
	 */
	struct Prop* prop = prop_stack_top;
	while(prop!=NULL)
	{
		if(prop->kind==1)
		{
			if(prop->un.funprop->defined==-1)
			{
				char info[256]="";
				int lineno = prop->un.funprop->declared;
				sprintf(info,"function \"%s\" declared at line %d is not defined",prop->name,lineno);
				semanticErrorPrint(18,lineno,info);
			}
		}
		prop=prop->next;
	}
}

/* 函数声明-语义节点属性构造 */
int analyze_node_ExtDef(struct Node* node_ExtDef)
{
	/* 
	 * ExtDef	:	Specifier ExtDecList SEMI (4)
	 *		|	Specifier SEMI (5)
	 * 		|	Specifier FunDec CompSt (6)
	 * 返回值：
	 * 	0:完全没错
	 * 	-1:存在错误
	 */
	int rtn_value=-1;
	switch(node_ExtDef->info.ruleId)
	{
		case 4:	/* ExtDef -> Specifier ExtDecList SEMI (4) */
			{
				struct Type* t_Specifier = analyze_node_Specifier(node_ExtDef->child[0]);
	
				if(t_Specifier==NULL)
				{
					rtn_value = -1;
					break;
				}
				
				rtn_value = analyze_node_ExtDecList(node_ExtDef->child[1],t_Specifier);
				break;
			}
		case 5:	/* ExtDef -> Specifier SEMI (5) */
			{
				struct Type* t_Specifier = analyze_node_Specifier(node_ExtDef->child[0]);
				if(t_Specifier)
				{
					rtn_value = 0;
				}
				else
				{
					rtn_value = -1;
				}
				break;
			}
		case 6:	/* ExtDef -> Specifier FunDec CompSt (6) */
			{
				struct Type* t_Specifier = analyze_node_Specifier(node_ExtDef->child[0]);
				if(t_Specifier==NULL)
				{
					//函数返回值类型出错，不继续分析函数了
					rtn_value = -1;
					break;
				}
			
				struct Prop* p_FunDec = analyze_node_FunDec(node_ExtDef->child[1],2,t_Specifier);
			
				if(p_FunDec==NULL)
				{
					rtn_value = -1;
					break;
				}
				rtn_value =  analyze_node_CompSt(node_ExtDef->child[2],t_Specifier);
				break;
			}
	}
	return rtn_value;
}
struct Type* analyze_node_Specifier(struct Node* node_Specifier)
{
	/*
	 * Specifier	:	TYPE (11)
	 *		|	StructSpecifier	(12)
	 */
	struct Type* rtn_type = NULL;
	switch(node_Specifier->info.ruleId)
	{
		case 11:
			{
	
				rtn_type = analyze_node_TYPE(node_Specifier->child[0]);
				break;
			}
		case 12:
			{
				rtn_type = analyze_node_StructSpecifier(node_Specifier->child[0]);
				break;
			}
	}
	return rtn_type;
}
struct Type* analyze_node_TYPE(struct Node* node_TYPE)
{
	/*
	 * TYPE	is defined in lexical level
	 */
	struct Type* newtype = (struct Type*)malloc(sizeof(struct Type));
	newtype->kind = 0;
	newtype->un.basic = (node_TYPE->info.value.INT==1)?0:1;
	newtype->def_lineno = node_TYPE->info.lineno;
	newtype->layer = semantic_layer;
	newtype->next = NULL;
	return newtype;
}
struct Type* analyze_node_StructSpecifier(struct Node* node_StructSpecifier)
{
	/*
	 * StructSpecifier	:	STRUCT OptTag LC DefList RC (13)
	 *			|	STRUCT Tag (14)
	 * 其他说明：
	 * 	对于合法的StructSpecifier，要入栈type_stack_top
	 */
	struct Type* rtn_type = NULL;

	switch(node_StructSpecifier->info.ruleId)
	{
		case 13:
			{
				char* c_OptTag = analyze_node_OptTag(node_StructSpecifier->child[1]);
				if(c_OptTag==NULL)
				{
					//结构体名重名了（已报错）
					//c_OptTag的值是NULL,不需要free
					rtn_type = NULL;
					break;
				}
				analyze_node_LC();
				struct FieldList* fldlst_sync = analyze_node_DefList(node_StructSpecifier->child[3],1);
				analyze_node_RC();
			
				/*
				 * if(fldlst_sync==NULL)
				 * {
				 * 	 即使结构体内没有合法的域，仍然建立该结构体
				 * 	 只不过结构体内的Type.un.structure.struct_fldlst是NULL而已
				 * }
				 */
				struct Type* newstruct = (struct Type*)malloc(sizeof(struct Type));
				newstruct->kind = 2;
				strcpy(newstruct->un.structure.struct_name,c_OptTag);
				free(c_OptTag);	//已复制完，释放下层OptTag申请的结构体名字符串空间
				c_OptTag = NULL;
				newstruct->un.structure.struct_field = fldlst_sync;
				newstruct->def_lineno = node_StructSpecifier->info.lineno;
				newstruct->layer = semantic_layer;
				newstruct->next = NULL;
				push_type_stack(newstruct);	//完成建立新结构体类型，入栈！
				rtn_type = copyType(newstruct);
				break;
			}
		case 14:
			{
				/* Tag->ID
				 * 非常底层简单，故在本函数内完成
				 * 对未定义的结构体类型名的使用（包括未定义先声明），认为是语义错误
				 */
				struct Node* node_ID = node_StructSpecifier->child[1]->child[0];
				char* name = node_ID->info.notation;
				struct Type* exist_type = searchTypeStackByStructName(name,0);
				if(exist_type==NULL)
				{
					//如果使用了未定义的struct结构体类型，则报错
					char info[256] = "";
					sprintf(info,"struct \"%s\" is used before defination",name);
					int lineno = node_ID->info.lineno;
					semanticErrorPrint(17,lineno,info);
					rtn_type = NULL;
					break;
				}
				else
				{
					rtn_type = exist_type;
					break;
				}
				break;
			}
	}
	return rtn_type;
}
char* analyze_node_OptTag(struct Node* node_OptTag)
{
	/*
	 * OptTag	:	ID (15)
	 * 		|	epsilon （16)
	 * 已经非常底层了，直接由此函数处理完。
	 * 返回值：
	 * 	有名结构体，返回结构体名
	 * 	匿名结构体，返回“#UNNAMED”+编号
	 *	NULL:与已有结构体名或变量名重名 
	 * 其他说明：
	 * 	返回的字符串指针在本函数内申请了空间，建议上层使用完后释放
	 */
	char* rtn_char = NULL;
	switch(node_OptTag->info.ruleId)
	{
		case 15:
			{
				//这里在定义新的有名结构体，需要检查结构体名是否与已有的变量、结构体重名
				struct Node* node_ID = node_OptTag->child[0];
				struct Type* exist_type = searchTypeStackByStructName(node_ID->info.notation,0);
				if(exist_type)
				{
					//与已有的结构体重名
					char info[256]="";
					sprintf(info,"struct \"%s\" has already been defined at line %d",exist_type->un.structure.struct_name,exist_type->def_lineno);
					int lineno = node_OptTag->info.lineno;
					semanticErrorPrint(16,lineno,info);
					return NULL;
				}
				struct Prop* exist_prop = searchPropStackByPropName(node_ID->info.notation,0,1,0);
				if(exist_prop)
				{
					//与已有的变量重名
					char info[256]="";
					sprintf(info,"struct has same name with variable \"%s\" at line %d",exist_prop->name,exist_prop->un.varprop->def_lineno);
					int lineno = node_OptTag->info.lineno;
					semanticErrorPrint(16,lineno,info);
					return NULL;
				}
				//检查完成，没有发现重名，则生成新的名字
				rtn_char = (char*)malloc(sizeof(char)*MAX_ID_LEN);
				strcpy(rtn_char,node_ID->info.notation);
				break;
			}
		case 16:
			{
				rtn_char = (char*)malloc(sizeof(char)*MAX_ID_LEN);
				sprintf(rtn_char,"#UNNAMED%d",unnamed_struct_counter);
				unnamed_struct_counter++;
				break;
			}
	}
	return rtn_char;

}
struct FieldList* analyze_node_DefList(struct Node* node_DefList,int flag)
{
	/*
	 * DefList	:	Def DefList (34)
	 * 		|	epsilon (35)
	 * 参数：
	 * 	flag: 1:是struct中的DefList; 2:是CompSt中的DefList
	 * 返回值：
	 * 	如果是结构体中的DefList，则返回整个FieldList链表
	 * 	如果是CompSt函数块中的DefList，则无需返回值，返回NULL即可
	 * 其他说明：
	 * 	除了DefList -> Def DefList以外，DefList只在CompSt和StructSpecifier中被用到
	 * 	只处理此子树的DefList,父节点和兄弟节点的DefList由上层处理
	 * 	case 1-34中，本层要检查本Def与子DefList二者间的重名问题，把子DefList挂到Def节点之后
	 * 	case 2-34中，在Def子节点里通过符号表检查重名冲突情况。
	 */
	
	
	struct FieldList* rtn_fldlst = NULL;
	switch(flag)
	{
		case 1:
			{
				switch(node_DefList->info.ruleId)
				{
					case 34:
						{
							struct FieldList* fldlst = analyze_node_Def(node_DefList->child[0],flag);
							struct FieldList* fldlst_sync = analyze_node_DefList(node_DefList->child[1],flag);
			
							struct FieldList* p_fldlst = fldlst;
							while(p_fldlst!=NULL)
							{
								struct FieldList* exist_fldlst = searchFieldListByName(p_fldlst->name,fldlst_sync);
			
								if(exist_fldlst)
								{
				
									//如果本层的Def与后面的子DefList域名重复，则忽略本层的Def，返回子DefList
			
									char info[256]="";
									sprintf(info,"field name \"%s\" conflict with field name at line %d. All Def at line %d will be ignored",p_fldlst->name,p_fldlst->def_lineno,p_fldlst->def_lineno);
			
									int lineno = exist_fldlst->def_lineno;
									semanticErrorPrint(15,lineno,info);
									freeFieldList(fldlst,0);
									rtn_fldlst = fldlst_sync;
				
									return rtn_fldlst;
									/* 
									 * 因要跳出while和case才能到达函数末尾，
									 * 要跳两层
									 * 所以就直接在这里提前return了
									 * 破坏了switch-case-break-return模式
									 */
								}
								p_fldlst = p_fldlst->next;
							}
							//本层Def域名没有与后面的子DefList域名重复
							rtn_fldlst = linkFieldList(fldlst,fldlst_sync);
							break;
						}
					case 35:
						{
							
							rtn_fldlst = NULL;
							break;
						}
				}
				break;
			}
		case 2:	/* CompSt中的DefList */
			{
				
				
				switch(node_DefList->info.ruleId)
				{
					case 34:/* DefList -> Def DefList (34) */
						{
						
							struct FieldList* f_Def = analyze_node_Def(node_DefList->child[0],flag);
						
							analyze_node_DefList(node_DefList->child[1],flag);
							break;
						}
					case 35:/* DefList -> epsilon (35) */
						{
							break;
						}
				}
				
				break;
			}
	}
	return rtn_fldlst;
}
struct FieldList* analyze_node_Def(struct Node* node_Def,int flag)
{

	/*
	 * Def	:	Specifier DecList SEMI (36)
	 * 参数：
	 * 	flag: 1:是struct中的Def; 2:是CompSt中的Def
	 * 返回值：
	 * 	如果是结构体中的一个DefList里的Def，则返回整个FieldList链表
	 * 	如果是CompSt函数块中的DefList，则无需返回值，返回NULL即可
	 * 其他说明：
	 * 	除了DecList -> Dec COMMA DecList以外，DecList只在CompSt和StructSpecifier中的DefList里被用到
	 * 	此函数只负责本子树的Def,对父节点和兄弟节点的DefList不作处理，由上层处理。
	 * 	如果是flag=2，则由子DefClist通过符号表来处理重名问题
	 */
	struct FieldList* rtn_fldlst = NULL;
	switch(flag)
	{
		case 1:	/* struct结构体内的Def */
			{
				switch(node_Def->info.ruleId)
				{
					case 36:
						{
							struct Type* t_Specifier = analyze_node_Specifier(node_Def->child[0]);
							if(t_Specifier==NULL)
							{
								rtn_fldlst =  NULL;	//结构体的域的数据类型有问题，这个域就无法实现。故不把这个域加到与链表中。
								break;
							}
							rtn_fldlst = analyze_node_DecList(node_Def->child[1],flag,t_Specifier);
							break;
						}
				}
				break;	
			}
		case 2:	/* CompSt函数块内的Def */
			{
				/* Def -> Specifier DecList SEMI (36) */
				switch(node_Def->info.ruleId)
				{
					case 36:
						{
							struct Type* t_Specifier = analyze_node_Specifier(node_Def->child[0]);
					if(t_Specifier->kind==2)
					{
						
						
						
					}
							if(t_Specifier==NULL)
							{
								//类型有问题，无法继续处理
								break;
							}
							//类型没问题，处理DecList
							//由DecList通过符号表解决重名问题
							analyze_node_DecList(node_Def->child[1],flag,t_Specifier);
							break;
						}
				}
				break;
			}
	}
	return rtn_fldlst;
}
struct FieldList* analyze_node_DecList(struct Node* node_DecList,int flag,struct Type* t_Specifier)
{
	/*
	 * DecList	:	Dec (37)
	 * 		|	Dec COMMA DecList (38)
	 * 参数：
	 * 	flag: 1:是struct中的DecList; 2:是CompSt中的DecList
	 * 返回值：
	 * 	如果是结构体中的一个DefList的DecList里的Dec，则返回本Dec中的FieldList链表（它们是Specifier都相同的变量或数组)
	 * 	如果是CompSt函数块中的DefList，则无需返回值，返回NULL即可
	 * 其他说明：
	 * 	当DecList在结构体内时，为了避免此函数的工作量过大，之前定义好的域(FieldList)就不再交由此函数处理了，而是由上层的DecList节点处理
	 * 	当DecList在CompSt中时，由Dec通过符号表栈来处理重名问题
	 */
	struct FieldList* rtn_fldlst = NULL;
	switch(flag)
	{
		case 1:
			{
				switch(node_DecList->info.ruleId)
				{
					case 37:
						{
							rtn_fldlst = analyze_node_Dec(node_DecList->child[0],flag,t_Specifier);
							break;
						}
					case 38:
						{
							struct FieldList* this_fld;	//本层处理的Dec
						       	struct FieldList* fldlst_sync;	//子DecList返回的域链表
							this_fld = analyze_node_Dec(node_DecList->child[0],flag,t_Specifier);
							fldlst_sync = analyze_node_DecList(node_DecList->child[2],flag,t_Specifier);
							if(this_fld==NULL)
							{
								//如果本层的Dec是非法的，则只返回后面的DefList
								rtn_fldlst = fldlst_sync;
								break;
							}
							char* this_fld_name = this_fld->name;
							struct FieldList* rename_fld = searchFieldListByName(this_fld_name,fldlst_sync);
							if(rename_fld)
							{
								//如果本层Dec与后面的子DecList重名,则不要本层的
								char info[256]="";
								sprintf(info,"field name \"%s\" conflicts with field at line %d",this_fld_name,this_fld->def_lineno);
								int lineno = rename_fld->def_lineno;
								semanticErrorPrint(15,lineno,info);
								freeFieldList(this_fld,0);
								rtn_fldlst = fldlst_sync;
								break;
							}
							rtn_fldlst = linkFieldList(this_fld,fldlst_sync);
							break;
						}
				}
				break;
			}
		case 2:
			{
				switch(node_DecList->info.ruleId)
				{
					case 37:/* DecList -> Dec (37) */
						{
							analyze_node_Dec(node_DecList->child[0],flag,t_Specifier);
							break;
						}
					case 38:/* DecList -> Dec COMMA DecList (38) */
						{
							analyze_node_Dec(node_DecList->child[0],flag,t_Specifier);
							analyze_node_DecList(node_DecList->child[2],flag,t_Specifier);
							break;
						}
				}
				break;
			}
	}
	return rtn_fldlst;
}
struct FieldList* analyze_node_Dec(struct Node* node_Dec,int flag,struct Type* t_Specifier)
{
	/*
	 * Dec	:	VarDec (41)
	 * 	|	VarDec ASSIGNOP Exp (42)
	 * 参数：
	 * 	flag: 1:是struct中的Dec; 2:是CompSt中的Dec
	 * 返回值：
	 * 	如果是结构体中的一个DefList的DecList里的Dec，则返回本Dec中的FieldList链表节点。
	 * 	如果是CompSt函数块中的Def，则无需返回值，返回NULL即可
	 * 其他说明：
	 * 	如果是CompSt函数快中的Def，需要做类型检查、变量名合法性检查等
	 */
	struct FieldList* rtn_fldlst = NULL;
	switch(flag)
	{
		case 1:
			{
				switch(node_Dec->info.ruleId)
				{
					case 41:
						{
							rtn_fldlst = analyze_node_VarDec(node_Dec->child[0],1,t_Specifier);
							break;
						}
					case 42:
						{
							//在结构体中对域变量赋值是非法的，要报错
							char info[256] = "";
							sprintf(info,"field is initinalized");
							int lineno = node_Dec->info.lineno;
							semanticErrorPrint(15,lineno,info);
							rtn_fldlst = NULL;
							break;
						}
				}
				break;
			}
		case 2:
			{
				char* name = NULL;
				switch(node_Dec->info.ruleId)
				{
					case 42:/* Dec -> VarDec ASSIGNOP Exp (42) */
					case 41: /* Dec -> VarDec (41) */
						{
							struct FieldList* f_VarDec = analyze_node_VarDec(node_Dec->child[0],flag,t_Specifier);
							if(f_VarDec==NULL)
							{
								rtn_fldlst = NULL;
								break;
							}
							if(t_Specifier==NULL)
							{
								//如果是使用变量而不是声明/定义变量，则查符号表
								name = f_VarDec->name;
								struct Prop* exist_prop = searchPropStackByPropName(name,0,1,0);
								if(exist_prop==NULL)
								{
									//使用了未定义的变量
									char info[256] = "";
									sprintf(info,"undefined variable \"%s\"",name);
									int lineno = f_VarDec->def_lineno;
									semanticErrorPrint(1,lineno,info);
									rtn_fldlst = NULL;
									//freeFieldList(f_VarDec)有风险！暂不free
									break;
								}
							}
							else
							{
								//在定义新的变量，要做变量名合法性检查
								name = f_VarDec->name;
								struct Prop* exist_prop = searchPropStackByPropName(name,semantic_layer,1,0);
							
							
								struct Type* exist_type = searchTypeStackByStructName(name,0);
								if(exist_prop)
								{
									//果然重定义了
									char info[256] = "";
									sprintf(info,"redefination of variable \"%s\". see previous one at line %d",name,exist_prop->un.varprop->def_lineno);
									int lineno = node_Dec->info.lineno;
									semanticErrorPrint(3,lineno,info);
									rtn_fldlst = NULL;
									break;
								}
								else if(exist_type)
								{
									//果然与结构体名重名了
									char info[256] = "";
									sprintf(info,"\"%s\" is a name of struct type at line %d",name,exist_type->def_lineno);
									int lineno = node_Dec->info.lineno;
									semanticErrorPrint(3,lineno,info);
									rtn_fldlst = NULL;
									break;
								}
								else
								{
									//生成新的变量
									struct Prop* newprop = transferFieldToProp(f_VarDec);
									push_prop_stack(newprop);
									rtn_fldlst = f_VarDec;
									if(node_Dec->info.ruleId==41)
										break;
								}
							}
							if(node_Dec->info.ruleId==41)
								break;
						}
					case 9999:/* 对合法的变量赋值 */
						{
							struct Prop* lft_prop = searchPropStackByPropName(name,0,1,0);
							struct Type* lft_type = lft_prop->un.varprop->type;
							struct Type* rit_type = analyze_node_Exp(node_Dec->child[2],0);
							if(typeIsSame(lft_type,rit_type)==0)
							{
								char info[256] = "assignment type conflict";
								int lineno = node_Dec->child[1]->info.lineno;
								semanticErrorPrint(5,lineno,info);
							}
							break;
						}
				}
				break;
			}
	}
	return rtn_fldlst;
}
struct FieldList* analyze_node_VarDec(struct Node* node_VarDec,int flag,struct Type* t_Specifier)
{
	/*
	 * VarDec	:	ID (18)
	 *	 	|	VarDec LB INT RB (19)
	 * 函数功能：
	 * 	1.处理结构体定义中域的VarDec
	 * 	2.处理外部变量声明的VarDec
	 * 	3.处理函数声明/定义中的VarDec（与1一样）
	 * 	4.处理函数块中的VarDec
	 * 参数：
	 * 	flag:
	 *		1:在结构体定义中域的VarDec;
	 *		2:在外部变量声明中的VarDec;
	 *		3:在函数声明/定义中的VarDec
	 *		4.在函数快CompSt中的VarDec
	 * 其他说明：
	 * 	如果是结构体域，名称是否域内重名由上层检查。此函数不检查名称重名。
	 * 	如果是外部声明，名称是否与已有变量名、结构体名重名，由本层本函数检查。
	 * 	如果是函数声明/定义，名称的语义合法性由上层函数检查。此函数不检查名称重名。
	 * 	目前认为flag=1和flag=3时本函数的动作是一样的。
	 * 	flag=1,3的情况的处理都不涉及layer
	 */
	struct FieldList* rtn_fldlst = NULL;
	switch(flag)
	{
		case 3:	/* 函数声明/定义中的VarDec，与case 1动作一样 */
		case 1:	/* 结构体定义中的VarDec */
			{
				switch(node_VarDec->info.ruleId)
				{
					case 18:
						{
							struct Node* node_ID = node_VarDec->child[0];
							rtn_fldlst = (struct FieldList*)malloc(sizeof(struct FieldList));
							strcpy(rtn_fldlst->name,node_ID->info.notation);
							rtn_fldlst->type = copyType(t_Specifier);
							rtn_fldlst->def_lineno = node_ID->info.lineno;
							rtn_fldlst->next = NULL;
							rtn_fldlst->layer = semantic_layer;
							break;
						}
					case 19:
						{
							struct Type* t_mytype = (struct Type*)malloc(sizeof(struct Type));
							t_mytype->kind = 1;
							t_mytype->un.array.arreletype = copyType(t_Specifier);
							t_mytype->un.array.arrsize = node_VarDec->child[2]->info.value.INT;
							t_mytype->def_lineno = t_Specifier->def_lineno;
							t_mytype->layer = t_Specifier->layer;
							t_mytype->next = NULL;
							rtn_fldlst = analyze_node_VarDec(node_VarDec->child[0],flag,t_mytype);
							freeType(t_mytype);
							break;
						}
				}
				break;
			}
		case 4:	/* 在函数块CompSt中的VarDec，与case 2一样 */
		case 2:	/* 外部声明中的VarDec */
			{
				switch(node_VarDec->info.ruleId)
				{
					case 18:
						{
							struct Node* node_ID = node_VarDec->child[0];
							char* name = node_ID->info.notation;
							//检查是否与结构体重名
							struct Type* exist_type = searchTypeStackByStructName(name,0);
							if(exist_type)
							{
								
								//与结构体重名
								char info[256]="";
								sprintf(info,"variable name conflict with struct %s at line %d",exist_type->un.structure.struct_name,exist_type->def_lineno);
								int lineno = node_ID->info.lineno;
								semanticErrorPrint(3,lineno,info);
								rtn_fldlst = NULL;
								
								break;
							}
							//检查是否与已有变量重名（只在当前{}定义域内查）
							struct Prop* exist_prop = searchPropStackByPropName(name,semantic_layer,1,0);
							if(exist_prop)
							{
								//与变量重名
								char info[256]="";
								sprintf(info,"redefination of variable \"%s\". see previous one at line %d",exist_prop->name,exist_prop->un.varprop->def_lineno);
								int lineno = node_ID->info.lineno;
								semanticErrorPrint(3,lineno,info);
								rtn_fldlst = NULL;
								break;
							}
							rtn_fldlst = (struct FieldList*)malloc(sizeof(struct FieldList));
							strcpy(rtn_fldlst->name,name);
							rtn_fldlst->type = copyType(t_Specifier);
							rtn_fldlst->def_lineno = node_ID->info.lineno;
							rtn_fldlst->next = NULL;
							rtn_fldlst->layer = semantic_layer;
							break;
						}
					case 19:
						{
							struct Type* t_mytype = (struct Type*)malloc(sizeof(struct Type));
							t_mytype->kind = 1;
							t_mytype->un.array.arreletype = copyType(t_Specifier);
							t_mytype->un.array.arrsize = node_VarDec->child[2]->info.value.INT;
							t_mytype->def_lineno = t_Specifier->def_lineno;
							t_mytype->layer = t_Specifier->layer;
							t_mytype->next = NULL;
							rtn_fldlst = analyze_node_VarDec(node_VarDec->child[0],flag,t_mytype);
							freeType(t_mytype);
							break;
						}
				}
				break;
			}
	}

	return rtn_fldlst;
}
int analyze_node_ExtDecList(struct Node* node_ExtDecList,struct Type* t_Specifier)
{
	/*
	 * ExtDecList	:	VarDec (7)
	 * 		|	VarDec COMMA ExtDecList (8)
	 * 	 	|	FunDec (9)
	 *  		|	FunDec COMMA ExtDecList (10)
	 * 返回值：
	 * 	0: 正常；-1:异常
	 */
	int rtn_value = -1;
	

	switch(node_ExtDecList->info.ruleId)
	{
		case 7:	/* ExtDecList -> VarDec (7) */
			{
				struct FieldList* fldlst = analyze_node_VarDec(node_ExtDecList->child[0],2,t_Specifier);
				if(fldlst==NULL)
				{
					//如果VarDec变量名非法，则返回-1
					rtn_value=-1;
					break;
				}
				struct Prop* prop = transferFieldToProp(fldlst);
				freeFieldList(fldlst,0);
				push_prop_stack(prop);
				rtn_value = 0;
				break;				
			}
		case 8:	/* ExtDecList -> VarDec COMMA ExtDecList (8) */
			{
				struct FieldList* fldlst = analyze_node_VarDec(node_ExtDecList->child[0],2,t_Specifier);
				if(fldlst==NULL)
				{
					//如果本VarDec变量名非法，则继续分析子ExtDecList，但必定返回-1
					analyze_node_ExtDecList(node_ExtDecList->child[2],t_Specifier);
					rtn_value = -1;
				}
				else
				{
					//如果本VarDec变量名合法，则生成相应的Prop，入栈
					//(不需与子ExtDecList串成链表)
					struct Prop* prop = transferFieldToProp(fldlst);
					freeFieldList(fldlst,0);
					push_prop_stack(prop);
					rtn_value = analyze_node_ExtDecList(node_ExtDecList->child[2],t_Specifier);
				}
				break;				
			}
		case 9:	/* ExtDecList -> FunDec (9) */
			{
				
				
				struct Prop* p_FunDec = analyze_node_FunDec(node_ExtDecList->child[0],1,t_Specifier);
				
				if(p_FunDec)
				{
					rtn_value = 1;
				}
				else
				{
					rtn_value = 0;
				}
				
				break;
			}
		case 10: /* ExtDecList -> FunDec COMMA ExtDecList (10) */
			{
				if(NULL==analyze_node_FunDec(node_ExtDecList->child[0],1,t_Specifier))
				{
					//即使此FunDec有错，仍然继续分析子ExtDecList，但本层返回值设为-1
					analyze_node_ExtDecList(node_ExtDecList->child[2],t_Specifier);//返回值为int(0/-1)
					rtn_value = -1;
				}
				else
				{
					//如果此FunDec没错，就看接下来的子ExtDecList有没有错了
					if(-1==analyze_node_ExtDecList(node_ExtDecList->child[2],t_Specifier))
					{
						rtn_value = -1;
					}
					else
					{
						rtn_value = 0;
					}
				}
			}
	}
	return rtn_value;
}
struct Prop* analyze_node_FunDec(struct Node* node_FunDec,int flag,struct Type* t_Specifier)
{
	/* 
	 * FunDec	:	ID LP VarList RP (20)
	 * 		|	ID LP RP (21)
	 * 参数：
	 * 	flag:
	 * 		1:在外部【声明】函数
	 * 		2:在外部【定义】函数
	 * 	t_Specifier: 函数的返回值类型
	 * 返回值：
	 * 	函数一个节点Prop* rtn_prop(已加入符号表)
	 * 	遇到错误（再议）返回NULL
	 * 其他说明：
	 * 	若是函数声明，则在case 20中退出函数声明时要把放入符号表中的形参弹出
	 */
	struct Prop* rtn_prop = NULL;
	switch(flag)
	{
		case 1:	/* 外部函数声明 */
		case 2:	/* 外部函数定义 */
			/*
			 * 这部分原本是为case 1写的。
			 * 后来发现，case 2与case 1的差别并不大，
			 * 不同之处有3点：
			 * 	在内层的case 20中(有形参的函数定义/声明)，要在何处把形参从符号表中弹栈
			 * 	在内层的case 20、21中，对新声明/定义的函数，funprop-<declared和funprop->defined的值不同
			 *
			 * 故合并，并在内部有一处if(flag==1)用于区分
			 */
			{
				switch(node_FunDec->info.ruleId)
				{
					case 20:/* 含参函数 */
						{
							/* ID 太底层简单了，就在本层处理了 */
							struct Node* node_ID = node_FunDec->child[0];
							char* name = node_ID->info.notation;
							//构造函数节点
							struct Prop* p_FunDec = (struct Prop*)malloc(sizeof(struct Prop));
							p_FunDec->kind = 1;
							p_FunDec->un.funprop = (struct FunProp*)malloc(sizeof(struct FunProp));
							p_FunDec->un.funprop->f_rtn_type = copyType(t_Specifier);
							
							semantic_layer++;	//（
							p_FunDec->un.funprop->paraprop = analyze_node_VarList(node_FunDec->child[2],1);
							semantic_layer--;	//）
							p_FunDec->un.funprop->paracnt = countFunParaList(p_FunDec->un.funprop->paraprop);
							if(flag==1)
							{
								//如果是函数声明
								p_FunDec->un.funprop->declared = node_ID->info.lineno;
								p_FunDec->un.funprop->defined = -1;
							}
							else
							{
								//如果是函数定义
								p_FunDec->un.funprop->declared = -1;
								p_FunDec->un.funprop->defined = node_ID->info.lineno;
							}
							strcpy(p_FunDec->name,name);
							p_FunDec->layer = semantic_layer;	//注意，无需加1
							p_FunDec->next = NULL;
							if(flag==1)
							{
								/*
								 * 如果是函数声明而非函数定义，
								 * 那么至此已经把函数形参的合法性分析完了，
								 * 且已在函数para->funpara中对形参的数据类型做了记录
								 * 所以就不需要再用符号表栈中的这些形参了。
								 * 又因为函数定义完后，不会有右花括号符号帮助释放栈，
								 * 所以需要在此专门弹栈。
								 * 而函数定义中，函数体需要这些形参定义，
								 * 故不能在此弹栈。
								 */

								/*
								 * 为适应Lab3，所有符号的定义域为全局，不再弹栈
								 
								pop_prop_stack(semantic_layer);	//把形参从符号表中弹出，符号表回归正常

								 */
							}
							//查找是否已有同名函数
							struct Prop* exist_prop = searchPropStackByPropName(name,0,0,1);
							if(exist_prop)
							{
								//如果存在一个同名的函数，则检查两个函数是否重【定义】、是否等价
								if(funIsRedefined(p_FunDec,exist_prop))
								{
									//如果重定义（已报错），则无需再加入新的函数
									//释放心函数的空间，返回NULL
									freeProp(p_FunDec);
									rtn_prop = NULL;
								}
								else if(funIsSame(p_FunDec,exist_prop))
								{
									//如果等价，则无需再加入新的函数
									//释放新函数的空间，返回NULL
									freeProp(p_FunDec);
(p_FunDec->un.funprop->paraprop);
									exist_prop->un.funprop->defined = node_ID->info.lineno;
									rtn_prop = NULL;
									/*
									 * 其实没必要用funIsRedefined()来判断的，
									 * 已知flag，
									 * 还可以取到exist_prop->funprop->declared
									 * 直接可以判断
									 */
								}
								else
								{
									//如果不等价，则报错，释放空间，并返回NULL
									char info[256] = "";
									char dec_or_def[15] = "";
									int conflict_lineno = -1;
									if(exist_prop->un.funprop->declared!=-1)
									{
										conflict_lineno = exist_prop->un.funprop->declared;
										strcpy(dec_or_def,"declaration");
									}
									else if(exist_prop->un.funprop->defined!=-1)
									{
										conflict_lineno = exist_prop->un.funprop->defined;
										strcpy(dec_or_def,"defination");
									}
									sprintf(info,"declaration of function \"%s\" has type-conflict with previous %s at line %d",name,dec_or_def,conflict_lineno);
									int lineno = node_ID->info.lineno;
									semanticErrorPrint(19,lineno,info);
							
									freeProp(p_FunDec);
							
									rtn_prop = NULL;
								}
							}
							else
							{
								//如果没有重名函数，则放入符号表中
								/* 注意，
								 * 因形参的作用域比函数名要深一层，
								 * 故要先弹栈，弹出已在VarDecList中压入栈的形参，
								 * 放进函数节点，
								 * 然后重新压如形参！
								 * （但如果只是函数声明，就不能再把形参压进去！！！）
								 */
								/* 为适应Lab3，不再弹栈。
								 * 所有变量/函数的作用于都是全局的
								 * 栈中layer不单调也无所谓
								 
								pop_prop_stack(semantic_layer);
								pop_type_stack(semantic_layer);		//防止参数内进行过了结构体定义

								 */
								push_prop_stack(p_FunDec);
						
								if(flag==2)
								{
									/* 如果是函数定义，则通过再次调用a_n_VarList()来把刚刚弹出来的形参压回去 */
									semantic_layer++;
								/*
								 * 为了Lab3，既然不弹栈，那也就不用再次入了。
								 * 	analyze_node_VarList(node_FunDec->child[2],2);
								 */
									semantic_layer--;
								}
								rtn_prop = p_FunDec;
							}
							break;
						}
					case 21:/* 无参数函数 */
						{
							/* ID 太底层简单了，就在本层处理了 */
							struct Node* node_ID = node_FunDec->child[0];
							char* name = node_ID->info.notation;
							//构造函数节点
							struct Prop* p_FunDec = (struct Prop*)malloc(sizeof(struct Prop));
							p_FunDec->kind = 1;
							p_FunDec->un.funprop = (struct FunProp*)malloc(sizeof(struct FunProp));
							p_FunDec->un.funprop->f_rtn_type = copyType(t_Specifier);
							
							p_FunDec->un.funprop->paraprop = NULL;	//无参数
							p_FunDec->un.funprop->paracnt = 0;	//无参数
							if(flag==1)
							{
								//如果是函数声明
								p_FunDec->un.funprop->declared = node_ID->info.lineno;
								p_FunDec->un.funprop->defined = -1;
							}
							else
							{
								
								
								//如果是函数定义
								p_FunDec->un.funprop->declared = -1;
								p_FunDec->un.funprop->defined = node_ID->info.lineno;
								
							}
							strcpy(p_FunDec->name,name);
							p_FunDec->layer = semantic_layer;	//注意，无需加1
							p_FunDec->next = NULL;
							//查找是否已有同名函数
							struct Prop* exist_prop = searchPropStackByPropName(name,0,0,1);
							if(exist_prop)
							{
								//如果存在一个同名的函数，则检查两个函数是否重【定义】、是否等价
								if(funIsRedefined(p_FunDec,exist_prop))
								{
									//如果重定义（已报错），则无需再加入新的函数
									//释放心函数的空间，返回NULL
									freeProp(p_FunDec);
									rtn_prop = NULL;
									/*
									 * 其实没必要用funIsRedefined()来判断的，
									 * 已知flag，
									 * 还可以取到exist_prop->funprop->declared
									 * 直接可以判断
									 */
								}
								else if(funIsSame(p_FunDec,exist_prop))
								{
									//如果等价，则无需再加入新的函数
									//释放新函数的空间，返回NULL
									freeProp(p_FunDec);
									rtn_prop = NULL;
								}
								else
								{
									//如果不等价，则报错，释放空间，并返回NULL
									char info[256] = "";
									char dec_or_def[15] = "";
									int conflict_lineno = -1;
									if(exist_prop->un.funprop->declared!=-1)
									{
										conflict_lineno = exist_prop->un.funprop->declared;
										strcpy(dec_or_def,"declaration");
									}
									else if(exist_prop->un.funprop->defined!=-1)
									{
										conflict_lineno = exist_prop->un.funprop->defined;
										strcpy(dec_or_def,"defination");
									}
									sprintf(info,"declaration of function \"%s\" has type-conflict with previous %s at line %d",name,dec_or_def,conflict_lineno);
									int lineno = node_ID->info.lineno;
									semanticErrorPrint(19,lineno,info);
									freeProp(p_FunDec);
									rtn_prop = NULL;
								}
							}
							else
							{
								//如果没有重名函数，则放入符号表中
								push_prop_stack(p_FunDec);
								rtn_prop = p_FunDec;
							}
							break;
						}
				}
			}
	}
	return rtn_prop;
}
struct Prop* analyze_node_VarList(struct Node* node_VarList,int flag)
{
	/* VarList	:	ParamDec COMMA VarList (22)
	 * 		|	ParamDec (23)
	 * 参数：
	 * 	flag:
	 * 		1:函数声明中的参数列表（记录形参名）
	 * 		2:函数定义中的参数列表（记录形参名，由符号表记录形参）
	 * 返回值：
	 * 	参数列表的【链表】
	 * 其他说明：
	 * 	函数声明时，不对形参名的重定义做检查。
	 * 	另参见参数flag说明。
	 */
	struct Prop* rtn_prop = NULL;
	switch(flag)
	{
		case 1:
		case 2:	/* 这部分起初是为了case 1写的。后来发现，case 2似乎与case 1没有任何差别 */
			{
				switch(node_VarList->info.ruleId)
				{
					case 22:
						{
							struct Prop* p_ParamDec = analyze_node_ParamDec(node_VarList->child[0]);
							if(p_ParamDec==NULL)
							{
								//如果第一个ParamDec有错，则忽略之并只返回后续形参
								rtn_prop = analyze_node_VarList(node_VarList->child[2],flag);
								break;
							}
							else
							{
								//如果第一个ParamDec没错，则【复制】作为函数形参，并把后续形参链表串在其后
								rtn_prop = copyVarProp(p_ParamDec);
								rtn_prop->next = analyze_node_VarList(node_VarList->child[2],flag);
								break;
							}

						}
					case 23:
						{
							struct Prop* p_ParamDec = analyze_node_ParamDec(node_VarList->child[0]);
							if(p_ParamDec==NULL)
							{
								//如果ParamDec有错，则忽略之并返回NULL
								rtn_prop = NULL;
							}
							else
							{
								//如果ParamDec没错，则【复制】作为函数形参，并返回
								rtn_prop = copyVarProp(p_ParamDec);
							}
							break;
						}
				}
				break;
			}
	}
	return rtn_prop;
}
struct Prop* analyze_node_ParamDec(struct Node* node_ParamDec)
{
	/* ParamDec	:	Specifier VarDec (24)
	 * 其他说明：
	 * 	无论是在函数定义还是函数声明中被调用，都会记录形参的变量名
	 * 	此函数会进行变量名合法性检查，并在对参数layer做特殊处理后把生成的Prop加入prop_stack_top中
	 */
	struct Prop* rtn_prop = NULL;
	switch(node_ParamDec->info.ruleId)
	{
		case 24:
			{
				struct Type* t_Specifier = analyze_node_Specifier(node_ParamDec->child[0]);
				if(t_Specifier==NULL)
				{
					//参数类型出现异常，无法构造完整形参
					freeType(t_Specifier);
					rtn_prop = NULL;
					break;
				}
				struct FieldList* fld_lst = analyze_node_VarDec(node_ParamDec->child[1],3,t_Specifier);
				if(fld_lst==NULL)
				{
					rtn_prop = NULL;
					break;
				}
				rtn_prop = transferFieldToProp(fld_lst);
				freeFieldList(fld_lst,0);
				rtn_prop->layer = semantic_layer;	//为形参的定义域做特别设定查
				//检查变量合法性
				char* name = rtn_prop->name;
				//先检查是否与本函数内已有的形参名重名
				struct Prop* exist_prop = searchPropStackByPropName(name,semantic_layer,1,0);
				if(exist_prop)
				{
					//与本函数内已有的形参名重名
					char info[256]="";
					sprintf(info,"redefination of function parameter \"%s\"",name);
					int lineno = rtn_prop->un.varprop->def_lineno;
					semanticErrorPrint(3,lineno,info);
					freeProp(rtn_prop);
					rtn_prop = NULL;
					break;
				}
				//再检查是否与已有的结构体名重名(全局检查）
				struct Type* exist_type = searchTypeStackByStructName(name,0);
				if(exist_type)
				{
					//与已有的结构体重名
					char info[256]="";
					sprintf(info,"parameter name conflict with struct \"%s\" at line %d",name,exist_type->def_lineno);
					int lineno = rtn_prop->un.varprop->def_lineno;
					semanticErrorPrint(3,lineno,info);
					freeProp(rtn_prop);
					rtn_prop = NULL;
					break;
				}
				//至此，形参名合法。加入prop_stack_top中
				push_prop_stack(rtn_prop);
				break;
			}
	}
	return rtn_prop;
}
int analyze_node_CompSt(struct Node* node_CompSt,struct Type* t_Specifier)
{
	/*
	 * CompSt	:	LC DefList StmtList RC (25)
	 * 参数：
	 * 	t_Specifier:
	 * 		如果CompSt是整个函数体，则t_Specifier为返回值类型
	 * 		如果CompSt只是一般的程序块，则t_Specifier为NULL
	 * 返回值：
	 * 	0:StmtList部分没错误
	 * 	-1:StmtList部分有错误
	 * 其他说明：
	 * 	由于起初对DefList的返回值类型设计错误，导致DefList对于CompSt只返回NULL，故无法查看DefList错误情况
	 */
	int rtn_value = -1;
	switch(node_CompSt->info.ruleId)
	{
		case 25:
			{
				analyze_node_LC();
	
				struct FieldList* f_fldlst = analyze_node_DefList(node_CompSt->child[1],2);
				rtn_value = analyze_node_StmtList(node_CompSt->child[2],t_Specifier);
				analyze_node_RC();
				break;
			}
	}
	return rtn_value;
}
int analyze_node_StmtList(struct Node* node_StmtList,struct Type* t_Specifier)
{
	/* StmtList	:	Stmt StmtList (26)
	 *		|	epsilon (27)
	 */
	int rtn_value = -1;
	switch(node_StmtList->info.ruleId)
	{
		case 26:
			{
				rtn_value = analyze_node_Stmt(node_StmtList->child[0],t_Specifier);
				rtn_value += analyze_node_StmtList(node_StmtList->child[1],t_Specifier);
				rtn_value = rtn_value<0?-1:0;
				break;
			}
		case 27:
			{
				rtn_value = 0;
				break;
			}
	}
	return rtn_value;
}
int analyze_node_Stmt(struct Node* node_Stmt,struct Type* t_Specifier)
{
	/*
	 * Stmt	:	Exp SEMI (28)
	 *    	|	CompSt (29)
	 * 	|	RETURN Exp SEMI (30)
	 * 	|	IF LP Exp RP Stmt (31)
	 * 	|	IF LP Exp RP Stmt ELSE Stmt (32)
	 * 	|	WHILE LP Exp RP Stmt (33)
	 */
	int lineno = node_Stmt->info.lineno;
	switch(node_Stmt->info.ruleId)
	{
		case 28:
			{
				analyze_node_Exp(node_Stmt->child[0],0);
				break;
			}
		case 29:
			{
				analyze_node_CompSt(node_Stmt->child[0],t_Specifier);
				break;
			}
		case 30:
			{
				struct Type* rtn_type = analyze_node_Exp(node_Stmt->child[1],0);
				if(typeIsSame(t_Specifier,rtn_type)==0)
				{
					char info[256] = "return type mismatched with function type";
					int lineno = node_Stmt->child[0]->info.lineno;
					semanticErrorPrint(8,lineno,info);
				}
				break;
			}
		case 31:
			{
				struct Type* t_Exp = analyze_node_Exp(node_Stmt->child[2],0);
				if(typeIsBasic(t_Exp,0)==0)
				{
					char info[256] = "condition expression of \"if\" is not int";
					semanticErrorPrint(7,lineno,info);
					break;
				}
				analyze_node_Stmt(node_Stmt->child[4],t_Specifier);
				break;
			}
		case 32:
			{
				struct Type* t_Exp = analyze_node_Exp(node_Stmt->child[2],0);
				if(typeIsBasic(t_Exp,0)==0)
				{
					char info[256] = "condition expression of \"if\" is not int";
					semanticErrorPrint(7,lineno,info);
					break;
				}
				analyze_node_Stmt(node_Stmt->child[4],t_Specifier);
				analyze_node_Stmt(node_Stmt->child[6],t_Specifier);
				break;
			}
		case 33:
			{
				struct Type* t_Exp = analyze_node_Exp(node_Stmt->child[2],0);
				if(typeIsBasic(t_Exp,0)==0)
				{
					char info[256] = "condition expression of \"while\" is not int";
					semanticErrorPrint(7,lineno,info);
					break;
				}
				analyze_node_Stmt(node_Stmt->child[4],t_Specifier);
				break;
			}
	}
	return 0;
}
struct Type* analyze_node_Exp(struct Node* node_Exp,int dim)
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
	 * 参数：
	 * 	dim: Exp->Exp[Exp]用dim向下传送已发现的维数信息
	*/
	struct Type* rtn_type = NULL;
	int lineno = node_Exp->info.lineno;
	switch(node_Exp->info.ruleId)
	{
		case 43:
			{
				//判断左值
				int left_ruleId = node_Exp->child[0]->info.ruleId;
				if(left_ruleId!=56 && left_ruleId!=57 && left_ruleId!=58)
				{
					char info[256]="assign to left-value";
					semanticErrorPrint(6,lineno,info);
					rtn_type = NULL;
					break;
				}
				//开始子节点分析
				struct Type* t_Exp1 = analyze_node_Exp(node_Exp->child[0],0);
				struct Type* t_Exp2 = analyze_node_Exp(node_Exp->child[2],0);
				if(t_Exp1==NULL || t_Exp2==NULL)
				{
					rtn_type = NULL;
					break;
				}
				if(0==typeIsSame(t_Exp1,t_Exp2))
				{
					//类型不同的赋值
					char info[256]="type conflict in assignment";
					semanticErrorPrint(5,lineno,info);
					rtn_type = NULL;
					break;
				}
				rtn_type = t_Exp1;
				break;
			}
		case 44:
			{
				struct Type* t_Exp1 = analyze_node_Exp(node_Exp->child[0],0);
				struct Type* t_Exp2 = analyze_node_Exp(node_Exp->child[2],0);
				if(typeIsBasic(t_Exp1,0)==0 || typeIsBasic(t_Exp2,0)==0)
				{
					char info[256]="type cannot be used by \"&&\"";
					semanticErrorPrint(7,lineno,info);
				}
				rtn_type = getAnIntType();
				break;
			}
		case 45:
			{
				struct Type* t_Exp1 = analyze_node_Exp(node_Exp->child[0],0);
				struct Type* t_Exp2 = analyze_node_Exp(node_Exp->child[2],0);
				if(typeIsBasic(t_Exp1,0)==0 || typeIsBasic(t_Exp2,0)==0)
				{
					char info[256]="type cannot be used by \"||\"";
					semanticErrorPrint(7,lineno,info);
				}
				rtn_type = getAnIntType();
				break;
			}
		case 46:
			{
				struct Type* t_Exp1 = analyze_node_Exp(node_Exp->child[0],0);
				struct Type* t_Exp2 = analyze_node_Exp(node_Exp->child[2],0);
				if(typeIsBasic(t_Exp1,0)==0 || typeIsBasic(t_Exp2,0)==0)
				{
					if(typeIsBasic(t_Exp1,1)==0 || typeIsBasic(t_Exp2,1)==0)
					{
						//两边既不同时是int也不同时是float
						char info[256]="";
						sprintf(info,"type uncompairable by \"%s\"",node_Exp->child[1]->info.notation);
						semanticErrorPrint(7,lineno,info);
					}
				}
				rtn_type = getAnIntType();
				break;
			}
		case 47:
			{
				struct Type* t_Exp1 = analyze_node_Exp(node_Exp->child[0],0);
				struct Type* t_Exp2 = analyze_node_Exp(node_Exp->child[2],0);
				if(typeIsBasic(t_Exp1,0)==0 || typeIsBasic(t_Exp2,0)==0)
				{
					if(typeIsBasic(t_Exp1,1)==0 || typeIsBasic(t_Exp2,1)==0)
					{
						//两边既不同时是int也不同时是float
						char info[256]="type unaddable by \"+\"";
						semanticErrorPrint(7,lineno,info);
						rtn_type = NULL;
						break;
					}
				}
				rtn_type = t_Exp1;
				break;
			}
		case 48:
			{
				struct Type* t_Exp1 = analyze_node_Exp(node_Exp->child[0],0);
				struct Type* t_Exp2 = analyze_node_Exp(node_Exp->child[2],0);
				if(t_Exp1==NULL)
				{
					rtn_type=t_Exp2;
					break;
				}
				else if(t_Exp2==NULL)
				{
					rtn_type=t_Exp1;
					break;
				}
				if(typeIsBasic(t_Exp1,0)==0 || typeIsBasic(t_Exp2,0)==0)
				{
					if(typeIsBasic(t_Exp1,1)==0 || typeIsBasic(t_Exp2,1)==0)
					{
						//两边既不同时是int也不同时是float
						char info[256]="type unsubtractable by \"-\"";
						semanticErrorPrint(7,lineno,info);
						rtn_type = NULL;
						break;
					}
				}
				rtn_type = t_Exp1;
				break;
			}
		case 49:
			{
				struct Type* t_Exp1 = analyze_node_Exp(node_Exp->child[0],0);
				struct Type* t_Exp2 = analyze_node_Exp(node_Exp->child[2],0);
				if(typeIsBasic(t_Exp1,0)==0 || typeIsBasic(t_Exp2,0)==0)
				{
					if(typeIsBasic(t_Exp1,1)==0 || typeIsBasic(t_Exp2,1)==0)
					{
						//两边既不同时是int也不同时是float
						char info[256]="type cannot be multiplied by \"*\"";
						semanticErrorPrint(7,lineno,info);
						rtn_type = NULL;
						break;
					}
				}
				rtn_type = t_Exp1;
				break;
			}
		case 50:
			{
				struct Type* t_Exp1 = analyze_node_Exp(node_Exp->child[0],0);
				struct Type* t_Exp2 = analyze_node_Exp(node_Exp->child[2],0);
				if(typeIsBasic(t_Exp1,0)==0 || typeIsBasic(t_Exp2,0)==0)
				{
					if(typeIsBasic(t_Exp1,1)==0 || typeIsBasic(t_Exp2,1)==0)
					{
						//两边既不同时是int也不同时是float
						char info[256]="type cannot be divided by \"/\"";
						semanticErrorPrint(7,lineno,info);
						rtn_type = NULL;
						break;
					}
				}
				rtn_type = t_Exp1;
				break;
			}
		case 51:
			{
				rtn_type = analyze_node_Exp(node_Exp->child[1],0);
				break;
			}
		case 52:
			{
				struct Type* t_Exp = analyze_node_Exp(node_Exp->child[1],0);
				if(typeIsBasic(t_Exp,0)==0)
				{
					if(typeIsBasic(t_Exp,1)==0)
					{
						//既不是int也不是float
						char info[256]="type cannot be used by minus \"-\"";
						semanticErrorPrint(7,lineno,info);
						rtn_type = NULL;
					}
				}
				else
				{
					rtn_type = t_Exp;
				}
				break;
			}
		case 53:
			{
				struct Type* t_Exp = analyze_node_Exp(node_Exp->child[1],0);
				if(typeIsBasic(t_Exp,0)==0)
				{
					//不是int
					char info[256]="type cannot be used by \"!\"";
					semanticErrorPrint(7,lineno,info);
					rtn_type = NULL;
				}
				else
				{
					rtn_type = t_Exp;
				}
			}
		case 54:
			{
				//函数的使用
				struct Node* node_ID = node_Exp->child[0];
				char* name = node_ID->info.notation;
				//查看是否对变量用了（）函数表达
				struct Prop* exist_prop_v = searchPropStackByPropName(name,0,1,0);
				//查看函数是否有定义
				struct Prop* exist_prop = searchPropStackByPropName(name,0,0,1);
				if(exist_prop_v!=NULL && exist_prop==NULL)
				{
					//对变量用了括号（）
					char info[256] = "";
					int conflict_lineno = exist_prop_v->un.varprop->def_lineno;
					sprintf(info,"\"%s\" defined at line %d is not a function",name,conflict_lineno);
					semanticErrorPrint(11,lineno,info);
					rtn_type = NULL;
			dbg_printPropStack(1);
					break;
				}
				if(exist_prop==NULL)
				{
					//使用了未定义的函数
					char info[256] = "";
					sprintf(info,"function \"%s\" is neither defined nor declared before being used",name);
					semanticErrorPrint(2,lineno,info);
					rtn_type = NULL;
					break;
				}
				//检查函数参数是否与定义一致
				struct Type* t_Args = analyze_node_Args(node_Exp->child[2]);
				int arg_cnt = 0;
				struct Type* t_ptr = t_Args;
				while(t_ptr!=NULL)
				{
					arg_cnt++;
					t_ptr = t_ptr->next;
				}
				if(arg_cnt!=exist_prop->un.funprop->paracnt)
				{
					//参数个数不同
					char info[256];
					sprintf(info,"argument number of function \"%s\" does not match its defination",name);
					semanticErrorPrint(9,lineno,info);
					rtn_type = getArrEleType(exist_prop->un.funprop->f_rtn_type);
					break;
				}
				struct Prop* p_Args = exist_prop->un.funprop->paraprop;
				int cnt=0;
				while(t_Args!=NULL)
				{
					cnt++;
					if(typeIsSame(t_Args,p_Args->un.varprop->type)==0)
					{
						//参数类型不同
						char info[256]="";
						sprintf(info,"type of the No.%d argument of function \"%s\" does not match its defination",cnt,name);
						semanticErrorPrint(9,lineno,info);
					}
					t_Args = t_Args->next;
					p_Args = p_Args->next;
				}
				rtn_type = exist_prop->un.funprop->f_rtn_type;
				break;
			}
		case 55:
			{
				//函数的使用
				struct Node* node_ID = node_Exp->child[0];
				char* name = node_ID->info.notation;
				//查看是否对变量用了（）函数表达
				struct Prop* exist_prop = searchPropStackByPropName(name,0,1,0);
				if(exist_prop)
				{
					//对变量用了括号（）
					char info[256] = "";
					int conflict_lineno = exist_prop->un.varprop->def_lineno;
					sprintf(info,"\"%s\" defined at line %d is not a function",name,conflict_lineno);
					semanticErrorPrint(11,lineno,info);
					rtn_type = NULL;
					break;
				}
				//查看函数是否有定义
				exist_prop = searchPropStackByPropName(name,0,0,1);
				if(exist_prop==NULL)
				{
					//使用了未定义的函数
					char info[256] = "";
					sprintf(info,"function \"%s\" is neither defined nor declared before being used",name);
					semanticErrorPrint(2,lineno,info);
					rtn_type = NULL;
					break;
				}
				//检查函数参数在定义时是否也是0个
				if(exist_prop->un.funprop->paracnt!=0)
				{
					//参数个数不同
					char info[256];
					sprintf(info,"argument number of function \"%s\" does not match its defination",name);
					semanticErrorPrint(9,lineno,info);
					rtn_type = getArrEleType(exist_prop->un.funprop->f_rtn_type);
					break;
				}
				rtn_type = getArrEleType(exist_prop->un.funprop->f_rtn_type);
				break;
			}
		case 56:
			{
				struct Type* t_Exp2 = analyze_node_Exp(node_Exp->child[2],0);

				//判断[]内类型
				if(typeIsBasic(t_Exp2,0)==0)
				{
					//数组[]内不是整数
					char info[256] = "index of array is not integer";
					semanticErrorPrint(12,lineno,info);
					//不break，姑且当作[]内是整数，继续分析
				}
				//确定数组维数
				struct Type* t_Exp = analyze_node_Exp(node_Exp->child[0],dim+1);	/*如果child[0]是ID，则返回ID的数据类型 */
				if(t_Exp==NULL)
				{
					rtn_type = NULL;
					break;
				}
				//向上返回的数组维度要【低】一维
				rtn_type = t_Exp->un.array.arreletype;
				break;
			}
			case 57:
			{
				struct Type* t_Exp = analyze_node_Exp(node_Exp->child[0],0);
				if(t_Exp==NULL || t_Exp->kind!=2)
				{
					//如果.之前的不是合法的结构变量
					char info[256]="experision before \".\" is not a struct variable";
					semanticErrorPrint(13,lineno,info);
					rtn_type = NULL;
					break;
				}
				struct FieldList* fld_lst =t_Exp->un.structure.struct_field;
				char* fld_name = node_Exp->child[2]->info.notation;
			
				struct FieldList* exist_fldlst = searchFieldListByName(fld_name,fld_lst);
			
				if(exist_fldlst==NULL)
				{
					//未定义的域
					char info[256]="";
					char* struct_name = t_Exp->un.structure.struct_name;
					sprintf(info,"struct \"%s\" has no member named \"%s\"",struct_name,fld_name);
					semanticErrorPrint(14,lineno,info);
					rtn_type = NULL;
					break;
				}
//				rtn_type = getArrEleType(exist_fldlst->type);
				rtn_type = copyType(exist_fldlst->type);
				break;
			}
		case 58:
			{
				struct Node* node_ID = node_Exp->child[0];
				char* name = node_ID->info.notation;
		
				struct Prop* p_ID = searchPropStackByPropName(name,0,1,0);
				if(p_ID==NULL)
				{
					//使用了未定义的变量
					char info[256]="";
					sprintf(info,"variable \"%s\" is used before declaration",name);
					semanticErrorPrint(1,lineno,info);
					rtn_type = NULL;
					break;
				}
		
				if(dim>0 && p_ID->un.varprop->type->kind!=1)
				{
					//如果是对ID取数组，但ID在符号表中不是数组
					char info[256]="";
					sprintf(info,"variable \"%s\" defined at line %d is not an array",name,p_ID->un.varprop->def_lineno);
					semanticErrorPrint(10,lineno,info);
					rtn_type = NULL;
					break;
				}
				int dim_in_table = countArrDim(p_ID->un.varprop->type);	//在符号表中记录的维数
				if( p_ID->un.varprop->type->kind==1 && dim > dim_in_table)
				{
					//如果是数组但是维数过多
					char info[256]="";

					sprintf(info,"array \"%s\" defined at line %d is of dimension %d rather than %d",name,p_ID->un.varprop->def_lineno,dim_in_table,dim);
					semanticErrorPrint(10,lineno,info);
					rtn_type = NULL;
					break;
				}
//				rtn_type = getArrEleType(p_ID->un.varprop->type);
				rtn_type = copyType(p_ID->un.varprop->type);
				break;
			}
		case 59:
			{
				rtn_type = getAnIntType();
				rtn_type->def_lineno = lineno;
				break;
			}
		case 60:
			{
				rtn_type = getAnIntType();
				rtn_type->un.basic = 1;
				rtn_type->def_lineno = lineno;
				break;
			}
	}
	return rtn_type;
}
struct Type* analyze_node_Args(struct Node* node_Args)
{
	/* Args	:	Exp COMMA Args (61)
	 * 	|	Exp (62)
	 * 返回值：
	 * 	串起来的Args类型链表
	 */
	struct Type* rtn_type;
	switch(node_Args->info.ruleId)
	{
		case 61:
			{
				struct Type* t_Exp = analyze_node_Exp(node_Args->child[0],0);
				struct Type* t_Args = analyze_node_Args(node_Args->child[2]);
				if(t_Exp!=NULL)
				{
					t_Exp->next = t_Args;
					rtn_type = t_Exp;
				}
				else
				{
					rtn_type = t_Args;
				}
				break;
			}
		case 62:
			{
				rtn_type = analyze_node_Exp(node_Args->child[0],0);
				break;
			}

	}
	return rtn_type;
}
int analyze_node_LC()
{
	semantic_layer++;
	return semantic_layer;
}
int analyze_node_RC()
{
	//暂时没有pop结构体
	semantic_layer--;
	return semantic_layer;
}
/* 函数声明-用于调试程序的函数 */
void dbg_printPropStack(int need_detail)
{
	/*
	 * 函数功能：
	 * 	输出当前符号表栈内的信息
	 * 参数：
	 * 	need_detail:
	 * 		1:输出详细信息
	 * 		0:输出概要信息
	 * 		-1:输出极简信息
	 */
	if(need_detail==-1)
	{
		dbgprt("=====================================\r\n");
		struct Prop* p = prop_stack_top;
		while(p!=NULL)
		{
			dbgprt("%s ",p->name);
			p = p->next;
		}
		dbgprt("\r\n=====================================\r\n");
		return;
	}
	dbgprt("(栈顶)\r\n");
	dbgprt("====变量/函数符号表====\r\n");
	struct Prop* p_prop = prop_stack_top;
	if(need_detail==0)
	{
		dbgprt("name\tkind\ttype\tlayer\r\n");
	}
	else
	{
		dbgprt("name\tkind\ttype\tlayer\tline\targcnt\r\n");
	}
	while(p_prop!=NULL)
	{
		dbgprt("%s\t",p_prop->name);
		dbgprt("%s\t",p_prop->kind==0?"var":"fun");
		if(p_prop->kind == 0)
		{
			dbgprt("%d\t",p_prop->un.varprop->type->kind);
		}
		else
		{
			dbgprt("%d\t",p_prop->un.funprop->f_rtn_type->kind);
		}
		dbgprt("%d",p_prop->layer);
		if(need_detail)
		{
			dbgprt("\t");
			if(p_prop->kind==0)
			{
				dbgprt("%d",p_prop->un.varprop->def_lineno);
			}
			else
			{
				dbgprt("%d/%d\t",p_prop->un.funprop->declared,p_prop->un.funprop->defined);
				dbgprt("%d",p_prop->un.funprop->paracnt);
			}
		}
		p_prop = p_prop->next;
		dbgprt("\r\n");
	}
	dbgprt("=======================\r\n");
	dbgprt("(栈底)\r\n");
}
void dbg_printTypeStack(int need_detail)
{
	dbgprt("======结构体表栈======\r\n");
	if(need_detail==1)
		dbgprt("typename\tlayer\tfieldlist\r\n");
	else
		dbgprt("typename\r\n");
	struct Type* t_type = type_stack_top;
	while(t_type!=NULL)
	{
		dbgprt("%s",t_type->un.structure.struct_name);
		if(need_detail)
		{
			dbgprt("\t");
			dbgprt("%d\t",t_type->layer);
			struct FieldList* fldlst = t_type->un.structure.struct_field;
			while(fldlst!=NULL)
			{
				dbgprt("%s ",fldlst->name);
				fldlst=fldlst->next;
			}
		}
		dbgprt("\r\n");
		t_type=t_type->next;
	}
	dbgprt("======================\r\n");
}
