/*
 * compressR_LOLS.c
 *
 *  Created on: Nov 8, 2016
 *      Author: RyanMini
 *      Summary: This program receives an uncompressed input and will write the specified number of compressed files to the directory.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


void compressR_LOLS1(char * filename, int parts){

	/***********************************************************************/
	// in this segment, we fork into our different processes

	pid_t * pids = (pid_t *)malloc(parts*sizeof(pid_t));
	int i;

	char * ps = malloc(4);
	sprintf(ps, "%d", parts);

	for (i = 0; i < parts; i++){

		pids[i] = fork();

		if (pids[i] == -1){ 							// returns -1 when there is an error forking
			printf("ERROR during forking...\n");
			exit(-1);
		}
		else if (pids[i]){
			// do nothing
		}
		else { 											// if pids[i] is 0 we are in the child process
			char * p = malloc(4);
			sprintf(p, "%d", i);

			char * const paramList[] = {filename, p, ps, NULL};

			int error = execv("/Users/RyanMini/Documents/JUNYA/SystemsProgramming/Assignment2Worker/Debug/Assignment2Worker", paramList);
			printf("ERROR: %s\n", strerror(errno));
			exit(-1);
		}
	}

	/***********************************************************************/
	// in this segment, we wait for the proceses to return

	for (i = 0; i < parts; i++){
		wait(NULL);
	}

	free(pids);

	return;
}

int main(int argc, char ** argv){

	// code to read the input file and then calls compressR_LOLS
	int parts = atoi(argv[2]);
	char * filename = (char*)malloc(sizeof(char)*sizeof(argv[1]));
	filename = argv[1];
	compressR_LOLS1(filename, parts);
	// printf("\n");


}
