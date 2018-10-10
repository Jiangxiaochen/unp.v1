/*
 * =====================================================================================
 *    Description:  a simple udp echo server
 *        Version:  1.0
 *        Created:  10/10/2018 01:56:28 PM
 *         Author:  xiaochen.jiang (kl), jiangxch.hn@gmail.com
 * =====================================================================================
 */
#include "common.h"

void dg_echo(int sockfd, SA * clie_addr, socklen_t cli_len)
{
	int n;
	socklen_t len;
	char mesg[MAXLINE];
	while (1) {
		len = cli_len;
		n = Recvfrom(sockfd, mesg, MAXLINE, 0, clie_addr, &len);
		Sendto(sockfd, mesg, n, 0, clie_addr, len);
	}
}

int main(int argc, const char *argv[])
{
	int sockfd;
	struct sockaddr_in serv_addr, clie_addr;
	sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(SERV_PORT);

	Bind(sockfd, (SA *) & serv_addr, sizeof(serv_addr));
	dg_echo(sockfd, (SA *) & clie_addr, sizeof(clie_addr));
	return 0;
}
