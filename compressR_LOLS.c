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



char ** divide(char * text, int parts);


void compressR_LOLS(char * filename, int parts){

	char buf[1000];
	FILE * ptr_file;
	ptr_file = fopen(filename, "r");

	if (!ptr_file){
		printf("ERROR reading file, check file name...\n");
		exit(-1);
	}

	char * text = (char*)malloc(1000);
	while (fgets(buf,1000, ptr_file) != NULL){
		sprintf(text, "%s", buf);
	}
	fclose(ptr_file);

	int i;

	char **array = divide(text, parts);

	pid_t * pids = (pid_t *)malloc(parts*sizeof(pid_t));
	for (i = 0; i < parts; i++){
		pids[i] = fork();
		if (pids[i] == 0){  // running as the child
			char * s = malloc(2);
			sprintf(s, "%d", i);
			char * const paramList[] = {array[i], s, filename};

			int error = execv("/Users/RyanMini/Documents/JUNYA/Systems Programming/Assignment2Worker/Debug/Assignment2Worker", paramList);
			printf("%d\n", error);
		}
		else { 				// running as parent
			//int  status;
			//waitpid(pids[i], &status, NULL);
		}
	}

	for (i = 0; i < parts; i++){
		wait(NULL);
	}
	return;

}

char ** divide(char * text, int parts){


	int len = strlen(text);

	int baselen = len/parts;
	int remain = len % parts;
	int index = 0;
	int i;

	char **array = (char**)malloc(len);
	for (i = 0; i < parts; i++){
		int inputlen = baselen;
		if (remain != 0){
			inputlen++;
			remain--;
		}
		array[i] = (char*)malloc(inputlen + 1);
		memcpy(array[i], &text[index], inputlen);
		index += inputlen;
		printf("%s\n", array[i]);
	}
	return array;
}

int main(int argc, char ** argv){

	// code to read the input file and then calls compressR_LOLS
	int parts = atoi(argv[2]);
	char * filename = (char*)malloc(sizeof(char)*sizeof(argv[1]));
	filename = argv[1];
	compressR_LOLS(filename, parts);
	printf("Nigga we made it\n");


}
