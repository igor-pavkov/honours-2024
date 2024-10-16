static inline void fib6_walker_unlink(struct fib6_walker_t *w)
{
	write_lock_bh(&fib6_walker_lock);
	list_del(&w->lh);
	write_unlock_bh(&fib6_walker_lock);
}