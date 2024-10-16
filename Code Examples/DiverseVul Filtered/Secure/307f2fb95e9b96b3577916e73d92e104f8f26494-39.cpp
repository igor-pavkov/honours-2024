static inline void fib6_walker_link(struct fib6_walker_t *w)
{
	write_lock_bh(&fib6_walker_lock);
	list_add(&w->lh, &fib6_walkers);
	write_unlock_bh(&fib6_walker_lock);
}