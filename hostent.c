/*
 * =====================================================================================
 *    Description:  test for gethostbyname
 *        Version:  1.0
 *        Created:  10/11/2018 10:26:21 AM
 *         Author:  xiaochen.jiang (kl), jiangxch.hn@gmail.com
 * =====================================================================================
 */

#include "common.h"


int main(int argc, const char *argv[])
{
	char *ptr;
	struct hostent *hptr;

	while (--argc > 0) {
		ptr = *++argv;
		if ((hptr = gethostbyname(ptr)) == NULL) {
			err_msg("gethostbyname error: %s: %s", ptr, hstrerror(h_errno));
			continue;
		}
		printHostent(hptr);
	}

	return 0;
}
