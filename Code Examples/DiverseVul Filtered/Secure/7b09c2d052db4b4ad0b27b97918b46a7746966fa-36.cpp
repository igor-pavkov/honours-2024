static struct fib6_table *fib6_alloc_table(struct net *net, u32 id)
{
	struct fib6_table *table;

	table = kzalloc(sizeof(*table), GFP_ATOMIC);
	if (table) {
		table->tb6_id = id;
		rcu_assign_pointer(table->tb6_root.leaf,
				   net->ipv6.fib6_null_entry);
		table->tb6_root.fn_flags = RTN_ROOT | RTN_TL_ROOT | RTN_RTINFO;
		inet_peer_base_init(&table->tb6_peers);
	}

	return table;
}