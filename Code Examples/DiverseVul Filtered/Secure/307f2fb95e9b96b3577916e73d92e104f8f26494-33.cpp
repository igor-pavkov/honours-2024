static int fib6_walk(struct fib6_walker_t *w)
{
	int res;

	w->state = FWS_INIT;
	w->node = w->root;

	fib6_walker_link(w);
	res = fib6_walk_continue(w);
	if (res <= 0)
		fib6_walker_unlink(w);
	return res;
}