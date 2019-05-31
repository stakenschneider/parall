#include <stdio.h>
#include <fstream>
#include <iostream>

#include "Tree.h"

using namespace std;

tnode* addNode(long v, long level, tnode *tree)
{
	if (tree == NULL)
	{
		tree = makeNewTree(v, level, tree);
	}
	else if (v < tree->value)	
		tree->left = addNode(v, level + 1, tree->left);
	else if(v >= tree->value)
		tree->right = addNode(v, level + 1, tree->right);
	return(tree);
}


tnode* makeNewTree(long v, long level, tnode *tree)
{
    tree = new tnode;		
    tree->value = v;		
    tree->sum = 0;
    tree->level = level;			
    tree->right = NULL;
    tree->left = NULL;		
    return(tree);
}
