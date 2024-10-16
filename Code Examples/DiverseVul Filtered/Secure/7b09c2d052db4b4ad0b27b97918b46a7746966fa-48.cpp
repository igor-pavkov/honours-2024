int fib6_lookup(struct net *net, int oif, struct flowi6 *fl6,
		struct fib6_result *res, int flags)
{
	return fib6_table_lookup(net, net->ipv6.fib6_main_tbl, oif, fl6,
				 res, flags);
}