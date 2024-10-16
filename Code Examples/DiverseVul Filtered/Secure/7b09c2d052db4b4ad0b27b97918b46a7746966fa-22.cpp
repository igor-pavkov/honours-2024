static int inet6_dump_fib(struct sk_buff *skb, struct netlink_callback *cb)
{
	struct rt6_rtnl_dump_arg arg = { .filter.dump_exceptions = true,
					 .filter.dump_routes = true };
	const struct nlmsghdr *nlh = cb->nlh;
	struct net *net = sock_net(skb->sk);
	unsigned int h, s_h;
	unsigned int e = 0, s_e;
	struct fib6_walker *w;
	struct fib6_table *tb;
	struct hlist_head *head;
	int res = 0;

	if (cb->strict_check) {
		int err;

		err = ip_valid_fib_dump_req(net, nlh, &arg.filter, cb);
		if (err < 0)
			return err;
	} else if (nlmsg_len(nlh) >= sizeof(struct rtmsg)) {
		struct rtmsg *rtm = nlmsg_data(nlh);

		if (rtm->rtm_flags & RTM_F_PREFIX)
			arg.filter.flags = RTM_F_PREFIX;
	}

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

	if (arg.filter.table_id) {
		tb = fib6_get_table(net, arg.filter.table_id);
		if (!tb) {
			if (arg.filter.dump_all_families)
				goto out;

			NL_SET_ERR_MSG_MOD(cb->extack, "FIB table does not exist");
			return -ENOENT;
		}

		if (!cb->args[0]) {
			res = fib6_dump_table(tb, skb, cb);
			if (!res)
				cb->args[0] = 1;
		}
		goto out;
	}

	s_h = cb->args[0];
	s_e = cb->args[1];

	rcu_read_lock();
	for (h = s_h; h < FIB6_TABLE_HASHSZ; h++, s_e = 0) {
		e = 0;
		head = &net->ipv6.fib_table_hash[h];
		hlist_for_each_entry_rcu(tb, head, tb6_hlist) {
			if (e < s_e)
				goto next;
			res = fib6_dump_table(tb, skb, cb);
			if (res != 0)
				goto out_unlock;
next:
			e++;
		}
	}
out_unlock:
	rcu_read_unlock();
	cb->args[1] = e;
	cb->args[0] = h;
out:
	res = res < 0 ? res : skb->len;
	if (res <= 0)
		fib6_dump_end(cb);
	return res;
}