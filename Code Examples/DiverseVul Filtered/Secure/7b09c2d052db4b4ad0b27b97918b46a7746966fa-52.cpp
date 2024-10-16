void fib6_update_sernum_stub(struct net *net, struct fib6_info *f6i)
{
	spin_lock_bh(&f6i->fib6_table->tb6_lock);
	fib6_update_sernum_upto_root(net, f6i);
	spin_unlock_bh(&f6i->fib6_table->tb6_lock);
}