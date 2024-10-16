static int fib6_add_rt2node(struct fib6_node *fn, struct fib6_info *rt,
			    struct nl_info *info,
			    struct netlink_ext_ack *extack)
{
	struct fib6_info *leaf = rcu_dereference_protected(fn->leaf,
				    lockdep_is_held(&rt->fib6_table->tb6_lock));
	struct fib6_info *iter = NULL;
	struct fib6_info __rcu **ins;
	struct fib6_info __rcu **fallback_ins = NULL;
	int replace = (info->nlh &&
		       (info->nlh->nlmsg_flags & NLM_F_REPLACE));
	int add = (!info->nlh ||
		   (info->nlh->nlmsg_flags & NLM_F_CREATE));
	int found = 0;
	bool rt_can_ecmp = rt6_qualify_for_ecmp(rt);
	u16 nlflags = NLM_F_EXCL;
	int err;

	if (info->nlh && (info->nlh->nlmsg_flags & NLM_F_APPEND))
		nlflags |= NLM_F_APPEND;

	ins = &fn->leaf;

	for (iter = leaf; iter;
	     iter = rcu_dereference_protected(iter->fib6_next,
				lockdep_is_held(&rt->fib6_table->tb6_lock))) {
		/*
		 *	Search for duplicates
		 */

		if (iter->fib6_metric == rt->fib6_metric) {
			/*
			 *	Same priority level
			 */
			if (info->nlh &&
			    (info->nlh->nlmsg_flags & NLM_F_EXCL))
				return -EEXIST;

			nlflags &= ~NLM_F_EXCL;
			if (replace) {
				if (rt_can_ecmp == rt6_qualify_for_ecmp(iter)) {
					found++;
					break;
				}
				if (rt_can_ecmp)
					fallback_ins = fallback_ins ?: ins;
				goto next_iter;
			}

			if (rt6_duplicate_nexthop(iter, rt)) {
				if (rt->fib6_nsiblings)
					rt->fib6_nsiblings = 0;
				if (!(iter->fib6_flags & RTF_EXPIRES))
					return -EEXIST;
				if (!(rt->fib6_flags & RTF_EXPIRES))
					fib6_clean_expires(iter);
				else
					fib6_set_expires(iter, rt->expires);

				if (rt->fib6_pmtu)
					fib6_metric_set(iter, RTAX_MTU,
							rt->fib6_pmtu);
				return -EEXIST;
			}
			/* If we have the same destination and the same metric,
			 * but not the same gateway, then the route we try to
			 * add is sibling to this route, increment our counter
			 * of siblings, and later we will add our route to the
			 * list.
			 * Only static routes (which don't have flag
			 * RTF_EXPIRES) are used for ECMPv6.
			 *
			 * To avoid long list, we only had siblings if the
			 * route have a gateway.
			 */
			if (rt_can_ecmp &&
			    rt6_qualify_for_ecmp(iter))
				rt->fib6_nsiblings++;
		}

		if (iter->fib6_metric > rt->fib6_metric)
			break;

next_iter:
		ins = &iter->fib6_next;
	}

	if (fallback_ins && !found) {
		/* No ECMP-able route found, replace first non-ECMP one */
		ins = fallback_ins;
		iter = rcu_dereference_protected(*ins,
				    lockdep_is_held(&rt->fib6_table->tb6_lock));
		found++;
	}

	/* Reset round-robin state, if necessary */
	if (ins == &fn->leaf)
		fn->rr_ptr = NULL;

	/* Link this route to others same route. */
	if (rt->fib6_nsiblings) {
		unsigned int fib6_nsiblings;
		struct fib6_info *sibling, *temp_sibling;

		/* Find the first route that have the same metric */
		sibling = leaf;
		while (sibling) {
			if (sibling->fib6_metric == rt->fib6_metric &&
			    rt6_qualify_for_ecmp(sibling)) {
				list_add_tail(&rt->fib6_siblings,
					      &sibling->fib6_siblings);
				break;
			}
			sibling = rcu_dereference_protected(sibling->fib6_next,
				    lockdep_is_held(&rt->fib6_table->tb6_lock));
		}
		/* For each sibling in the list, increment the counter of
		 * siblings. BUG() if counters does not match, list of siblings
		 * is broken!
		 */
		fib6_nsiblings = 0;
		list_for_each_entry_safe(sibling, temp_sibling,
					 &rt->fib6_siblings, fib6_siblings) {
			sibling->fib6_nsiblings++;
			BUG_ON(sibling->fib6_nsiblings != rt->fib6_nsiblings);
			fib6_nsiblings++;
		}
		BUG_ON(fib6_nsiblings != rt->fib6_nsiblings);
		rt6_multipath_rebalance(temp_sibling);
	}

