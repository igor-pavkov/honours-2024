static int fib6_walk(struct net *net, struct fib6_walker *w)
{
	int res;

	w->state = FWS_INIT;
	w->node = w->root;

	fib6_walker_link(net, w);
	res = fib6_walk_continue(w);
	if (res <= 0)
		fib6_walker_unlink(net, w);
	return res;
}