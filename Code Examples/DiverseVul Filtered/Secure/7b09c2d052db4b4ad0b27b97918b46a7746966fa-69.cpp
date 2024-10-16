static void fib6_del_route(struct fib6_table *table, struct fib6_node *fn,
			   struct fib6_info __rcu **rtp, struct nl_info *info)
{
	struct fib6_walker *w;
	struct fib6_info *rt = rcu_dereference_protected(*rtp,
				    lockdep_is_held(&table->tb6_lock));
	struct net *net = info->nl_net;

	RT6_TRACE("fib6_del_route\n");

	/* Unlink it */
	*rtp = rt->fib6_next;
	rt->fib6_node = NULL;
	net->ipv6.rt6_stats->fib_rt_entries--;
	net->ipv6.rt6_stats->fib_discarded_routes++;

	/* Flush all cached dst in exception table */
	rt6_flush_exceptions(rt);

	/* Reset round-robin state, if necessary */
	if (rcu_access_pointer(fn->rr_ptr) == rt)
		fn->rr_ptr = NULL;

	/* Remove this entry from other siblings */
	if (rt->fib6_nsiblings) {
		struct fib6_info *sibling, *next_sibling;

		list_for_each_entry_safe(sibling, next_sibling,
					 &rt->fib6_siblings, fib6_siblings)
			sibling->fib6_nsiblings--;
		rt->fib6_nsiblings = 0;
		list_del_init(&rt->fib6_siblings);
		rt6_multipath_rebalance(next_sibling);
	}

	/* Adjust walkers */
	read_lock(&net->ipv6.fib6_walker_lock);
	FOR_WALKERS(net, w) {
		if (w->state == FWS_C && w->leaf == rt) {
			RT6_TRACE("walker %p adjusted by delroute\n", w);
			w->leaf = rcu_dereference_protected(rt->fib6_next,
					    lockdep_is_held(&table->tb6_lock));
			if (!w->leaf)
				w->state = FWS_U;
		}
	}
	read_unlock(&net->ipv6.fib6_walker_lock);

	/* If it was last route, call fib6_repair_tree() to:
	 * 1. For root node, put back null_entry as how the table was created.
	 * 2. For other nodes, expunge its radix tree node.
	 */
	if (!rcu_access_pointer(fn->leaf)) {
		if (!(fn->fn_flags & RTN_TL_ROOT)) {
			fn->fn_flags &= ~RTN_RTINFO;
			net->ipv6.rt6_stats->fib_route_nodes--;
		}
		fn = fib6_repair_tree(net, table, fn);
	}

	fib6_purge_rt(rt, fn, net);

	if (!info->skip_notify_kernel)
		call_fib6_entry_notifiers(net, FIB_EVENT_ENTRY_DEL, rt, NULL);
	if (!info->skip_notify)
		inet6_rt_notify(RTM_DELROUTE, rt, info, 0);

	fib6_info_release(rt);
}