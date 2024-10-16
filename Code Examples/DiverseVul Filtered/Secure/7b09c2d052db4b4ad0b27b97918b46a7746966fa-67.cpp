int call_fib6_entry_notifiers(struct net *net,
			      enum fib_event_type event_type,
			      struct fib6_info *rt,
			      struct netlink_ext_ack *extack)
{
	struct fib6_entry_notifier_info info = {
		.info.extack = extack,
		.rt = rt,
	};

	rt->fib6_table->fib_seq++;
	return call_fib6_notifiers(net, event_type, &info.info);
}