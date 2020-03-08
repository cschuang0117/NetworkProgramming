#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>

#define DELAY 33000
#define MAXLINE 1024

struct info{
	int len;
	int sndtype;
	char file[MAXLINE]; //name
	char data[MAXLINE];
};
int printProgress(uint32_t sent, uint32_t total)
{
    int num = ((double) sent / total) * 20;
    if(num >= 20) num = 20;
    int left = 20 - num;
    int i;
    fflush(stdout);
    fprintf(stdout, "Pid: %d Progress : [", getpid());
    //puts("hihi......................................");
    for(i = 0; i < num; i++) fprintf(stdout, "#");
    fflush(stdout);
    for(i = 0; i < left; i++) fprintf(stdout, " ");
  //  fprintf(stdout, "]");
    printf("]");
    if(num == 20){
        printf("\n");
        //puts("................wtgwtgargaergargareg...");
    }
    else{
        printf("\r");
    }
    return num == 20;
}
char name[MAXLINE];
void sending(int sockfd, char msg[], int pid){
	char *pch = strtok(msg, " \n");
	int time;
	if(!strcmp(pch, "put")){
		pch = strtok(NULL, "\n");
		FILE *fp = fopen(pch, "rb");
		struct info snddata;
		if(fp == NULL){
			perror("fopen error");
			return;
		}
		fseek(fp, 0, SEEK_END);
		int filesize = ftell(fp);
		snddata.len = filesize;
		snddata.sndtype = 1;//file info
		strcpy(snddata.file, pch);
		write(sockfd, &snddata, sizeof(snddata));
		fclose(fp);
		printf("Pid: %d [Upload] %s Start!\n", pid, snddata.file);
		fp = fopen(snddata.file, "rb");
		while(snddata.len = read(fileno(fp), snddata.data, MAXLINE)){
			snddata.sndtype = 0;//file content
			strcpy(snddata.file, pch);
			write(sockfd, &snddata, sizeof(snddata));
			printProgress(snddata.len, filesize);
			usleep(DELAY);
		}
	}else if(!strcmp(pch, "sleep")){
		pch = strtok(NULL, "\n");
		if((time = atoi(pch)) <= 0){
			perror("sleep time error");
			return;
		}
		printf("Pid: %d The client starts to sleep.\n", pid);
		for(int i = 1 ; i <= time ; i++){
			printf("Pid: %d Sleep %d\n", pid, i);
			sleep(1);
		}
		printf("Pid: %d Client wakes up.\n", pid);
	}else if(!strcmp(pch, "exit")){
		close(sockfd);
		exit(0);
	}else{
		puts("Error Command");
	}
}
void process(int sockfd, int pid){
	char sndmsg[MAXLINE], rcvmsg[MAXLINE];
	int nread;
	struct info rcvdata;
	while(1){
		if((nread = read(0, sndmsg, sizeof(sndmsg))) > 0)
			sending(sockfd, sndmsg, pid);
		if((nread = read(sockfd, &rcvdata, sizeof(rcvdata))) > 0){
			if(rcvdata.sndtype == 1){
				//recv file info

			}
		}

	}
}
int main(int argc, char *argv[]){
	if(argc != 4){
		fprintf(stderr, "Usage: %s <ip> <port> <username>\n", argv[0]);
		exit(1);
	}
	struct sockaddr_in srvaddr;
	memset(&srvaddr, 0, sizeof(srvaddr));
	srvaddr.sin_family = AF_INET;
	srvaddr.sin_addr.s_addr = inet_addr(argv[1]);
	srvaddr.sin_port = htons(atoi(argv[2]));
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(connect(sockfd, (struct sockaddr *)&srvaddr, sizeof(srvaddr)) == -1){
		fputs("failed to connect...", stderr);
		exit(1);
	}
	printf("Pid: %d Welcome to the dropbox-like server: %s\n", getpid(), name);
    write(sockfd, argv[3], MAXLINE);
    process(sockfd, getpid());
}