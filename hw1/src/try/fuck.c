#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int main(){
	int client[FD_SETSIZE];
	memset(client, -1, sizeof(client));
	int i;
	printf("FD_SETSIZE = %d\n",FD_SETSIZE);
	for(i = 0 ; i < 10 ; i++)
		printf("client = %d\n", client[i]);
	printf("fileno = %d\n",fileno(stdin));
	char pos[20];
	//if(pos)
	//printf("pos = %s\n", pos);
	strcpy(pos,"asdf");
	printf("pos = %s\n",pos);	
	snprintf(pos, sizeof(pos),"%s->me",pos);
	printf("pos = %s\n",pos);

}
