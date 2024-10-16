static int fib6_clean_node(struct fib6_walker *w)
{
	int res;
	struct fib6_info *rt;
	struct fib6_cleaner *c = container_of(w, struct fib6_cleaner, w);
	struct nl_info info = {
		.nl_net = c->net,
		.skip_notify = c->skip_notify,
	};

	if (c->sernum != FIB6_NO_SERNUM_CHANGE &&
	    w->node->fn_sernum != c->sernum)
		w->node->fn_sernum = c->sernum;

	if (!c->func) {
		WARN_ON_ONCE(c->sernum == FIB6_NO_SERNUM_CHANGE);
		w->leaf = NULL;
		return 0;
	}

	for_each_fib6_walker_rt(w) {
		res = c->func(rt, c->arg);
		if (res == -1) {
			w->leaf = rt;
			res = fib6_del(rt, &info);
			if (res) {
#if RT6_DEBUG >= 2
				pr_debug("%s: del failed: rt=%p@%p err=%d\n",
					 __func__, rt,
					 rcu_access_pointer(rt->fib6_node),
					 res);
#endif
				continue;
			}
			return 0;
		} else if (res == -2) {
			if (WARN_ON(!rt->fib6_nsiblings))
				continue;
			rt = list_last_entry(&rt->fib6_siblings,
					     struct fib6_info, fib6_siblings);
			continue;
		}
		WARN_ON(res != 0);
	}
	w->leaf = rt;
	return 0;
}