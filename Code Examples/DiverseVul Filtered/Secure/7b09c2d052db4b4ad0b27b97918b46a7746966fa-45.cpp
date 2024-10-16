static void *ipv6_route_seq_next(struct seq_file *seq, void *v, loff_t *pos)
{
	int r;
	struct fib6_info *n;
	struct net *net = seq_file_net(seq);
	struct ipv6_route_iter *iter = seq->private;

	if (!v)
		goto iter_table;

	n = rcu_dereference_bh(((struct fib6_info *)v)->fib6_next);
	if (n) {
		++*pos;
		return n;
	}

iter_table:
	ipv6_route_check_sernum(iter);
	spin_lock_bh(&iter->tbl->tb6_lock);
	r = fib6_walk_continue(&iter->w);
	spin_unlock_bh(&iter->tbl->tb6_lock);
	if (r > 0) {
		if (v)
			++*pos;
		return iter->w.leaf;
	} else if (r < 0) {
		fib6_walker_unlink(net, &iter->w);
		return NULL;
	}
	fib6_walker_unlink(net, &iter->w);

	iter->tbl = ipv6_route_seq_next_table(iter->tbl, net);
	if (!iter->tbl)
		return NULL;

	ipv6_route_seq_setup_walk(iter, net);
	goto iter_table;
}