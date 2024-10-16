static int inet6_dump_fib(struct sk_buff *skb, struct netlink_callback *cb)
{
	struct net *net = sock_net(skb->sk);
	unsigned int h, s_h;
	unsigned int e = 0, s_e;
	struct rt6_rtnl_dump_arg arg;
	struct fib6_walker_t *w;
	struct fib6_table *tb;
	struct hlist_head *head;
	int res = 0;

	s_h = cb->args[0];
	s_e = cb->args[1];

	w = (void *)cb->args[2];
	if (!w) {
		/* New dump:
		 *
		 * 1. hook callback destructor.
		 */
		cb->args[3] = (long)cb->done;
		cb->done = fib6_dump_done;

		/*
		 * 2. allocate and initialize walker.
		 */
		w = kzalloc(sizeof(*w), GFP_ATOMIC);
		if (!w)
			return -ENOMEM;
		w->func = fib6_dump_node;
		cb->args[2] = (long)w;
	}

	arg.skb = skb;
	arg.cb = cb;
	arg.net = net;
	w->args = &arg;

	rcu_read_lock();
	for (h = s_h; h < FIB6_TABLE_HASHSZ; h++, s_e = 0) {
		e = 0;
		head = &net->ipv6.fib_table_hash[h];
		hlist_for_each_entry_rcu(tb, head, tb6_hlist) {
			if (e < s_e)
				goto next;
			res = fib6_dump_table(tb, skb, cb);
			if (res != 0)
				goto out;
next:
			e++;
		}
	}
out:
	rcu_read_unlock();
	cb->args[1] = e;
	cb->args[0] = h;

	res = res < 0 ? res : skb->len;
	if (res <= 0)
		fib6_dump_end(cb);
	return res;
}