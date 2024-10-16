static int call_fib6_entry_notifier(struct notifier_block *nb, struct net *net,
				    enum fib_event_type event_type,
				    struct fib6_info *rt)
{
	struct fib6_entry_notifier_info info = {
		.rt = rt,
	};

	return call_fib6_notifier(nb, net, event_type, &info.info);
}