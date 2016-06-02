#include"syntaxtree.h"
struct Node* syntaxTreeRoot = NULL;	//语法树的根
void InitInfo(struct Info* info)
{
	info->name[0]='\0';
	info->lineno = -1;
	info->value.INT = -1;
	info->nodetype = ERR;
	info->notation[0]='\0';
	info->ruleId = -1;
}
void InitNode(struct Node* node)
{
	InitInfo(&(node->info));
	int i;for(i=0;i<MAX_NODE_NUM;i++)
		node->child[i] = NULL;
}
struct Info* getNewInfo()
{
	struct Info* p;
	p = (struct Info*)malloc(sizeof(struct Info));
	if(p == NULL)
	{
		printf("ERROR: %s:%d\n",__FILE__,__LINE__);
		exit(1);
	}
	InitInfo(p);
	return p;
}
struct Node* getNewNode()
{
	struct Node* p;
	p = (struct Node*)malloc(sizeof(struct Node));
	if(p == NULL)
	{
		printf("ERROR: %s:%d\n",__FILE__,__LINE__);
		exit(1);
	}
	InitNode(p);
	return p;
}
void InsertNode(struct Node* parent,struct Node* child)
{
	if(parent->numofchild == MAX_NODE_NUM)
	{
		printf("ERROR: %s:%d\n",__FILE__,__LINE__);
		exit(1);
	}
	parent->child[parent->numofchild] = child;
	parent->numofchild++;
}
void FreeNodeRecurs(struct Node* node)
{
	free(&(node->info));
	int i;for(i=0;i<node->numofchild;i++)
	{
		FreeNodeRecurs(node->child[i]);
	}
	free(node);
}
void printSyntaxTree(struct Node* root,int tab)
{
	if(root==NULL)
		return;
	if(root->info.lineno==-1)
		return;
	int i;for(i=0;i<tab;i++)
		printf("  ");
	if(root->info.nodetype==STX)
	{
		printf("%s (%d)",root->info.name,root->info.lineno);
	}
	else if(root->info.nodetype==LEX)
	{
		printf("%s",root->info.name);
		if(strcmp(root->info.name,"ID")==0)
		{
			printf(": %s",root->info.notation);
		}
		else if(strcmp(root->info.name,"INT")==0)
		{
			printf(": %u",root->info.value.INT);
		}
		else if(strcmp(root->info.name,"FLOAT")==0)
		{
			printf(": %f",root->info.value.FLOAT);
		}
		else if(strcmp(root->info.name,"TYPE")==0)
		{
			printf(": %s",root->info.notation);
		}
	}
	else
	{
		printf("ERR at %s:%d\n",__FILE__,__LINE__);
		exit(1);
	}
/*
printf("DEBUG:not,chn=%s,%d",root->info.notation,root->numofchild);
*/
	printf("\n");
	for(i=0;i<root->numofchild;i++)
	{
		printSyntaxTree(root->child[i],tab+1);
	}
}
