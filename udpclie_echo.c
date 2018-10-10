/*
 * =====================================================================================
 *    Description:  a simple udp echo client
 *        Version:  1.0
 *        Created:  10/10/2018 02:13:43 PM
 *         Author:  xiaochen.jiang (kl), jiangxch.hn@gmail.com
 * =====================================================================================
 */

#include "common.h"

void dg_cli(FILE * fp, int sockfd, const SA * pservaddr, socklen_t servlen)
{
	int n;
	socklen_t len;
	struct sockaddr *addr = Malloc(servlen);
	char sendline[MAXLINE], recvline[MAXLINE + 1];

	while (Fgets(sendline, MAXLINE, fp) != NULL) {

		Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

		n = Recvfrom(sockfd, recvline, MAXLINE, 0, addr, &len);
		if (len != servlen || memcmp(pservaddr, addr, len) != 0) {
			printf("reply from %s (ignored)\n", Sock_ntop(addr, len));
			continue;
		}

		recvline[n] = 0;		/* null terminate */
		Fputs(recvline, stdout);
	}
}

int main(int argc, const char *argv[])
{
	int sockfd;
	struct sockaddr_in serv_addr;

	assert(argc == 2);
	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERV_PORT);
	Inet_pton(AF_INET, argv[1], &serv_addr.sin_addr);

	sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
	dg_cli(stdin, sockfd, (SA *) & serv_addr, sizeof(serv_addr));
	return 0;
}
