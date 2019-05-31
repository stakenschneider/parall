#include <ctime>
#include <fstream>
#include <vector>
#include <mpi.h>

#include "TreeUtils.h"

tnode* makeRandomTree() {
 
    tnode* tree = new tnode;
    tree->value = 5; 

    srand(1);
    for (int i = 0; i < GENERATE_COUNT; i++) {
        long random = rand()%10;
        addNode(random, 0, tree);
    }

    return tree;
}


void prepareSubTreesForMPI(tnode* tree, int level, std::vector<tnode*> *buf) {
	
	if (tree != NULL){
  		if(level > tree->level)  {
     			prepareSubTreesForMPI(tree->left, level, buf);
     			prepareSubTreesForMPI(tree->right, level, buf);
  		}

  		if(level == tree->level) {
    			buf->push_back(tree);
  		}
 	} else {printf("lol");}
}

long getSumOfAllChilds(tnode* tree) {
	if (tree != NULL){
		long leftSum = 0;
		long rightSum = 0;

		if (tree->left != NULL) {
			tree->left->sum = getSumOfAllChilds(tree->left);
			leftSum = tree->left->sum + tree->left->value;
		}

		if (tree->right != NULL)
		{
			tree->right->sum = getSumOfAllChilds(tree->right);
			rightSum = tree->right->sum + tree->right->value;
		}

		return leftSum + rightSum;
	}
	return 0;
}

long getSumOfAllChilds_MPI(tnode* tree, int level, int rank, int numprocs) {
	MPI_Request request;
	long Result = 0;
	MPI_Status status;
	int tag = 50;
	std::vector<tnode*> buf;
	double starttime, endtime = 0.0;
	prepareSubTreesForMPI(tree, level, &buf);


	if (rank == 0) {
		starttime= MPI_Wtime();
	}

	if (rank != 0) {
			tnode* subTree = buf.at(rank-1);
			long drobSum = getSumOfAllChilds(subTree) + subTree->value;
			MPI_Isend(&drobSum, 1, MPI_LONG, 0, tag, MPI_COMM_WORLD, &request);			
	}
	if (rank == 0) {
		for (int i = 1; i < numprocs; i++) {
			long res = 0;
			MPI_Recv(&res, 1, MPI_LONG, i, tag, MPI_COMM_WORLD, &status);
			Result += res;
		}
		Result = Result + getLastNodes(level, tree) - tree->value;
		endtime = MPI_Wtime();
		printf("[MPI]: Time %lf\n", endtime - starttime);
		printf("[MPI]: Sum %llu\n", Result);
				
	}
	
	return 0;
}

long getLastNodes(int level, tnode* tree) {
	if (tree != NULL && level > tree->level) {
			return getLastNodes(level, tree->left) + getLastNodes(level, tree->right) + tree->value;
	}
	return 0;
}

void* getSumOfAllChilds_Pthread(void *args){
    pthreadArg *arg = (pthreadArg *)args; 
	
    if (arg->tree != NULL){	
        long leftSum = 0; 
	long rightSum = 0;
		

	if (arg->threadCount <= 1){
            arg->tree->sum = getSumOfAllChilds(arg->tree);
            return 0;
	}
        int leftJoinStatus, rightJoinStatus; 
        int leftCreateStatus, rightCreateStatus;

	
	pthread_t leftThread;
	pthreadArg leftArg;
	if (arg->tree->left != NULL){		
            leftArg.tree = arg->tree->left;
            leftArg.threadCount = arg->threadCount/2;
            leftCreateStatus = pthread_create(&leftThread, NULL, getSumOfAllChilds_Pthread, (void*) &leftArg);	
            if (leftCreateStatus != 0) {
                printf("[ERROR] Can't create thread. Status: %d\n", leftCreateStatus);
		exit(ERROR_CREATE_THREAD);
            }
	}

	
	pthread_t rightThread;
	pthreadArg rightArg;
	if (arg->tree->right != NULL){		
            rightArg.tree = arg->tree->right;
            rightArg.threadCount =  arg->threadCount/2;
            rightCreateStatus = pthread_create(&rightThread, NULL, getSumOfAllChilds_Pthread, (void*) &rightArg);	
            if (rightCreateStatus != 0) {
                printf("[ERROR] Can't create thread. Status: %d\n", rightCreateStatus);
		exit(ERROR_CREATE_THREAD);
            }
	}		
		
     
	leftCreateStatus = pthread_join(leftThread, (void**)&leftJoinStatus);
	if (leftCreateStatus != SUCCESS) {
            printf("[ERROR] Can't join thread. Status: %d\n", leftCreateStatus);
            exit(ERROR_JOIN_THREAD);
	}
	if (arg->tree->left != NULL){
            arg->tree->left->sum = leftArg.tree->sum;
            leftSum = arg->tree->left->sum + arg->tree->left->value;
	}

	rightCreateStatus = pthread_join(rightThread, (void**)&rightJoinStatus);
	if (rightCreateStatus != SUCCESS) {
            printf("[ERROR] Can't join thread. Status: %d\n", rightCreateStatus);
            exit(ERROR_JOIN_THREAD);
	}
	if (arg->tree->right != NULL){
            arg->tree->right->sum = rightArg.tree->sum;
            rightSum = arg->tree->right->sum + arg->tree->right->value;
	}
		
	arg->tree->sum = leftSum + rightSum;
    }
    return 0;
}
