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

pthread_mutex_t m;
sem_t s;
char * filename;
int parts;

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

void *compressT_worker_LOLS(char ** ptr){

	char * text = ptr[0];
	int part = atoi(ptr[1]);
	char * filename = ptr[2];

	printf("Text is: %s, part is %d, filename is: %s\n", text, part, filename);

	char compressed[1000];

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
				compressed[index] = (char)(currentSubLen + '0');
				compressed[index + 1] = text[currentSubStart];
				index += 2;
			}
			currentSubStart = i;
			currentSubLen = 1;
		}
		if (i == len - 1){
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
				compressed[index] = (char)(currentSubLen + '0');
				compressed[index + 1] = text[currentSubStart];
				index += 2;
			}
		}
	}
	compressed[index] = '\0';
	// at this point compresses holds the compressed string


	int filelen = strlen(filename) - 3;
	char subbuff[filelen];
	memcpy(subbuff, filename, filelen);
	subbuff[filelen - 1] = '\0';
	filelen += 10;

	char buff[filelen];

	sprintf(buff, "%s_txt_LOLS%d", subbuff, part);
	printf("%s\n", buff);


	FILE * ptr_file;
	ptr_file = fopen(buff, "w");

	if (ptr_file == NULL){
		printf("ERROR writing file...");
		exit(-1);
	}

	fprintf(ptr_file, "%s", compressed);
	fclose(ptr_file);
	printf("THREAD DONE\n");

	pthread_exit(NULL);
}

void compressT_LOLS(char * filename, int parts){

	errno = 0;
	char buf[10000];
	FILE * ptr_file;
	ptr_file = fopen(filename, "r");

	if (!ptr_file){
		printf("ERROR reading file, check file name...\n");
		exit(-1);
	}

	char * text = (char*)malloc(10000);
	while (fgets(buf,10000, ptr_file) != NULL){
		sprintf(text, "%s", buf);
	}
	fclose(ptr_file);

	int i;

	char **array = divide(text, parts);

	pthread_t thread[parts];
	for (i = 0; i < parts; i++){

		char * s = malloc(2);
		sprintf(s, "%d", i);

		char *items[] = {array[i], s, filename};
		pthread_create(&thread[i], NULL, compressT_worker_LOLS, items);
		pthread_join(thread[i], NULL);
	}

	printf("PROCESS DONE\n");


}

char * compress(char * text){

	char * compressed = (char *)malloc(1000);

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
	}



	compressed[index] = '\0';
	// at this point compresses holds the compressed string

	return compressed;

}


void * compressT_worker_LOLS1(int part){

	/*********************************************************************/
	// in this segment we open the file and write its contents to a buffer

	char * buf = malloc(10000);
	FILE * ptr_file;
	ptr_file = fopen(filename, "r");

	if (!ptr_file){ 				// if ptr_file is NULL, there was an error
		printf("ERROR reading file, check file name...\n");
		exit(-1);
	}

	while (fgets(buf, 10000, ptr_file) != NULL){  		// scan through 10000 characters, or until we hit an error
		sprintf(buf, "%s", buf);
	}
	fclose(ptr_file);

	/*********************************************************************/
	// in this segment, we use the buffer and the given number of parts to determine the indexes of the different parts


	int len = strlen(buf);						// we are sure to free buf so that large sums of memory aren't copied

	if (parts > len){
		printf("ERROR parts > characters in input file...\n");
	}

	int baselen = len/parts;
	int remain = len % parts;

	int templen;
	int i;

	int startIndex = 0;
	int endIndex;

	templen = baselen + remain;
	for (i = 0; i < part; i++){

		remain = 0;
		startIndex += templen;
		templen = baselen + remain;
	}

	endIndex = startIndex + templen;

	printf("Goes from %d to %d\n", startIndex, endIndex);
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
	printf("%s\n", buf);
	free(subbuf);

	ptr_file = fopen(buf, "w");

	if (ptr_file == NULL){
		printf("ERROR writing file...");
		exit(-1);
	}

	fprintf(ptr_file, "%s", compressed);
	fclose(ptr_file);

	free(buf);

	pthread_exit(0);
}

void compressT_LOLS1(char * filename, int parts){


	int i;
	pthread_t * threads = (pthread_t*)malloc(parts*sizeof(pthread_t));

	for (i = 0; i < parts; i++){

		pthread_create(&threads[i], NULL, compressT_worker_LOLS1, i);
		// sleep(1);
		// pthread_join(threads[i], NULL);
	}

	for (i = 0; i < parts; i++){
		pthread_join(threads[i], NULL);
	}

	exit(0);

}

int main(int argc, char ** argv){

	sem_init(&s, 0, 1);
	parts = atoi(argv[2]);
	filename = (char*)malloc(sizeof(char)*sizeof(argv[1]));
	filename = argv[1];
	compressT_LOLS1(filename, parts);
}
