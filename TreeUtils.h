#pragma once

#include "Tree.h"


#define MAX_VALUE 0xFFFFFFFF        
#define GENERATE_COUNT 100000   

#define SUCCESS 0
#define ERROR_CREATE_THREAD -1
#define ERROR_JOIN_THREAD   -2

struct pthreadArg {
    struct tnode *tree;
    int threadCount;
};


tnode* makeRandomTree();
long getLastNodes(int level, tnode* tree);
long getSumOfAllChilds(tnode* tree);
long getSumOfAllChilds_MPI(tnode* tree, int level, int rank, int numprocs);
void* getSumOfAllChilds_Pthread(void *args);
