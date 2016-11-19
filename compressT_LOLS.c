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

pthread_mutex_t m;

/* This functions splits the text into the appropriate number of parts

*/
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


/* This function is very similar to the one for processes

*/
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

int main(int argc, char ** argv){

	pthread_mutex_init(&m, NULL);
	int parts = atoi(argv[2]);
	char * filename = (char*)malloc(sizeof(char)*sizeof(argv[1]));
	filename = argv[1];
	compressT_LOLS(filename, parts);
}
