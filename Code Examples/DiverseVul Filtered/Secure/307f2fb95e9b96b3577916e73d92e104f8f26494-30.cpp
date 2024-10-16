void fib6_clean_all_ro(struct net *net, int (*func)(struct rt6_info *, void *arg),
		    int prune, void *arg)
{
	struct fib6_table *table;
	struct hlist_head *head;
	unsigned int h;

	rcu_read_lock();
	for (h = 0; h < FIB6_TABLE_HASHSZ; h++) {
		head = &net->ipv6.fib_table_hash[h];
		hlist_for_each_entry_rcu(table, head, tb6_hlist) {
			read_lock_bh(&table->tb6_lock);
			fib6_clean_tree(net, &table->tb6_root,
					func, prune, arg);
			read_unlock_bh(&table->tb6_lock);
		}
	}
	rcu_read_unlock();
}