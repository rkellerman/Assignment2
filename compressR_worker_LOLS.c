/*
 * compressR_worker_LOLS.c
 *
 *  Created on: Nov 8, 2016
 *      Author: RyanMini
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char ** argv){

	char * text = argv[0];
	int part = atoi(argv[1]);
	char * filename = argv[2];

	printf("text is : %s, part is %d, filename is %s\n", text, part, filename);

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


	printf("DONE\n");
	exit(1);

}

