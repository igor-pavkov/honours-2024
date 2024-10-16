static void __fib6_update_sernum_upto_root(struct fib6_info *rt,
					   int sernum)
{
	struct fib6_node *fn = rcu_dereference_protected(rt->fib6_node,
				lockdep_is_held(&rt->fib6_table->tb6_lock));

	/* paired with smp_rmb() in rt6_get_cookie_safe() */
	smp_wmb();
	while (fn) {
		fn->fn_sernum = sernum;
		fn = rcu_dereference_protected(fn->parent,
				lockdep_is_held(&rt->fib6_table->tb6_lock));
	}
}