static int fib6_walk_continue(struct fib6_walker *w)
{
	struct fib6_node *fn, *pn, *left, *right;

	/* w->root should always be table->tb6_root */
	WARN_ON_ONCE(!(w->root->fn_flags & RTN_TL_ROOT));

	for (;;) {
		fn = w->node;
		if (!fn)
			return 0;

		switch (w->state) {
#ifdef CONFIG_IPV6_SUBTREES
		case FWS_S:
			if (FIB6_SUBTREE(fn)) {
				w->node = FIB6_SUBTREE(fn);
				continue;
			}
			w->state = FWS_L;
#endif
			/* fall through */
		case FWS_L:
			left = rcu_dereference_protected(fn->left, 1);
			if (left) {
				w->node = left;
				w->state = FWS_INIT;
				continue;
			}
			w->state = FWS_R;
			/* fall through */
		case FWS_R:
			right = rcu_dereference_protected(fn->right, 1);
			if (right) {
				w->node = right;
				w->state = FWS_INIT;
				continue;
			}
			w->state = FWS_C;
			w->leaf = rcu_dereference_protected(fn->leaf, 1);
			/* fall through */
		case FWS_C:
			if (w->leaf && fn->fn_flags & RTN_RTINFO) {
				int err;

				if (w->skip) {
					w->skip--;
					goto skip;
				}

				err = w->func(w);
				if (err)
					return err;

				w->count++;
				continue;
			}
skip:
			w->state = FWS_U;
			/* fall through */
		case FWS_U:
			if (fn == w->root)
				return 0;
			pn = rcu_dereference_protected(fn->parent, 1);
			left = rcu_dereference_protected(pn->left, 1);
			right = rcu_dereference_protected(pn->right, 1);
			w->node = pn;
#ifdef CONFIG_IPV6_SUBTREES
			if (FIB6_SUBTREE(pn) == fn) {
				WARN_ON(!(fn->fn_flags & RTN_ROOT));
				w->state = FWS_L;
				continue;
			}
#endif
			if (left == fn) {
				w->state = FWS_R;
				continue;
			}
			if (right == fn) {
				w->state = FWS_C;
				w->leaf = rcu_dereference_protected(w->node->leaf, 1);
				continue;
			}
#if RT6_DEBUG >= 2
			WARN_ON(1);
#endif
		}
	}
}