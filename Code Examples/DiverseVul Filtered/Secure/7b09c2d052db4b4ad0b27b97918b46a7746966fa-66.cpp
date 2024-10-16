int fib6_tables_dump(struct net *net, struct notifier_block *nb)
{
	struct fib6_dump_arg arg;
	struct fib6_walker *w;
	unsigned int h;

	w = kzalloc(sizeof(*w), GFP_ATOMIC);
	if (!w)
		return -ENOMEM;

	w->func = fib6_node_dump;
	arg.net = net;
	arg.nb = nb;
	w->args = &arg;

	for (h = 0; h < FIB6_TABLE_HASHSZ; h++) {
		struct hlist_head *head = &net->ipv6.fib_table_hash[h];
		struct fib6_table *tb;

		hlist_for_each_entry_rcu(tb, head, tb6_hlist)
			fib6_table_dump(net, tb, w);
	}

	kfree(w);

	return 0;
}