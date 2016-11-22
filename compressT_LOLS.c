/*
 * compressT_LOLS.c
 *
 *  Created on: Nov 9, 2016
 *      Author: RyanMini
 */

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <semaphore.h>
#include <time.h>

pthread_mutex_t m;
sem_t s;
char * filename;
int parts;

char * compress(char * text){


	char * compressed = (char *)malloc(strlen(text));

	printf("%s\n", text);

	int len = strlen(text);
	int currentSubStart = 0;
	int currentSubLen = 1;
	int i;
	int index = 0;

	for (i = 1; i < len; i++){

		if (text[i] == text[i-1]){
			currentSubLen++;
		}
		else {
			if (currentSubLen == 1){
				compressed[index] = text[currentSubStart];
				index++;
			}
			else if (currentSubLen == 2){
				compressed[index] = text[currentSubStart];
				compressed[index + 1] = text[currentSubStart];
				index += 2;
			}
			else {
				char * number = (char *)malloc(4);
				sprintf(number, "%d", currentSubLen);
				int numLen = strlen(number);

				int j;
				for (j = 0; j < numLen; j++){
					compressed[index] = number[j];
					index++;
				}
				compressed[index] = text[currentSubStart];
				index++;
			}
			currentSubStart = i;
			currentSubLen = 1;
		}

	}
	if (currentSubLen == 1){
		compressed[index] = text[currentSubStart];
		index++;
	}
	else if (currentSubLen == 2){
		compressed[index] = text[currentSubStart];
		compressed[index + 1] = text[currentSubStart];
		index += 2;
	}
	else {
		char * number = (char *)malloc(4);
		sprintf(number, "%d", currentSubLen);
		int numLen = strlen(number);

		int j;
		for (j = 0; j < numLen; j++){
			compressed[index] = number[j];
			index++;
		}
		compressed[index] = text[currentSubStart];
		index++;
		free(number);
	}



	compressed[index] = '\0';
	// at this point compresses holds the compressed string

	return compressed;

}

int findLength(FILE * fp){
	int count = 0;
	int newChar;
	while ((newChar = fgetc(fp)) != EOF)
		count++;

	return count;
}

void * compressT_worker_LOLS(int part){

	/*********************************************************************/
	// in this segment we open the file and write its contents to a buffer

	FILE * ptr_file;
	ptr_file = fopen(filename, "r");

	if (!ptr_file){ 				// if ptr_file is NULL, there was an error
		printf("ERROR reading file, check file name...\n");
		exit(-1);
	}

	int filelength = findLength(ptr_file) + 10;
	// printf("FILE LENGTH IS %d\n", filelength-10);
	char * buf = malloc(filelength);

	rewind(ptr_file);

	int i;
	char ch;
	for (i = 0; (i < filelength - 10) && ((ch = fgetc(ptr_file)) != EOF); i++){
		buf[i] = ch;
	}
	buf[i] = '\0';

	fclose(ptr_file);

	// printf("buffer is:\n%s\nEnd of buffer\n", buf);

	/*********************************************************************/
	// in this segment, we use the buffer and the given number of parts to determine the indexes of the different parts


	int len = strlen(buf);				// we are sure to free buf so that large sums of memory aren't copied

	if (parts > len){
		printf("ERROR parts > characters in input file...\n");
		exit(-1);
	}

	int baselen = len/parts;
	int remain = len % parts;

	int templen;


	int startIndex = 0;
	int endIndex;

	templen = baselen + remain;
	for (i = 0; i < part; i++){

		remain = 0;
		startIndex += templen;
		templen = baselen + remain;
	}

	endIndex = startIndex + templen;

	// printf("Goes from %d to %d\n", startIndex, endIndex);
	int partlen = endIndex - startIndex;

	char * text = (char *)malloc(partlen);

	memcpy(text, &buf[startIndex], partlen);
	text[partlen] = '\0';
	free(buf);

	/*********************************************************************/
	// in this segment, we have the chunk of text, we must now compress it
	char * compressed = compress(text);
	free(text);
	printf("%s\n", compressed);

	/*********************************************************************/
	// in this segment, we write the compressed text to the appropriate file

	int filelen = strlen(filename) - 3;
	char * subbuf = (char*)malloc(filelen);
	memcpy(subbuf, filename, filelen);
	subbuf[filelen - 1] = '\0';
	filelen += 10;

	buf = (char*)malloc(filelen);

	sprintf(buf, "%s_txt_LOLS%d", subbuf, part);
	// printf("%s\n", buf);
	free(subbuf);

	ptr_file = fopen(buf, "w");

	if (ptr_file == NULL){
		printf("ERROR writing file...");
		exit(-1);
	}

	fprintf(ptr_file, "%s", compressed);
	fclose(ptr_file);

	free(buf);
	free(compressed);

	pthread_exit(0);
}

void compressT_LOLS(char * filename, int parts){

	int i;
	pthread_t * threads = (pthread_t*)malloc(parts*sizeof(pthread_t));

	for (i = 0; i < parts; i++){

		pthread_create(&threads[i], NULL, compressT_worker_LOLS, i);
		// sleep(1);
		// pthread_join(threads[i], NULL);
	}

	for (i = 0; i < parts; i++){
		pthread_join(threads[i], NULL);
	}

	free(threads);

	return;

}

int main(int argc, char ** argv){


	clock_t begin = clock();
	//int i = 0;
	//for (i = 0; i < 10000000; i++){


		parts = atoi(argv[2]);
		// filename = (char*)malloc(sizeof(char)*sizeof(argv[1]));


		filename = argv[1];
		compressT_LOLS(filename, parts);


	//}


	clock_t end = clock();
	double time_spent = (double)(end - begin)/CLOCKS_PER_SEC;
	printf("Run time = %0.6f\n", time_spent);

	exit(0);
}
