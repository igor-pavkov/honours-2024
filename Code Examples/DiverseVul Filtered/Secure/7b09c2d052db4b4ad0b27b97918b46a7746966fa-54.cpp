int fib6_del(struct fib6_info *rt, struct nl_info *info)
{
	struct fib6_node *fn = rcu_dereference_protected(rt->fib6_node,
				    lockdep_is_held(&rt->fib6_table->tb6_lock));
	struct fib6_table *table = rt->fib6_table;
	struct net *net = info->nl_net;
	struct fib6_info __rcu **rtp;
	struct fib6_info __rcu **rtp_next;

	if (!fn || rt == net->ipv6.fib6_null_entry)
		return -ENOENT;

	WARN_ON(!(fn->fn_flags & RTN_RTINFO));

	/*
	 *	Walk the leaf entries looking for ourself
	 */

	for (rtp = &fn->leaf; *rtp; rtp = rtp_next) {
		struct fib6_info *cur = rcu_dereference_protected(*rtp,
					lockdep_is_held(&table->tb6_lock));
		if (rt == cur) {
			fib6_del_route(table, fn, rtp, info);
			return 0;
		}
		rtp_next = &cur->fib6_next;
	}
	return -ENOENT;
}