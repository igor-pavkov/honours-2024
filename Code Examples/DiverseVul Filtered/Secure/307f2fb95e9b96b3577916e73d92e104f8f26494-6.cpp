static int fib6_clean_node(struct fib6_walker_t *w)
{
	int res;
	struct rt6_info *rt;
	struct fib6_cleaner_t *c = container_of(w, struct fib6_cleaner_t, w);
	struct nl_info info = {
		.nl_net = c->net,
	};

	for (rt = w->leaf; rt; rt = rt->dst.rt6_next) {
		res = c->func(rt, c->arg);
		if (res < 0) {
			w->leaf = rt;
			res = fib6_del(rt, &info);
			if (res) {
#if RT6_DEBUG >= 2
				pr_debug("%s: del failed: rt=%p@%p err=%d\n",
					 __func__, rt, rt->rt6i_node, res);
#endif
				continue;
			}
			return 0;
		}
		WARN_ON(res != 0);
	}
	w->leaf = rt;
	return 0;
}