#ifndef _SYNTAXTREE_H_
#define _SYNTAXTREE_H_

#include<stdio.h>
#include<stdlib.h>
#define MAX_NODE_NUM 7		//多叉树的最大叉数
#define MAX_NOTATION_LEN 64	//Node的Info中的notation的长度
struct Node;
enum NodeType	{ERR,LEX,STX};
struct Node;
struct Info;
struct Node* getNewNode();
void InsertNode(struct Node* parent,struct Node* child);
void FreeNodeRecurs(struct Node* node);
void printSyntaxTree(struct Node* root,int tab);

struct Info
{
	char	name[50];		//symbol name(such as "ID","ExtDefList")
	int	lineno;			//line number
	union 	{
		unsigned int INT;
		float FLOAT;
	}	value;			//const value
	enum NodeType	nodetype;	//lexical(leaf) or syntax node
	char	notation[MAX_NOTATION_LEN];	//notation namely: name of ID,type of RELOP,constant text,TYPE("INT","FLOAT").
	int	ruleId;			//production rule ID adapted
};
struct Node
{
	struct Info	info;
	int		numofchild;
	struct Node*	child[MAX_NODE_NUM];
};


#endif
