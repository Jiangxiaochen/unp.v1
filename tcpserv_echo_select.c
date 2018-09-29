/*
 * =====================================================================================
 *    Description:  simple echo server
 *        Version:  1.0
 *        Created:  09/27/2018 02:18:58 PM
 *         Author:  xiaochen.jiang (kl), jiangxch.hn@gmail.com
 * =====================================================================================
 */

#include "common.h"

int main(int argc, const char *argv[])
{
//  mydaemonize("myechod");
	mysignal(SIGCHLD, sigchld0);
	struct sockaddr_in cli_addr, ser_addr;
	memset(&ser_addr, 0, sizeof(ser_addr));
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	ser_addr.sin_port = htons(SERV_PORT);

	int listen_fd;
	chk_nega((listen_fd =
			  initserver(SOCK_STREAM, (SA *) & ser_addr, sizeof(ser_addr),
						 LISTENQ)));

	int connfd;
	socklen_t cli_len;

	int maxi, maxfd, sockfd, nready, client[FD_SETSIZE], i;
	for (i = 0; i < FD_SETSIZE; i++)
		client[i] = -1;
	maxfd = listen_fd;
	maxi = -1;
	fd_set read_set, all_set;
	FD_ZERO(&all_set);
	FD_SET(listen_fd, &all_set);

	while (1) {
//      cli_len = sizeof(cli_addr);
//      chk_nega((connfd = accept(listen_fd, (SA *) & cli_addr, &cli_len)));
//      chk_nega((child_pid = fork()));
//      if (child_pid == 0) {
//          chk_nega(close(listen_fd));
//          str_echo(connfd);
//          exit(0);
//      }
//      chk_nega(close(connfd));
		read_set = all_set;
		nready = Select(maxfd + 1, &read_set, NULL, NULL, NULL);
		printf("nready:%d", nready);
		if (FD_ISSET(listen_fd, &read_set)) {
			cli_len = sizeof(cli_addr);
			connfd = Accept(listen_fd, (SA *) & cli_addr, &cli_len);
			for (i = 0; i < FD_SETSIZE; i++)
				if (client[i] == -1) {
					client[i] = connfd;
					break;
				}
			if (i == FD_SETSIZE)
				err_quit("too many clients");
			FD_SET(connfd, &all_set);
			if (connfd > maxfd) {
				maxfd = connfd;
			}
			if (i > maxi) {
				maxi = i;
			}
			if (--nready <= 0) {
				continue;
			}
		}
		for (i = 0; i <= maxi; i++) {
			if ((sockfd = client[i]) < 0)
				continue;
			if (FD_ISSET(sockfd, &read_set)) {
				ssize_t n;
				char buf[MAXLINE];
				if ((n = Read(sockfd, buf, MAXLINE)) == 0) {
					Close(sockfd);
					FD_CLR(sockfd, &all_set);
					client[i] = -1;
				} else {
					Writen(sockfd, buf, n);
				}
				if (--nready <= 0)
					break;
			}
		}
	}
	return 0;
}
