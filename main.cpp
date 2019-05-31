#include <stdio.h>
#include <ctime>
#include <fstream>
#include <omp.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <stdint.h>
#include <string>
#include <mpi.h>
#include "Tree.h"
#include "TreeUtils.h"


using namespace std;

double startTime, endTime; 

const int level = 2;

void defaultSum(tnode* tree){
    startTime = omp_get_wtime();
    long sum = getSumOfAllChilds(tree);
    endTime = omp_get_wtime();
    printf("[Default] Sum: %llu\n", sum);
    printf("[Default] Time: %lf\n", endTime - startTime);
}

void pthreadSum(tnode* tree){
       
    int threads = 7;
    
    pthreadArg arg;
    arg.tree = tree;
    arg.threadCount = threads;
    startTime = omp_get_wtime();
    getSumOfAllChilds_Pthread((void *) &arg);
    endTime = omp_get_wtime();
    printf("[Pthread] Sum: %llu\n", arg.tree->sum
);
    printf("[Pthread] Time: %lf\n", endTime - startTime);
}


int main(int argc, char* argv[]){
	int rank, numprocs;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    	tnode* tree = makeRandomTree();
    	getSumOfAllChilds_MPI(tree, level, rank, numprocs);
	if (rank == 0) {
    		defaultSum(tree);
   		pthreadSum(tree);
	}
    	MPI_Finalize();
    	return 0;
}
