/*
 * =====================================================================================
 *    Description:  simple daytime client
 *        Version:  1.0
 *        Created:  09/25/2018 02:38:11 PM
 *         Author:  xiaochen.jiang (kl), jiangxch.hn@gmail.com
 * =====================================================================================
 */
#include <common.h>

int main(int argc, const char *argv[])
{
	int sockfd, n;
	char recvline[MAXLINE + 1];
	struct sockaddr_in serv_addr;
	assert(argc == 2);
	chk_nega((sockfd = socket(AF_INET, SOCK_STREAM, 0)));	/* create fd */

	memset(&serv_addr, 0, sizeof(serv_addr));	/* init serv_addr */
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(13);
	chk_nega(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr));

	chk_nega(connect(sockfd, (SA *) & serv_addr, sizeof(serv_addr)));

	while ((n = read(sockfd, recvline, MAXLINE)) > 0) {
		recvline[n] = 0;
		assert(write(STDOUT_FILENO, recvline, n) == n);
	}
	if (n < 0)
		err_sys("read error");
	return 0;
}
