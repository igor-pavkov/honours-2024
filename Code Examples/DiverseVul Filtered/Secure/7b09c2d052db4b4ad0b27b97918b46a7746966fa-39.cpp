void fib6_update_sernum(struct net *net, struct fib6_info *f6i)
{
	struct fib6_node *fn;

	fn = rcu_dereference_protected(f6i->fib6_node,
			lockdep_is_held(&f6i->fib6_table->tb6_lock));
	if (fn)
		fn->fn_sernum = fib6_new_sernum(net);
}