	/*
	 *	insert node
	 */
	if (!replace) {
		if (!add)
			pr_warn("NLM_F_CREATE should be set when creating new route\n");

add:
		nlflags |= NLM_F_CREATE;

		if (!info->skip_notify_kernel) {
			err = call_fib6_entry_notifiers(info->nl_net,
							FIB_EVENT_ENTRY_ADD,
							rt, extack);
			if (err) {
				struct fib6_info *sibling, *next_sibling;

				/* If the route has siblings, then it first
				 * needs to be unlinked from them.
				 */
				if (!rt->fib6_nsiblings)
					return err;

				list_for_each_entry_safe(sibling, next_sibling,
							 &rt->fib6_siblings,
							 fib6_siblings)
					sibling->fib6_nsiblings--;
				rt->fib6_nsiblings = 0;
				list_del_init(&rt->fib6_siblings);
				rt6_multipath_rebalance(next_sibling);
				return err;
			}
		}

		rcu_assign_pointer(rt->fib6_next, iter);
		fib6_info_hold(rt);
		rcu_assign_pointer(rt->fib6_node, fn);
		rcu_assign_pointer(*ins, rt);
		if (!info->skip_notify)
			inet6_rt_notify(RTM_NEWROUTE, rt, info, nlflags);
		info->nl_net->ipv6.rt6_stats->fib_rt_entries++;

		if (!(fn->fn_flags & RTN_RTINFO)) {
			info->nl_net->ipv6.rt6_stats->fib_route_nodes++;
			fn->fn_flags |= RTN_RTINFO;
		}

	} else {
		int nsiblings;

		if (!found) {
			if (add)
				goto add;
			pr_warn("NLM_F_REPLACE set, but no existing node found!\n");
			return -ENOENT;
		}

		if (!info->skip_notify_kernel) {
			err = call_fib6_entry_notifiers(info->nl_net,
							FIB_EVENT_ENTRY_REPLACE,
							rt, extack);
			if (err)
				return err;
		}

		fib6_info_hold(rt);
		rcu_assign_pointer(rt->fib6_node, fn);
		rt->fib6_next = iter->fib6_next;
		rcu_assign_pointer(*ins, rt);
		if (!info->skip_notify)
			inet6_rt_notify(RTM_NEWROUTE, rt, info, NLM_F_REPLACE);
		if (!(fn->fn_flags & RTN_RTINFO)) {
			info->nl_net->ipv6.rt6_stats->fib_route_nodes++;
			fn->fn_flags |= RTN_RTINFO;
		}
		nsiblings = iter->fib6_nsiblings;
		iter->fib6_node = NULL;
		fib6_purge_rt(iter, fn, info->nl_net);
		if (rcu_access_pointer(fn->rr_ptr) == iter)
			fn->rr_ptr = NULL;
		fib6_info_release(iter);

		if (nsiblings) {
			/* Replacing an ECMP route, remove all siblings */
			ins = &rt->fib6_next;
			iter = rcu_dereference_protected(*ins,
				    lockdep_is_held(&rt->fib6_table->tb6_lock));
			while (iter) {
				if (iter->fib6_metric > rt->fib6_metric)
					break;
				if (rt6_qualify_for_ecmp(iter)) {
					*ins = iter->fib6_next;
					iter->fib6_node = NULL;
					fib6_purge_rt(iter, fn, info->nl_net);
					if (rcu_access_pointer(fn->rr_ptr) == iter)
						fn->rr_ptr = NULL;
					fib6_info_release(iter);
					nsiblings--;
					info->nl_net->ipv6.rt6_stats->fib_rt_entries--;
				} else {
					ins = &iter->fib6_next;
				}
				iter = rcu_dereference_protected(*ins,
					lockdep_is_held(&rt->fib6_table->tb6_lock));
			}
			WARN_ON(nsiblings != 0);
		}
	}

	return 0;
}