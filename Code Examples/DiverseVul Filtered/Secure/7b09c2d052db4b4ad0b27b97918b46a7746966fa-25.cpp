static void __fib6_clean_all(struct net *net,
			     int (*func)(struct fib6_info *, void *),
			     int sernum, void *arg, bool skip_notify)
{
	struct fib6_table *table;
	struct hlist_head *head;
	unsigned int h;

	rcu_read_lock();
	for (h = 0; h < FIB6_TABLE_HASHSZ; h++) {
		head = &net->ipv6.fib_table_hash[h];
		hlist_for_each_entry_rcu(table, head, tb6_hlist) {
			spin_lock_bh(&table->tb6_lock);
			fib6_clean_tree(net, &table->tb6_root,
					func, sernum, arg, skip_notify);
			spin_unlock_bh(&table->tb6_lock);
		}
	}
	rcu_read_unlock();
}