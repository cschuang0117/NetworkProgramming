#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<string.h>
#include<netinet/in.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/select.h>
#include<sys/stat.h>
#include<errno.h>
#include <fcntl.h>

struct userinfo{
	char username[10][20];
	char path[10][20];
};
int userflag[10] = {0};
struct userinfo list;
int userindex = 0;//for userinfo index
void cmd(int client[], int maxi, int id, char *msg){
	char *pch = strtok(msg, " \n");
	//printf ("cmd received is %s\n", pch);
	if(!strcmp(pch, "user")){
		pch = strtok(NULL, "\n");
		
		char welcomemsg[50];
		snprintf(welcomemsg, 50, "Welcome to the dropbox-like server: %s\n", pch);
		write(client[id], welcomemsg, strlen(welcomemsg));
		//printf("write msg: %s", welcomemsg);
		
		int i;
		for(i = 0 ; i < 10 ; i++){
			if(userflag[i] == 0){
				strcpy(list.username[i], pch);
				userflag[i] = 1;
				puts("user create...");
				userindex = i;
				snprintf(list.path[i], 20, "./%s", list.username[i]);
				int status = mkdir(list.path[i], S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
				break;
			}else if(userflag[i]){
				if(!strcmp(pch, list.username[i])){
					//update files...
					break;
				}
			}
		}
	}else if(!strcmp(pch, "put")){
		pch = strtok(NULL, "\n");
		char filepath[20];
		snprintf(filepath, 20, "%s/%s", list.path[userindex], pch);
		printf("filepath = %s\n", filepath);
		FILE *fp = fopen(filepath, "wb");
		if(fp){
			//puts("fuckyou");
			int n;
			char filecontent[1024];
			do{
				n = read(client[id], filecontent, 1023);
				puts("hihi");
				filecontent[n] = '\0';
				fwrite(filecontent, 1, sizeof(filecontent), fp);
				printf("n = %d\n", n);
			}while(n || (errno == EWOULDBLOCK && n < 0));
		}else{
			puts("QQ");
		}
		//fwrite("fuckyou", 1, 7, fp);
		
	}
}
int main(int argc, char *argv[]){
	if(argc != 2){
		fprintf(stderr, "Usage: %s <port>\n", argv[0]);
		exit(1);
	}
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in srvaddr;
	srvaddr.sin_family = AF_INET;
	srvaddr.sin_port = htons(atoi(argv[1]));
	srvaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(sockfd, (struct sockaddr *)&srvaddr, sizeof(srvaddr)) != -1){
		puts("bind success...");
	}else{
		fputs("bind failed...", stderr);
		exit(1);
	}
	listen(sockfd, 10);
	//int val = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, O_NONBLOCK);
	memset(client, -1, sizeof(client));
	
}