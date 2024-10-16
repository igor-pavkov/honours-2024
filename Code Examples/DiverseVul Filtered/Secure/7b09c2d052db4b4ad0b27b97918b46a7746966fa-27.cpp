static void fib6_table_dump(struct net *net, struct fib6_table *tb,
			    struct fib6_walker *w)
{
	w->root = &tb->tb6_root;
	spin_lock_bh(&tb->tb6_lock);
	fib6_walk(net, w);
	spin_unlock_bh(&tb->tb6_lock);
}