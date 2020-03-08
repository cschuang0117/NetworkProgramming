#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<string.h>
#include<netinet/in.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/select.h>

#define maxsz 1024

char names[10][20];
char ip[10][20];
int ports[10];

void send_client(int fd, char* msg){
	char buf[maxsz];
	//puts("buffer sending...\n");
	snprintf(buf, maxsz, "[Server] %s\n", msg);
	write(fd, buf, strlen(buf));
}
void broadcast(int client[], char* msg, int maxi){
	int i;
	//printf("maxi = %d\n", maxi);
	for(i = 0 ; i <= maxi ; i++){
		if(client[i] >= 0){
			send_client(client[i], msg);
		}
	}
}
void user_init(int client[], int id, const struct sockaddr_in *cliaddr){
	char welcome[maxsz];
	strcpy(names[id], "anonymous");
	strcpy(ip[id],inet_ntoa(cliaddr->sin_addr));
	ports[id] = ntohs(cliaddr->sin_port);
	snprintf(welcome, maxsz - 1, "Hello, anonymous! From: %s:%d",ip[id],ports[id]);
	send_client(client[id], welcome);
}
void user_down(int client[],int id, int maxi){
	char msg[maxsz];
	snprintf(msg, maxsz, "%s is offline.", names[id]);
	broadcast(client, msg, maxi);	
}
void cmd(int client[], int maxi, int id, char *msg){
	char *pch = strtok(msg, " \n");
	printf("cmd received is %s\n", pch);
	if(!strcmp(pch, "who")){
		for(int i = 0 ; i <= maxi ; i++){
			if(client[i] < 0){
				puts("this user is down\n");
				continue;
			}
			char info[maxsz];
		       snprintf(info, 1023, "%s %s:%d", names[i], ip[i], ports[i]);
		       if(i == id)	strcat(info, " ->me");
		       send_client(client[id], info);
		}
		return;
	}else if(!strcmp(pch, "name")){
		pch = strtok(NULL, "\n");
		puts("in name..");
		if(!strcmp(pch,"anonymous")){
			send_client(client[id], "ERROR: Username cannot be anonymous.");
			return;
		}
		for(int i = 0 ; i < maxi ; i++){
			if(!strcmp(pch, names[i]) && (i != id)){
				char info[maxsz];
				snprintf(info, 1023, "ERROR: %s has been used by others.", pch);
				send_client(client[id], info);
				return;
			}
		}
		int namelen = strlen(pch);
		if(namelen < 2 || namelen > 12 || strspn(pch, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz") != namelen){
			send_client(client[id], "ERROR: Username can only consists of 2~12 English letters.");
			return;
		}
		//puts("after condition");
		char info[maxsz];
		int temp = client[id];
		client[id] = -1; //讓廣播不會廣到自己
		snprintf(info, 1023, "%s is now known as %s.", names[id], pch);
		//puts("after condition");
		broadcast(client, info, maxi);
		//puts("after condition");
		strcpy(names[id], pch);
		snprintf(info, 1023, "You're now known as %s.", pch);
		client[id] = temp;
		send_client(client[id], info);
		return;
	}else if (!strcmp(pch, "yell")){
		pch = strtok(NULL, "\n");
		if(pch){
			char buf[maxsz];
			snprintf(buf, 1023, "%s yell %s", names[id], pch);
			broadcast(client, buf, maxi);
		}
		return;
	}else if (!strcmp(pch, "tell")){
		pch = strtok(NULL, " \n");
		if(pch){
			if(!strcmp(names[id], "anonymous")){
				send_client(client[id], "ERROR: You are anonymous.");
				return;
			}else if(!strcmp(pch, "anonymous")){
				send_client(client[id], "ERROR: The client to which you sent is anonymous.");
				return;
			}else{
				for (int i = 0 ; i <= maxi ; i++){
					if(!strcmp(names[i], pch)){
						char receiver[15];
						strcpy(receiver, pch);
						pch = strtok(NULL, "\n");
						if(pch){
							send_client(client[id], "SUCCESS: Your message has been sent.");
							char send[maxsz];
							snprintf(send, 1023, "%s tell you %s", names[id], pch);
							send_client(client[i], send);
						}
						return;
					}
				}
				send_client(client[id], "ERROR: The receiver doesn't exist.");
				return;
			}
		}
	}
	send_client(client[id], "ERROR: Error command.");
	return;
}
int main(int argc, char *argv[]){
	if(argc != 2){
		fprintf(stderr, "Usage: %s <port>\n", argv[0]);
		exit(1);//nonzero -> error code
	}
	int listenfd = socket(AF_INET,SOCK_STREAM,0);
	if(listenfd > 0)	puts("socket built...\n");
	else{
		fputs("socket built failed...\n",stderr);
		exit(1);
	}
	struct sockaddr_in srvaddr;
	memset(&srvaddr, 0, sizeof(srvaddr));
	srvaddr.sin_family = AF_INET;
	srvaddr.sin_port = htons(atoi(argv[1]));
	srvaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(listenfd, (struct sockaddr*)&srvaddr, sizeof(srvaddr)) != -1){
		puts("bind success...\n");
	}else{	//success return 0 failed return -1
		fputs("bind failed...\n",stderr);
		exit(1);
	}
	listen(listenfd, 10);
	fd_set all_set,r_set;
	FD_SET(listenfd, &all_set);
	int maxfd = listenfd + 1;

	int client[10];
	int maxi = 0;
	memset(client, -1, sizeof(client));

	while(1){
		r_set = all_set;
		//puts("twice><");
		int nready = select(maxfd, &r_set, NULL, NULL, NULL);
		//printf("select complete, nready = %d\n", nready);
		if(FD_ISSET(listenfd, &r_set)){
			struct sockaddr_in cliaddr;
			memset(&cliaddr, 0, sizeof(cliaddr));
			int cliaddrlen = sizeof(cliaddr);
			int clifd = accept(listenfd, (struct sockaddr*)&cliaddr, &cliaddrlen);
			//cliaddr用來把連線的資訊丟進去
			if(clifd)	printf("accept successfully...\n");
			else{
				fprintf(stderr,"accept failed...\n");
				exit(1);
			}
			//need to broadcast
			broadcast(client, "Someone is coming!", maxi);
			int i;
			for(i = 0 ; i < 10 ; i++){
				if(client[i] < 0){
					client[i] = clifd;
					break;
				}
				if(i == 9){
					fputs("Too many clients!\n", stderr);
					exit(1);
				}
			}
			//puts("creating client info...");
			//need to say hello to the new client
			user_init(client, i, &cliaddr);
			if(i > maxi)	maxi = i;
			if(clifd >= maxfd)	maxfd = clifd + 1;
			FD_SET(clifd, &all_set);
			if(--nready)	continue;
		}
	//	printf("nready = %d\n", nready);
		int i;
		for(i = 0 ; i <= maxi && nready; i++){
			if(FD_ISSET(client[i], &r_set)){
				--nready;
				char buf[maxsz];
				int n = read(client[i], buf, 1023);
				if(n == 0){
					close(client[i]);
					FD_CLR(client[i], &all_set);
					client[i] = -1;
					user_down(client, i, maxi);
				}else{
				//	puts("command is copied...\n");
					buf[n] = '\0';
					cmd(client, maxi, i, buf);
					//puts("instruction complete");
				}
			}
		}
	}
}	
