#include "header.h"

char names[FD_SETSIZE][20];
char ips[FD_SETSIZE][20];
short ports[FD_SETSIZE];

void user_init(int clients[], int id, const struct sockaddr_in *cli_addr) {
	char welcome_msg[CMD_MAX];

	strcpy(names[id], "anonymous");
	//inet_ntop(AF_INET, &cli_addr->sin_addr, ips[id], sizeof(ips[id]));
	strcpy(ips[id],inet_ntoa(cli_addr->sin_addr));
	ports[id] = ntohs(cli_addr->sin_port);
	
	sprintf(welcome_msg, "Hello, anonymous! From: %s:%hu", ips[id], ports[id]);
	send_client(clients[id], welcome_msg);
}

void user_down(int clients[], int maxi, int user) {
	char msg[CMD_MAX];
	sprintf(msg, "%s is offline.", names[user]);
	boardcast(clients, maxi, msg);
}

void cmd_who(int clients[], int maxi, int user) {
	int i;
	for(i=0; i<=maxi; i++) {
		if(clients[i] < 0)
			continue;
		char msg[CMD_MAX];
		sprintf(msg, "%s %s:%hu", names[i], ips[i], ports[i]);
		if(i == user) {
			strcat(msg, " ->me");
		}
		send_client(clients[user], msg);
	}
}

void cmd_name(int clients[], int maxi, int user, const char *name) {
	if(strcmp(name, "anonymous") == 0) {
		send_client(clients[user], "ERROR: Username cannot be anonymous.");
		return;
	}

	int len = strlen(name);
	if(len < 2 || len > 12 || strspn(name, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz") != len) {
		send_client(clients[user], "ERROR: Username can only consists of 2~12 English letters.");
		return;
	}

	int i;
	for(i=0; i<=maxi; i++) {
		if(clients[i] >= 0 && i != user && strcmp(name, names[i]) == 0) {
			char msg[CMD_MAX];
			sprintf(msg, "ERROR: %s has been used by others.", name);
			send_client(clients[user], msg);
			return;
		}
	}

	char msg[CMD_MAX];
	int u_fd = clients[user];
	clients[user] = -1;
	sprintf(msg, "%s is now known as %s.", names[user], name);
	strcpy(names[user], name);

	boardcast(clients, maxi, msg);
	clients[user] = u_fd;
	sprintf(msg, "You're now known as %s.", name);
	send_client(u_fd, msg);
}

void cmd_tell(int clients[], int maxi, int user, const char *name, const char *msg) {
	if(strcmp(names[user], "anonymous") == 0) {
		send_client(clients[user], "ERROR: You are anonymous.");
		return;
	}
	if(strcmp(name, "anonymous") == 0) {
		send_client(clients[user], "ERROR: The client to which you sent is anonymous.");
		return;
	}

	int i;
	for(i=0; i<=maxi; i++) {
		if(clients[i] >= 0 && strcmp(names[i], name) == 0) {
			char buf[CMD_MAX];
			sprintf(buf, "%s tell you %s", names[user], msg);
			send_client(clients[user], "SUCCESS: Your message has been sent.");
			send_client(clients[i], buf);
			return;
		}
	}
	send_client(clients[user], "ERROR: The receiver doesn't exist.");
}

void cmd_yell(int clients[], int maxi, int user, const char *msg) {
	char buf[CMD_MAX];
	sprintf(buf, "%s yell %s", names[user], msg);
	boardcast(clients, maxi, buf);
}

void boardcast(int clients[], int maxi, const char *msg) {
	int i;
	for(i=0; i<=maxi; i++) {
		if(clients[i] >= 0) {
			send_client(clients[i], msg);
		}
	}
}
