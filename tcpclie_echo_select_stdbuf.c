/*
 * =====================================================================================
 *    Description:  simple daytime client
 *        Version:  1.0
 *        Created:  09/25/2018 02:38:11 PM
 *         Author:  xiaochen.jiang (kl), jiangxch.hn@gmail.com
 * =====================================================================================
 */
#include "common.h"

void str_cli(FILE * fp, int sockfd)
{
	int maxfdp1;
	fd_set rset;
	char send_line[MAXLINE], recv_line[MAXLINE];

	FD_ZERO(&rset);
	while (1) {
		FD_SET(fileno(fp), &rset);
		FD_SET(sockfd, &rset);
		maxfdp1 = max(fileno(fp), sockfd) + 1;
		Select(maxfdp1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(sockfd, &rset)) {
			if (Readline(sockfd, recv_line, MAXLINE) == 0)
				err_quit("str_cli: server terminated prematurely");
			Fputs(recv_line, stdout);
		}

		if (FD_ISSET(fileno(fp), &rset)) {
			if (Fgets(send_line, MAXLINE, fp) == NULL)
				return;
			Writen(sockfd, send_line, strlen(send_line));
		}
	}
}

int main(int argc, const char *argv[])
{
	int sockfd;
	struct sockaddr_in serv_addr;
	assert(argc == 2);
	chk_nega((sockfd = socket(AF_INET, SOCK_STREAM, 0)));	/* create fd */

	memset(&serv_addr, 0, sizeof(serv_addr));	/* init serv_addr */
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERV_PORT);
	chk_nega(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr));

	chk_nega(connect(sockfd, (SA *) & serv_addr, sizeof(serv_addr)));

	str_cli(stdin, sockfd);
	return 0;
}
