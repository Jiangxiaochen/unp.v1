/*
 * =====================================================================================
 *    Description:  simple echo server using poll
 *        Version:  1.0
 *        Created:  09/29/2018 04:06:01 PM
 *         Author:  xiaochen.jiang (kl), jiangxch.hn@gmail.com
 * =====================================================================================
 */

#include "common.h"

int main(int argc, const char *argv[])
{
	int i, maxi, listenfd, connfd, sockfd, openmax;
	int nready;
	ssize_t n;
	char buf[MAXLINE];
	socklen_t clilen;

	openmax = open_max();
	struct pollfd client[openmax];
	struct sockaddr_in cliaddr, servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	chk_nega((listenfd =initserver(SOCK_STREAM, (SA*)&servaddr, sizeof(servaddr), LISTENQ)));
	for (i = 1; i < openmax; i++)
		client[i].fd = -1;
	client[0].fd = listenfd;
	client[0].events = POLLIN;
	maxi = 0;

	while (1) {
		nready = Poll(client, maxi + 1, INFTIM);
		if (client[0].revents & POLLIN) {
			clilen = sizeof(cliaddr);
			connfd = Accept(listenfd, (SA *) & cliaddr, &clilen);
			for (i = 1; i < openmax; i++)
				if (client[i].fd < 0) {
					client[i].fd = connfd;
					break;
				}
			client[i].events = POLLIN;
			if (i > maxi)
				maxi = i;
			if (--nready <= 0)
				continue;
		}
		for (i = 1; i < openmax; i++) {
			if ((sockfd = client[i].fd) < 0)
				continue;
			if (client[i].revents & (POLLIN | POLLERR)) {
				if ((n = read(sockfd, buf, MAXLINE)) < 0) {
					if (errno == ECONNRESET) {
						Close(sockfd);
						client[i].fd = -1;
					} else
						err_sys("read error");
				} else if (n == 0) {
					Close(sockfd);
					client[i].fd = -1;
				} else
					Writen(sockfd, buf, n);
				if (--nready <= 0)
					break;
			}
		}
	}
	return 0;
}
