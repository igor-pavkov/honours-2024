static int __net_init fib6_net_init(struct net *net)
{
	size_t size = sizeof(struct hlist_head) * FIB6_TABLE_HASHSZ;

	setup_timer(&net->ipv6.ip6_fib_timer, fib6_gc_timer_cb, (unsigned long)net);

	net->ipv6.rt6_stats = kzalloc(sizeof(*net->ipv6.rt6_stats), GFP_KERNEL);
	if (!net->ipv6.rt6_stats)
		goto out_timer;

	/* Avoid false sharing : Use at least a full cache line */
	size = max_t(size_t, size, L1_CACHE_BYTES);

	net->ipv6.fib_table_hash = kzalloc(size, GFP_KERNEL);
	if (!net->ipv6.fib_table_hash)
		goto out_rt6_stats;

	net->ipv6.fib6_main_tbl = kzalloc(sizeof(*net->ipv6.fib6_main_tbl),
					  GFP_KERNEL);
	if (!net->ipv6.fib6_main_tbl)
		goto out_fib_table_hash;

	net->ipv6.fib6_main_tbl->tb6_id = RT6_TABLE_MAIN;
	net->ipv6.fib6_main_tbl->tb6_root.leaf = net->ipv6.ip6_null_entry;
	net->ipv6.fib6_main_tbl->tb6_root.fn_flags =
		RTN_ROOT | RTN_TL_ROOT | RTN_RTINFO;
	inet_peer_base_init(&net->ipv6.fib6_main_tbl->tb6_peers);

#ifdef CONFIG_IPV6_MULTIPLE_TABLES
	net->ipv6.fib6_local_tbl = kzalloc(sizeof(*net->ipv6.fib6_local_tbl),
					   GFP_KERNEL);
	if (!net->ipv6.fib6_local_tbl)
		goto out_fib6_main_tbl;
	net->ipv6.fib6_local_tbl->tb6_id = RT6_TABLE_LOCAL;
	net->ipv6.fib6_local_tbl->tb6_root.leaf = net->ipv6.ip6_null_entry;
	net->ipv6.fib6_local_tbl->tb6_root.fn_flags =
		RTN_ROOT | RTN_TL_ROOT | RTN_RTINFO;
	inet_peer_base_init(&net->ipv6.fib6_local_tbl->tb6_peers);
#endif
	fib6_tables_init(net);

	return 0;

#ifdef CONFIG_IPV6_MULTIPLE_TABLES
out_fib6_main_tbl:
	kfree(net->ipv6.fib6_main_tbl);
#endif
out_fib_table_hash:
	kfree(net->ipv6.fib_table_hash);
out_rt6_stats:
	kfree(net->ipv6.rt6_stats);
out_timer:
	return -ENOMEM;
 }