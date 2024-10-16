unsigned int fib6_tables_seq_read(struct net *net)
{
	unsigned int h, fib_seq = 0;

	rcu_read_lock();
	for (h = 0; h < FIB6_TABLE_HASHSZ; h++) {
		struct hlist_head *head = &net->ipv6.fib_table_hash[h];
		struct fib6_table *tb;

		hlist_for_each_entry_rcu(tb, head, tb6_hlist)
			fib_seq += tb->fib_seq;
	}
	rcu_read_unlock();

	return fib_seq;
}