static int fib6_dump_table(struct fib6_table *table, struct sk_buff *skb,
			   struct netlink_callback *cb)
{
	struct net *net = sock_net(skb->sk);
	struct fib6_walker *w;
	int res;

	w = (void *)cb->args[2];
	w->root = &table->tb6_root;

	if (cb->args[4] == 0) {
		w->count = 0;
		w->skip = 0;
		w->skip_in_node = 0;

		spin_lock_bh(&table->tb6_lock);
		res = fib6_walk(net, w);
		spin_unlock_bh(&table->tb6_lock);
		if (res > 0) {
			cb->args[4] = 1;
			cb->args[5] = w->root->fn_sernum;
		}
	} else {
		if (cb->args[5] != w->root->fn_sernum) {
			/* Begin at the root if the tree changed */
			cb->args[5] = w->root->fn_sernum;
			w->state = FWS_INIT;
			w->node = w->root;
			w->skip = w->count;
			w->skip_in_node = 0;
		} else
			w->skip = 0;

		spin_lock_bh(&table->tb6_lock);
		res = fib6_walk_continue(w);
		spin_unlock_bh(&table->tb6_lock);
		if (res <= 0) {
			fib6_walker_unlink(net, w);
			cb->args[4] = 0;
		}
	}

	return res;
}