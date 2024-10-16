static int fib6_node_dump(struct fib6_walker *w)
{
	struct fib6_info *rt;

	for_each_fib6_walker_rt(w)
		fib6_rt_dump(rt, w->args);
	w->leaf = NULL;
	return 0;
}