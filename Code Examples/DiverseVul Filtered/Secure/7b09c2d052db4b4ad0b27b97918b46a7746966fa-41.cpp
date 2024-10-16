static void fib6_walker_link(struct net *net, struct fib6_walker *w)
{
	write_lock_bh(&net->ipv6.fib6_walker_lock);
	list_add(&w->lh, &net->ipv6.fib6_walkers);
	write_unlock_bh(&net->ipv6.fib6_walker_lock);
}