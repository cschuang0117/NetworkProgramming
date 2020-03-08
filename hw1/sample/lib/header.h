#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define CMD_MAX 1000

/* user.c */
void user_init(int clients[], int id, const struct sockaddr_in *);
void user_down(int clients[], int maxi, int user);
void boardcast(int clients[], int maxi, const char *msg);
void cmd_who(int clients[], int maxi, int user);
void cmd_name(int clients[], int maxi, int user, const char *name);
void cmd_tell(int clients[], int maxi, int user, const char *name, const char *msg);
void cmd_yell(int clients[], int maxi, int user, const char *msg);

/* recvsend.c */
void send_client(int fd, const char *msg);
void proc_cmd(int clients[], int maxi, int user, char *cmd);
