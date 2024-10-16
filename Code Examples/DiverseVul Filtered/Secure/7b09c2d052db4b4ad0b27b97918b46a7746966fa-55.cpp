static void fib6_net_exit(struct net *net)
{
	unsigned int i;

	del_timer_sync(&net->ipv6.ip6_fib_timer);

	for (i = 0; i < FIB6_TABLE_HASHSZ; i++) {
		struct hlist_head *head = &net->ipv6.fib_table_hash[i];
		struct hlist_node *tmp;
		struct fib6_table *tb;

		hlist_for_each_entry_safe(tb, tmp, head, tb6_hlist) {
			hlist_del(&tb->tb6_hlist);
			fib6_free_table(tb);
		}
	}

	kfree(net->ipv6.fib_table_hash);
	kfree(net->ipv6.rt6_stats);
	fib6_notifier_exit(net);
}