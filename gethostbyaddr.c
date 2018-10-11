/*
 * =====================================================================================
 *    Description:  test for gethostbyaddr
 *        Version:  1.0
 *        Created:  10/11/2018 11:15:56 AM
 *         Author:  xiaochen.jiang (kl), jiangxch.hn@gmail.com
 * =====================================================================================
 */

#include "common.h"

int main(int argc, const char *argv[])
{
	struct hostent *hptr;
	while (--argc) {
		hptr = gethostbyname(*++argv);
		printHostent(hptr);
		hptr = gethostbyaddr(*hptr->h_addr_list, sizeof(struct in_addr), AF_INET);
		if(hptr == NULL){
			printf("%s\n", hstrerror(h_errno));
			return -1;
		}
		printf("------------------\n");
		printHostent(hptr);
	}
	return 0;
}
