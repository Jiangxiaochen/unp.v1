/*
 * =====================================================================================
 *
 *    Description:  some common macros for my apue
 *        Created:  08/03/2018 09:14:46 AM
 *         Author:  xiaochen.jiang (kl), jiangxch.hn@gmail.com
 *
 * =====================================================================================
 */

#ifndef COMMON_JXC_APUE_H
#define COMMON_JXC_APUE_H

#include "unp.h"
#include <limits.h>
#include <time.h>
#include <assert.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <bits/signum.h>
#include <sys/socket.h>
#if defined(SOLARIS)
#include <netinet/in.h>
#endif
#include <netdb.h>
#include <arpa/inet.h>
#if defined(BSD)
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#define chk_nega(call) if ((call) < 0) {\
    err_sys("Error in `" #call "` syscall (%s:%d)", __FILE__, __LINE__);\
}

#define chk_null(ptr) if ((ptr) == NULL) {\
    err_sys("Error in `" #ptr "` syscall (%s:%d)", __FILE__, __LINE__);\
}

#define chk_zero(call) if ((call) == 0) {\
    err_sys("Error in `" #call "` syscall (%s:%d)", __FILE__, __LINE__);\
}

#define chk_eof(call) if ((call) == EOF) {\
    err_sys("Error in `" #call "` syscall (%s:%d)", __FILE__, __LINE__);\
}

#define chk_sig(call) if ((call) == SIG_ERR) {\
    err_sys("Error in `" #call "` syscall (%s:%d)", __FILE__, __LINE__);\
}

typedef void (*Sighandler_t) (int);
static inline Sighandler_t mysignal(int signo, Sighandler_t func)
{
	struct sigaction act, oact;
	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (signo == SIGALRM) {
#ifdef SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT;
#endif
	} else {
#ifdef SA_RESTART
		act.sa_flags |= SA_RESTART;
#endif
	}
//  chk_nega(sigaction(signo, &act, &oact));
	if (sigaction(signo, &act, &oact) < 0)
		return SIG_ERR;
	return oact.sa_handler;

}

static inline void printHostent(struct hostent *hptr)
{
	char **pptr;
	char str[INET_ADDRSTRLEN];
	printf("official hostname: %s\n", hptr->h_name);

	for (pptr = hptr->h_aliases; *pptr != NULL; pptr++)
		printf("\talias: %s\n", *pptr);
	if (hptr->h_addrtype == AF_INET) {
		pptr = hptr->h_addr_list;
		while (*pptr != NULL) {
			printf("\taddress: %s\n",
				   Inet_ntop(AF_INET, *pptr, str, sizeof(str)));
			pptr++;
		}
	} else {
		err_ret("unknown address type");
	}
}

static inline void mydaemonize(const char *cmd)
{
	int i, fd0, fd1, fd2;
	pid_t pid;
	struct rlimit rl;
	struct sigaction sa;
	umask(0);					/* clear file creation mask */
	chk_nega(getrlimit(RLIMIT_NOFILE, &rl));	/* get maximum number of fd */

	if ((pid = fork()) < 0) {
		err_quit("%s: can't fork", cmd);
	} else if (pid != 0) {
		exit(0);				/* parent exit */
	}
	setsid();					/* start new session without ctty */
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	chk_nega(sigaction(SIGHUP, &sa, NULL));	/* ignore sighup */
	chk_nega((pid = fork()));
	if (pid != 0)				/* parent exit */
		exit(0);
	chk_nega(chdir("/"));		/* change current working dir */
	if (rl.rlim_max == RLIM_INFINITY) {	/* get max fd */
		rl.rlim_max = 1024;
	}
	for (i = 0; i < rl.rlim_max; i++) {	/* close all fd */
		close(i);
	}

	fd0 = open("/dev/null", O_RDWR);	/* fd 0,1,2 attach to /dev/null */
	fd1 = dup(0);
	fd2 = dup(0);

	openlog(cmd, LOG_CONS, LOG_DAEMON);
	if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
		syslog(LOG_ERR, "unexpected file descriptors %d %d %d", fd0, fd1, fd2);
		exit(1);
	}
}

#define JXC_ENDIAN_BIG 1
#define JXC_ENDIAN_LIT 2
static inline int jxc_getEndian(void)
{
	static int a = 0x1234;
	char *pca = (char *) &a;
	return (*pca == 0x12 ? JXC_ENDIAN_BIG : JXC_ENDIAN_LIT);
}

#include <sys/socket.h>
static inline int initserver(int type, const struct sockaddr *addr,
							 socklen_t alen, int qlen)
{
	int fd, err;
	int reuse = 1;
	if ((fd = socket(addr->sa_family, type, 0)) < 0)
		return (-1);
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0)
		goto errout;
	if (bind(fd, addr, alen) < 0)
		goto errout;
	if (type == SOCK_STREAM || type == SOCK_SEQPACKET)
		if (listen(fd, qlen) < 0)
			goto errout;
	return (fd);
  errout:
	err = errno;
	close(fd);
	errno = err;
	return (-1);
}

static inline int InitTCPServer(int type, const struct sockaddr *addr,
							 socklen_t alen, int qlen)
{
	int fd;
	if ((fd = initserver(type, addr, alen, qlen)) == -1)
		err_sys("initserver error");
	return fd;
}

static inline char *my_sock_ntop(const SA * sa, socklen_t salen)
{
	char port[8];
	static char str[128];
	if (sa->sa_family == AF_INET) {
		struct sockaddr_in *sin = (struct sockaddr_in *) sa;
		if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
			return NULL;
		if (ntohs(sin->sin_port) != 0) {
			snprintf(port, sizeof(port), ":%d", ntohs(sin->sin_port));
			strcat(str, port);
		}
		return str;
	}
}

static inline int sockfd_to_family_jxc(int sockfd)
{
	struct sockaddr_storage ss;
	socklen_t len = sizeof(ss);
	if (getsockname(sockfd, (SA *) & ss, &len) < 0)
		return -1;
	return ss.ss_family;
}

static inline void sigchld0(int sig)
{
	int pid_chld;
	while ((pid_chld = waitpid(-1, 0, WNOHANG)) > 0);
	return;						/* 系统调用被中断时可追踪 */
}

static inline long open_max(void)
{
#ifdef OPEN_MAX
	static long openmax = OPEN_MAX;
#else
	static long openmax = 0;
#endif

#define OPEN_MAX_GUESS 256		/* inadequate */
	if (openmax == 0) {
		errno = 0;
		if ((openmax = sysconf(_SC_OPEN_MAX)) < 0) {
			if (errno == 0)
				openmax = OPEN_MAX_GUESS;
			else {
				fprintf(stderr, "sysconf error\n");
				exit(EXIT_FAILURE);
			}
		}
	}
	return openmax;
}

#endif
