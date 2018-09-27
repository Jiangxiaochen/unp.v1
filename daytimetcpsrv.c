/*
 * =====================================================================================
 *    Description:  simple daytime server
 *        Version:  1.0
 *        Created:  09/25/2018 03:56:13 PM
 *         Author:  xiaochen.jiang (kl), jiangxch.hn@gmail.com
 * =====================================================================================
 */
#include <common.h>

int main(int argc, const char *argv[])
{
	mydaemonize("daytimed");
	int listenfd, connfd;
	struct sockaddr_in serv_addr;
	char buf[MAXLINE];
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(13);

	listenfd =
		initserver(SOCK_STREAM, (SA *) & serv_addr, sizeof(serv_addr), LISTENQ);

	time_t t;
	size_t buf_len;
	while (1) {
		chk_nega((connfd = accept(listenfd, (SA *) NULL, NULL)));
		t = time(NULL);
		snprintf(buf, MAXLINE, "%.24s\r\n", ctime(&t));
		buf_len = strlen(buf);
		assert(write(connfd, buf, buf_len) == buf_len);
		chk_nega(close(connfd));
	}
	return 0;
}
