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
	char sendline[MAXLINE], recvline[MAXLINE];
	while (Fgets(sendline, MAXLINE, fp) != NULL) {
		Writen(sockfd, sendline, strlen(sendline));
		if (Readline(sockfd, recvline, MAXLINE) == 0)
			err_quit("str_cli: server terminated prematurely");
		Fputs(recvline, stdout);
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
