#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
	FILE *fp = fopen(argv[1], "r");
	char buffer[500], pchcp[500] = {0};
	if(fp == NULL)	printf("The file cannot open\n");
	else{
		printf("------------------------Input file %s------------------------\n", argv[1]);
		while (1){
			fgets(buffer, sizeof(buffer), fp);
			if(feof(fp))	break;
			printf("%s", buffer);
			char *pch = strtok(buffer, " ");
			//printf("buffer = %s\n",buffer);
			if(!strcmp(pch, "reverse")){
				//printf("it is reverse\n");
				pch = strtok(NULL, " ");
				//printf("pch = %s\n", pch);
				int i;
				for(i = 0 ; i < strlen(pch) ; i++){
					pchcp[i] = pch[strlen(pch) - i - 2];
				}
				printf("%s\n", pchcp);
			}else if(!strcmp(pch, "split")){
				pch = strtok(NULL, " ");
				pch = strtok(pch, argv[2]);
				while (pch != NULL){
					printf("%s", pch);
					pch = strtok (NULL, argv[2]);
					if (pch != NULL)	printf(" ");
				}
			}
		}
		printf("------------------------End of input file %s------------------------\n", argv[1]);
	}
	fclose(fp);
	char str[500]={0}, strcp[500]={0}, op[10];
	printf("************************User input************************\n");
	while (1){
		//puts("input options: ");
		scanf ("%s",op);
		//puts(op);
		if (!strcmp(op,"exit"))	break;
		//printf("str = %s", str);
		if (!strcmp(op, "reverse")){
			fgets(str, sizeof(str), stdin);
			int i;
			for(i = 0 ; i < strlen(str) ; i++){
					strcp[i] = str[strlen(str) - i - 2];
			}
			printf("%s\n", strcp);
		}else if (!strcmp(op, "split")){
			fgets(str, sizeof(str), stdin);
			char *pch = strtok(str, " ");
			//printf("pch = %s", pch);
			pch = strtok(pch, argv[2]);
			while (pch != NULL){
				printf("%s", pch);
				pch = strtok (NULL, argv[2]);
				if (pch != NULL)	printf(" ");
			}
		}else{
			printf("syntax error!!\nusage: <operation> <string>\n");
		}
	}

}