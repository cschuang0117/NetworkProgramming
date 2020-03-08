#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/select.h>
#include<signal.h>

#define maxsz 1024

int main(int argc, char* argv[]){
	if(argc!=3){
		fprintf(stderr,"Usage: %s <host> <port>\n", argv[0]);
		exit(1);
	}
	struct sockaddr_in srvaddr;
	memset(&srvaddr, 0, sizeof(srvaddr));
	srvaddr.sin_port = htons(atoi(argv[2]));
	srvaddr.sin_family = AF_INET;
	srvaddr.sin_addr.s_addr = inet_addr(argv[1]);
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(connect(sockfd, (struct sockaddr *)&srvaddr, sizeof(srvaddr)) == -1){
		fputs("failed to connect...", stderr);
		exit(1);
	}
	char buf[maxsz];
	fd_set r_set;
	FD_ZERO(&r_set);
	int maxfd = (sockfd > 0 ? sockfd : 0) + 1;
	while(1){
		FD_SET(0, &r_set);
		FD_SET(sockfd, &r_set);	
		select(maxfd, &r_set, NULL, NULL,NULL);
		if(FD_ISSET(0, &r_set)){
			int n = read(0, buf, 1023);
			buf[n] = '\0';
			if(!strcmp(buf,"exit\n")){
				close(sockfd);
				break;
			}
			//puts("writing input message");
			write(sockfd, buf, n);
		}
		if(FD_ISSET(sockfd, &r_set)){
			int n = read(sockfd, buf, 1023);
			if(n == 0){
				puts("Connection closed by server");
				close(sockfd);
				break;
			}
			buf[n] = '\0';
			//puts("message received...");
			printf("%s", buf);
		}
	}
}
