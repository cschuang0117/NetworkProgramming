#include "lib/header.h"

int main(int argc, char *argv[]) {
	if(argc != 2) {
		fprintf(stderr, "Usage: %s <port>\n", argv[0]);
		exit(1);
	}

	int listen_fd;
	int clients[FD_SETSIZE];
	int client_maxi = 0;
	int max_fd;

	memset(clients, 0xFF, sizeof(clients));

	struct sockaddr_in listen_addr;
	memset(&listen_addr, 0, sizeof(listen_addr));
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	listen_addr.sin_port = htons(atoi(argv[1]));

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_fd == -1) {
		fputs("Cannot create socket.\n", stderr);
		exit(2);
	}
	if(bind(listen_fd, (struct sockaddr *) &listen_addr, sizeof(listen_addr)) == -1) {
		fputs("Cannot bind.\n", stderr);
		exit(2);
	}
	if(listen(listen_fd, 1024) == -1) {
		fputs("Cannot listen.\n", stderr);
		exit(2);
	}

	fd_set all_set, r_set;
	FD_SET(listen_fd, &all_set);
	max_fd = listen_fd + 1;

	while(1) {
		r_set = all_set;
		int nready = select(max_fd, &r_set, NULL, NULL, NULL);
		printf("nready after select = %d\n", nready);
		if(FD_ISSET(listen_fd, &r_set)) {
			struct sockaddr_in client_addr;
			int client_len = sizeof(client_addr);
			int client_fd = accept(listen_fd, (struct sockaddr *) &client_addr, &client_len);
			
			/* boardcast to welcome this client */
			boardcast(clients, client_maxi, "Someone is coming!");

			int i;
			for(i=0; i<FD_SETSIZE; i++) {
				if(clients[i] < 0) {
					clients[i] = client_fd;
					break;
				}
			}

			/* hello to this client */
			user_init(clients, i, &client_addr);

			if(i == FD_SETSIZE) {
				printf("Too many clients!\n");
				exit(2);
			}
			if(i > client_maxi) {
				client_maxi = i;
			}
			if(client_fd >= max_fd) {
				max_fd = client_fd + 1;
			}
			FD_SET(client_fd, &all_set);
			printf("before nready = %d\n", nready);
			if(--nready){
				puts("nready = 0 continue");
				continue;
			}
		}
		printf("nready = %d\n", nready);
		int i;
		for(i=0; i<=client_maxi && nready; i++) {
			if(FD_ISSET(clients[i], &r_set)) {
				puts("minus the nready");
				--nready;

				char buf[CMD_MAX];
				int n = read(clients[i], buf, 999);
				if(n == 0) {
					close(clients[i]);
					FD_CLR(clients[i], &all_set);
					clients[i] = -1;
					user_down(clients, client_maxi, i);
				} else {
					buf[n] = '\0';
					proc_cmd(clients, client_maxi, i, buf);
				}
			}
		}
	}
}
