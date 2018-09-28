/*
 * =====================================================================================
 *    Description:  simple echo server
 *        Version:  1.0
 *        Created:  09/27/2018 02:18:58 PM
 *         Author:  xiaochen.jiang (kl), jiangxch.hn@gmail.com
 * =====================================================================================
 */

#include "common.h"

void str_echo(int sockfd)
{
	char line[MAXLINE];
	ssize_t n;
	long a, b;
	while(1){
		if((n = Readline(sockfd, line, MAXLINE)) == 0) return;
		if(sscanf(line, "%ld %ld", &a, &b) == 2)
			snprintf(line, MAXLINE, "%ld\n", a + b);
		else
			snprintf(line, MAXLINE, "input error\n");
		n = strlen(line);
		Writen(sockfd, line, n);
	}
}

int main(int argc, const char *argv[])
{
//  mydaemonize("myechod");
//	mysignal(SIGCHLD, sigchld0);
	signal(SIGCHLD, sigchld0);
	struct sockaddr_in cli_addr, ser_addr;
	memset(&ser_addr, 0, sizeof(ser_addr));
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	ser_addr.sin_port = htons(SERV_PORT);

	int listen_fd;
	chk_nega((listen_fd =
			  initserver(SOCK_STREAM, (SA *) & ser_addr, sizeof(ser_addr),
						 LISTENQ)));

	int connfd, child_pid;
	socklen_t cli_len;
	while (1) {
		cli_len = sizeof(cli_addr);
		chk_nega((connfd = accept(listen_fd, (SA *) & cli_addr, &cli_len)));
		chk_nega((child_pid = fork()));
		if (child_pid == 0) {
			chk_nega(close(listen_fd));
			str_echo(connfd);
			exit(0);
		}
		chk_nega(close(connfd));
	}
	return 0;
}
