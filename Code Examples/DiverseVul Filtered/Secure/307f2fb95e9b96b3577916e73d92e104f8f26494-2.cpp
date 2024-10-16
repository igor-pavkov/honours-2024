static void fib6_del_route(struct fib6_node *fn, struct rt6_info **rtp,
			   struct nl_info *info)
{
	struct fib6_walker_t *w;
	struct rt6_info *rt = *rtp;
	struct net *net = info->nl_net;

	RT6_TRACE("fib6_del_route\n");

	/* Unlink it */
	*rtp = rt->dst.rt6_next;
	rt->rt6i_node = NULL;
	net->ipv6.rt6_stats->fib_rt_entries--;
	net->ipv6.rt6_stats->fib_discarded_routes++;

	/* Reset round-robin state, if necessary */
	if (fn->rr_ptr == rt)
		fn->rr_ptr = NULL;

	/* Remove this entry from other siblings */
	if (rt->rt6i_nsiblings) {
		struct rt6_info *sibling, *next_sibling;

		list_for_each_entry_safe(sibling, next_sibling,
					 &rt->rt6i_siblings, rt6i_siblings)
			sibling->rt6i_nsiblings--;
		rt->rt6i_nsiblings = 0;
		list_del_init(&rt->rt6i_siblings);
	}

	/* Adjust walkers */
	read_lock(&fib6_walker_lock);
	FOR_WALKERS(w) {
		if (w->state == FWS_C && w->leaf == rt) {
			RT6_TRACE("walker %p adjusted by delroute\n", w);
			w->leaf = rt->dst.rt6_next;
			if (!w->leaf)
				w->state = FWS_U;
		}
	}
	read_unlock(&fib6_walker_lock);

	rt->dst.rt6_next = NULL;

	/* If it was last route, expunge its radix tree node */
	if (!fn->leaf) {
		fn->fn_flags &= ~RTN_RTINFO;
		net->ipv6.rt6_stats->fib_route_nodes--;
		fn = fib6_repair_tree(net, fn);
	}

	if (atomic_read(&rt->rt6i_ref) != 1) {
		/* This route is used as dummy address holder in some split
		 * nodes. It is not leaked, but it still holds other resources,
		 * which must be released in time. So, scan ascendant nodes
		 * and replace dummy references to this route with references
		 * to still alive ones.
		 */
		while (fn) {
			if (!(fn->fn_flags & RTN_RTINFO) && fn->leaf == rt) {
				fn->leaf = fib6_find_prefix(net, fn);
				atomic_inc(&fn->leaf->rt6i_ref);
				rt6_release(rt);
			}
			fn = fn->parent;
		}
		/* No more references are possible at this point. */
		BUG_ON(atomic_read(&rt->rt6i_ref) != 1);
	}

	inet6_rt_notify(RTM_DELROUTE, rt, info);
	rt6_release(rt);
}