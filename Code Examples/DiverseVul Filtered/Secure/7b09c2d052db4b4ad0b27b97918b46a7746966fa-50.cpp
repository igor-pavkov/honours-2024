static struct fib6_node *fib6_repair_tree(struct net *net,
					  struct fib6_table *table,
					  struct fib6_node *fn)
{
	int children;
	int nstate;
	struct fib6_node *child;
	struct fib6_walker *w;
	int iter = 0;

	/* Set fn->leaf to null_entry for root node. */
	if (fn->fn_flags & RTN_TL_ROOT) {
		rcu_assign_pointer(fn->leaf, net->ipv6.fib6_null_entry);
		return fn;
	}

	for (;;) {
		struct fib6_node *fn_r = rcu_dereference_protected(fn->right,
					    lockdep_is_held(&table->tb6_lock));
		struct fib6_node *fn_l = rcu_dereference_protected(fn->left,
					    lockdep_is_held(&table->tb6_lock));
		struct fib6_node *pn = rcu_dereference_protected(fn->parent,
					    lockdep_is_held(&table->tb6_lock));
		struct fib6_node *pn_r = rcu_dereference_protected(pn->right,
					    lockdep_is_held(&table->tb6_lock));
		struct fib6_node *pn_l = rcu_dereference_protected(pn->left,
					    lockdep_is_held(&table->tb6_lock));
		struct fib6_info *fn_leaf = rcu_dereference_protected(fn->leaf,
					    lockdep_is_held(&table->tb6_lock));
		struct fib6_info *pn_leaf = rcu_dereference_protected(pn->leaf,
					    lockdep_is_held(&table->tb6_lock));
		struct fib6_info *new_fn_leaf;

		RT6_TRACE("fixing tree: plen=%d iter=%d\n", fn->fn_bit, iter);
		iter++;

		WARN_ON(fn->fn_flags & RTN_RTINFO);
		WARN_ON(fn->fn_flags & RTN_TL_ROOT);
		WARN_ON(fn_leaf);

		children = 0;
		child = NULL;
		if (fn_r)
			child = fn_r, children |= 1;
		if (fn_l)
			child = fn_l, children |= 2;

		if (children == 3 || FIB6_SUBTREE(fn)
#ifdef CONFIG_IPV6_SUBTREES
		    /* Subtree root (i.e. fn) may have one child */
		    || (children && fn->fn_flags & RTN_ROOT)
#endif
		    ) {
			new_fn_leaf = fib6_find_prefix(net, table, fn);
#if RT6_DEBUG >= 2
			if (!new_fn_leaf) {
				WARN_ON(!new_fn_leaf);
				new_fn_leaf = net->ipv6.fib6_null_entry;
			}
#endif
			fib6_info_hold(new_fn_leaf);
			rcu_assign_pointer(fn->leaf, new_fn_leaf);
			return pn;
		}

#ifdef CONFIG_IPV6_SUBTREES
		if (FIB6_SUBTREE(pn) == fn) {
			WARN_ON(!(fn->fn_flags & RTN_ROOT));
			RCU_INIT_POINTER(pn->subtree, NULL);
			nstate = FWS_L;
		} else {
			WARN_ON(fn->fn_flags & RTN_ROOT);
#endif
			if (pn_r == fn)
				rcu_assign_pointer(pn->right, child);
			else if (pn_l == fn)
				rcu_assign_pointer(pn->left, child);
#if RT6_DEBUG >= 2
			else
				WARN_ON(1);
#endif
			if (child)
				rcu_assign_pointer(child->parent, pn);
			nstate = FWS_R;
#ifdef CONFIG_IPV6_SUBTREES
		}
#endif

		read_lock(&net->ipv6.fib6_walker_lock);
		FOR_WALKERS(net, w) {
			if (!child) {
				if (w->node == fn) {
					RT6_TRACE("W %p adjusted by delnode 1, s=%d/%d\n", w, w->state, nstate);
					w->node = pn;
					w->state = nstate;
				}
			} else {
				if (w->node == fn) {
					w->node = child;
					if (children&2) {
						RT6_TRACE("W %p adjusted by delnode 2, s=%d\n", w, w->state);
						w->state = w->state >= FWS_R ? FWS_U : FWS_INIT;
					} else {
						RT6_TRACE("W %p adjusted by delnode 2, s=%d\n", w, w->state);
						w->state = w->state >= FWS_C ? FWS_U : FWS_INIT;
					}
				}
			}
		}
		read_unlock(&net->ipv6.fib6_walker_lock);

		node_free(net, fn);
		if (pn->fn_flags & RTN_RTINFO || FIB6_SUBTREE(pn))
			return pn;

		RCU_INIT_POINTER(pn->leaf, NULL);
		fib6_info_release(pn_leaf);
		fn = pn;
	}
}