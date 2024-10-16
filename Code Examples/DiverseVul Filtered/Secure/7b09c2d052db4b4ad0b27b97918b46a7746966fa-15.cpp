int fib6_add(struct fib6_node *root, struct fib6_info *rt,
	     struct nl_info *info, struct netlink_ext_ack *extack)
{
	struct fib6_table *table = rt->fib6_table;
	struct fib6_node *fn, *pn = NULL;
	int err = -ENOMEM;
	int allow_create = 1;
	int replace_required = 0;
	int sernum = fib6_new_sernum(info->nl_net);

	if (info->nlh) {
		if (!(info->nlh->nlmsg_flags & NLM_F_CREATE))
			allow_create = 0;
		if (info->nlh->nlmsg_flags & NLM_F_REPLACE)
			replace_required = 1;
	}
	if (!allow_create && !replace_required)
		pr_warn("RTM_NEWROUTE with no NLM_F_CREATE or NLM_F_REPLACE\n");

	fn = fib6_add_1(info->nl_net, table, root,
			&rt->fib6_dst.addr, rt->fib6_dst.plen,
			offsetof(struct fib6_info, fib6_dst), allow_create,
			replace_required, extack);
	if (IS_ERR(fn)) {
		err = PTR_ERR(fn);
		fn = NULL;
		goto out;
	}

	pn = fn;

#ifdef CONFIG_IPV6_SUBTREES
	if (rt->fib6_src.plen) {
		struct fib6_node *sn;

		if (!rcu_access_pointer(fn->subtree)) {
			struct fib6_node *sfn;

			/*
			 * Create subtree.
			 *
			 *		fn[main tree]
			 *		|
			 *		sfn[subtree root]
			 *		   \
			 *		    sn[new leaf node]
			 */

			/* Create subtree root node */
			sfn = node_alloc(info->nl_net);
			if (!sfn)
				goto failure;

			fib6_info_hold(info->nl_net->ipv6.fib6_null_entry);
			rcu_assign_pointer(sfn->leaf,
					   info->nl_net->ipv6.fib6_null_entry);
			sfn->fn_flags = RTN_ROOT;

			/* Now add the first leaf node to new subtree */

			sn = fib6_add_1(info->nl_net, table, sfn,
					&rt->fib6_src.addr, rt->fib6_src.plen,
					offsetof(struct fib6_info, fib6_src),
					allow_create, replace_required, extack);

			if (IS_ERR(sn)) {
				/* If it is failed, discard just allocated
				   root, and then (in failure) stale node
				   in main tree.
				 */
				node_free_immediate(info->nl_net, sfn);
				err = PTR_ERR(sn);
				goto failure;
			}

			/* Now link new subtree to main tree */
			rcu_assign_pointer(sfn->parent, fn);
			rcu_assign_pointer(fn->subtree, sfn);
		} else {
			sn = fib6_add_1(info->nl_net, table, FIB6_SUBTREE(fn),
					&rt->fib6_src.addr, rt->fib6_src.plen,
					offsetof(struct fib6_info, fib6_src),
					allow_create, replace_required, extack);

			if (IS_ERR(sn)) {
				err = PTR_ERR(sn);
				goto failure;
			}
		}

		if (!rcu_access_pointer(fn->leaf)) {
			if (fn->fn_flags & RTN_TL_ROOT) {
				/* put back null_entry for root node */
				rcu_assign_pointer(fn->leaf,
					    info->nl_net->ipv6.fib6_null_entry);
			} else {
				fib6_info_hold(rt);
				rcu_assign_pointer(fn->leaf, rt);
			}
		}
		fn = sn;
	}
#endif

	err = fib6_add_rt2node(fn, rt, info, extack);
	if (!err) {
		if (rt->nh)
			list_add(&rt->nh_list, &rt->nh->f6i_list);
		__fib6_update_sernum_upto_root(rt, sernum);
		fib6_start_gc(info->nl_net, rt);
	}

out:
	if (err) {
#ifdef CONFIG_IPV6_SUBTREES
		/*
		 * If fib6_add_1 has cleared the old leaf pointer in the
		 * super-tree leaf node we have to find a new one for it.
		 */
		if (pn != fn) {
			struct fib6_info *pn_leaf =
				rcu_dereference_protected(pn->leaf,
				    lockdep_is_held(&table->tb6_lock));
			if (pn_leaf == rt) {
				pn_leaf = NULL;
				RCU_INIT_POINTER(pn->leaf, NULL);
				fib6_info_release(rt);
			}
			if (!pn_leaf && !(pn->fn_flags & RTN_RTINFO)) {
				pn_leaf = fib6_find_prefix(info->nl_net, table,
							   pn);
#if RT6_DEBUG >= 2
				if (!pn_leaf) {
					WARN_ON(!pn_leaf);
					pn_leaf =
					    info->nl_net->ipv6.fib6_null_entry;
				}
#endif
				fib6_info_hold(pn_leaf);
				rcu_assign_pointer(pn->leaf, pn_leaf);
			}
		}
#endif
		goto failure;
	}
	return err;

failure:
	/* fn->leaf could be NULL and fib6_repair_tree() needs to be called if:
	 * 1. fn is an intermediate node and we failed to add the new
	 * route to it in both subtree creation failure and fib6_add_rt2node()
	 * failure case.
	 * 2. fn is the root node in the table and we fail to add the first
	 * default route to it.
	 */
	if (fn &&
	    (!(fn->fn_flags & (RTN_RTINFO|RTN_ROOT)) ||
	     (fn->fn_flags & RTN_TL_ROOT &&
	      !rcu_access_pointer(fn->leaf))))
		fib6_repair_tree(info->nl_net, table, fn);
	return err;
}