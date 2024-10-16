static void fib6_net_exit(struct net *net)
{
	rt6_ifdown(net, NULL);
	del_timer_sync(&net->ipv6.ip6_fib_timer);

#ifdef CONFIG_IPV6_MULTIPLE_TABLES
	inetpeer_invalidate_tree(&net->ipv6.fib6_local_tbl->tb6_peers);
	kfree(net->ipv6.fib6_local_tbl);
#endif
	inetpeer_invalidate_tree(&net->ipv6.fib6_main_tbl->tb6_peers);
	kfree(net->ipv6.fib6_main_tbl);
	kfree(net->ipv6.fib_table_hash);
	kfree(net->ipv6.rt6_stats);
}