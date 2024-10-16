static struct fib6_node *fib6_repair_tree(struct net *net,
					   struct fib6_node *fn)
{
	int children;
	int nstate;
	struct fib6_node *child, *pn;
	struct fib6_walker_t *w;
	int iter = 0;

	for (;;) {
		RT6_TRACE("fixing tree: plen=%d iter=%d\n", fn->fn_bit, iter);
		iter++;

		WARN_ON(fn->fn_flags & RTN_RTINFO);
		WARN_ON(fn->fn_flags & RTN_TL_ROOT);
		WARN_ON(fn->leaf != NULL);

		children = 0;
		child = NULL;
		if (fn->right) child = fn->right, children |= 1;
		if (fn->left) child = fn->left, children |= 2;

		if (children == 3 || FIB6_SUBTREE(fn)
#ifdef CONFIG_IPV6_SUBTREES
		    /* Subtree root (i.e. fn) may have one child */
		    || (children && fn->fn_flags & RTN_ROOT)
#endif
		    ) {
			fn->leaf = fib6_find_prefix(net, fn);
#if RT6_DEBUG >= 2
			if (!fn->leaf) {
				WARN_ON(!fn->leaf);
				fn->leaf = net->ipv6.ip6_null_entry;
			}
#endif
			atomic_inc(&fn->leaf->rt6i_ref);
			return fn->parent;
		}

		pn = fn->parent;
#ifdef CONFIG_IPV6_SUBTREES
		if (FIB6_SUBTREE(pn) == fn) {
			WARN_ON(!(fn->fn_flags & RTN_ROOT));
			FIB6_SUBTREE(pn) = NULL;
			nstate = FWS_L;
		} else {
			WARN_ON(fn->fn_flags & RTN_ROOT);
#endif
			if (pn->right == fn) pn->right = child;
			else if (pn->left == fn) pn->left = child;
#if RT6_DEBUG >= 2
			else
				WARN_ON(1);
#endif
			if (child)
				child->parent = pn;
			nstate = FWS_R;
#ifdef CONFIG_IPV6_SUBTREES
		}
#endif

		read_lock(&fib6_walker_lock);
		FOR_WALKERS(w) {
			if (!child) {
				if (w->root == fn) {
					w->root = w->node = NULL;
					RT6_TRACE("W %p adjusted by delroot 1\n", w);
				} else if (w->node == fn) {
					RT6_TRACE("W %p adjusted by delnode 1, s=%d/%d\n", w, w->state, nstate);
					w->node = pn;
					w->state = nstate;
				}
			} else {
				if (w->root == fn) {
					w->root = child;
					RT6_TRACE("W %p adjusted by delroot 2\n", w);
				}
				if (w->node == fn) {
					w->node = child;
					if (children&2) {
						RT6_TRACE("W %p adjusted by delnode 2, s=%d\n", w, w->state);
						w->state = w->state>=FWS_R ? FWS_U : FWS_INIT;
					} else {
						RT6_TRACE("W %p adjusted by delnode 2, s=%d\n", w, w->state);
						w->state = w->state>=FWS_C ? FWS_U : FWS_INIT;
					}
				}
			}
		}
		read_unlock(&fib6_walker_lock);

		node_free(fn);
		if (pn->fn_flags & RTN_RTINFO || FIB6_SUBTREE(pn))
			return pn;

		rt6_release(pn->leaf);
		pn->leaf = NULL;
		fn = pn;
	}
}