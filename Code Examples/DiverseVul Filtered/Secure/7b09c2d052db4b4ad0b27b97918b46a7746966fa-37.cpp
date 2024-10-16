void fib6_update_sernum_upto_root(struct net *net, struct fib6_info *rt)
{
	__fib6_update_sernum_upto_root(rt, fib6_new_sernum(net));
}