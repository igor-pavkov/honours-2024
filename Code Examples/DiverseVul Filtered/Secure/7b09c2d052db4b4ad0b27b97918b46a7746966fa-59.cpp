static int ipv6_route_yield(struct fib6_walker *w)
{
	struct ipv6_route_iter *iter = w->args;

	if (!iter->skip)
		return 1;

	do {
		iter->w.leaf = rcu_dereference_protected(
				iter->w.leaf->fib6_next,
				lockdep_is_held(&iter->tbl->tb6_lock));
		iter->skip--;
		if (!iter->skip && iter->w.leaf)
			return 1;
	} while (iter->w.leaf);

	return 0;
}