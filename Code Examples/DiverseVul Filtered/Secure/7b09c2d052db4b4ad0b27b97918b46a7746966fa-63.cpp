static int fib6_dump_node(struct fib6_walker *w)
{
	int res;
	struct fib6_info *rt;

	for_each_fib6_walker_rt(w) {
		res = rt6_dump_route(rt, w->args, w->skip_in_node);
		if (res >= 0) {
			/* Frame is full, suspend walking */
			w->leaf = rt;

			/* We'll restart from this node, so if some routes were
			 * already dumped, skip them next time.
			 */
			w->skip_in_node += res;

			return 1;
		}
		w->skip_in_node = 0;

		/* Multipath routes are dumped in one route with the
		 * RTA_MULTIPATH attribute. Jump 'rt' to point to the
		 * last sibling of this route (no need to dump the
		 * sibling routes again)
		 */
		if (rt->fib6_nsiblings)
			rt = list_last_entry(&rt->fib6_siblings,
					     struct fib6_info,
					     fib6_siblings);
	}
	w->leaf = NULL;
	return 0;
}