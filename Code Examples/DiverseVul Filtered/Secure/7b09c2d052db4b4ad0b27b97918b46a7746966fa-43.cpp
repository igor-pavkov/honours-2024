void fib6_clean_all(struct net *net, int (*func)(struct fib6_info *, void *),
		    void *arg)
{
	__fib6_clean_all(net, func, FIB6_NO_SERNUM_CHANGE, arg, false);
}