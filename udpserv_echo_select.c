/*
 * =====================================================================================
 *    Description:  a simple udp echo server using select
 *        Version:  1.0
 *        Created:  10/10/2018 04:15:40 PM
 *         Author:  xiaochen.jiang (kl), jiangxch.hn@gmail.com
 * =====================================================================================
 */

#include "common.h"

int main(int argc, const char *argv[])
{
	int listenfd, connfd, udpfd, nready, maxfdp1;
	char mesg[MAXLINE];
	pid_t child_pid;
	fd_set rset;
	ssize_t n;
	socklen_t len;
	const int on = 1;
	struct sockaddr_in clie_addr, serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(SERV_PORT);

	listenfd =
		InitTCPServer(SOCK_STREAM, (SA *) & serv_addr, sizeof(serv_addr),
					  LISTENQ);
	udpfd = Socket(AF_INET, SOCK_DGRAM, 0);

	Bind(udpfd, (SA *) & serv_addr, sizeof(serv_addr));
	mysignal(SIGCHLD, sigchld0);

	FD_ZERO(&rset);
	maxfdp1 = max(listenfd, udpfd) + 1;

	for (;;) {
		FD_SET(listenfd, &rset);
		FD_SET(udpfd, &rset);
		if ((nready = select(maxfdp1, &rset, NULL, NULL, NULL)) < 0) {
			if (errno == EINTR)
				continue;
			else
				err_sys("select error");
		}

		if (FD_ISSET(listenfd, &rset)) {
			len = sizeof(clie_addr);
			connfd = Accept(listenfd, (SA *) & clie_addr, &len);
			if ((child_pid = Fork()) == 0) {
				Close(listenfd);
				str_echo(connfd);
				exit(0);
			}
			Close(connfd);
		}

		if (FD_ISSET(udpfd, &rset)) {
			len = sizeof(clie_addr);
			n = Recvfrom(udpfd, mesg, MAXLINE, 0, (SA *) & clie_addr, &len);
			Sendto(udpfd, mesg, n, 0, (SA *) & clie_addr, len);
		}
	}
	return 0;
}
