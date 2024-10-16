int sock_diag_check_cookie(void *sk, __u32 *cookie)
{
	if ((cookie[0] != INET_DIAG_NOCOOKIE ||
	     cookie[1] != INET_DIAG_NOCOOKIE) &&
	    ((u32)(unsigned long)sk != cookie[0] ||
	     (u32)((((unsigned long)sk) >> 31) >> 1) != cookie[1]))
		return -ESTALE;
	else
		return 0;
}