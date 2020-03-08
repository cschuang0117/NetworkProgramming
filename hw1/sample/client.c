#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <netdb.h>
#include <unistd.h>

#define CMD_MAX 1000

int main(int argc, char *argv[]) {
	if(argc != 3) {
		printf("Usage: %s <host> <port>\n", argv[0]);
		exit(1);
	}

	struct hostent *host = gethostbyname(argv[1]);
	if(host == NULL) {
			fprintf(stderr, "Cannot get host by %s.\n", argv[1]);
			exit(2);
	}

	struct sockaddr_in server_addr;
	int sock_fd;

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	memcpy(&server_addr.sin_addr, host->h_addr, host->h_length);

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(sock_fd == -1) {
		fputs("Fail to create socket.\n", stderr);
		exit(2);
	}

	if(connect(sock_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
		fputs("Fail to connect.\n", stderr);
		exit(2);
	}

	char buf[1000];
	int stdin_fd = fileno(stdin);
	fd_set r_set;
	int max_fd = (sock_fd > stdin_fd ? sock_fd : stdin_fd) + 1;
	FD_ZERO(&r_set);
	printf("stdin_fd = %d\n", stdin_fd);
	while(1) {
		FD_SET(stdin_fd, &r_set);
		FD_SET(sock_fd, &r_set);
		select(max_fd, &r_set, NULL, NULL, NULL);

		if(FD_ISSET(stdin_fd, &r_set)) {
			int n = read(stdin_fd, buf, 999);
			buf[n] = '\0';
			if(strcmp(buf, "exit\n") == 0) {
				close(sock_fd);
				break;
			}
			write(sock_fd, buf, n);
		}
		if(FD_ISSET(sock_fd, &r_set)) {
			int n = read(sock_fd, buf, 999);
			if(n == 0) {
				puts("Connection closed by server");
				close(sock_fd);
				break;
			}
			buf[n] = '\0';
			fputs(buf, stdout);
		}
	}

	return 0;
}
