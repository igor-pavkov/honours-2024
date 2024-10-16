static int fib6_dump_node(struct fib6_walker_t *w)
{
	int res;
	struct rt6_info *rt;

	for (rt = w->leaf; rt; rt = rt->dst.rt6_next) {
		res = rt6_dump_route(rt, w->args);
		if (res < 0) {
			/* Frame is full, suspend walking */
			w->leaf = rt;
			return 1;
		}
		WARN_ON(res == 0);
	}
	w->leaf = NULL;
	return 0;
}