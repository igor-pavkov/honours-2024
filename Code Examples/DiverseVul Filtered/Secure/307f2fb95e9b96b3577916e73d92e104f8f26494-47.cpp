void fib6_run_gc(unsigned long expires, struct net *net)
{
	if (expires != ~0UL) {
		spin_lock_bh(&fib6_gc_lock);
		gc_args.timeout = expires ? (int)expires :
			net->ipv6.sysctl.ip6_rt_gc_interval;
	} else {
		if (!spin_trylock_bh(&fib6_gc_lock)) {
			mod_timer(&net->ipv6.ip6_fib_timer, jiffies + HZ);
			return;
		}
		gc_args.timeout = net->ipv6.sysctl.ip6_rt_gc_interval;
	}

	gc_args.more = icmp6_dst_gc();

	fib6_clean_all(net, fib6_age, 0, NULL);

	if (gc_args.more)
		mod_timer(&net->ipv6.ip6_fib_timer,
			  round_jiffies(jiffies
					+ net->ipv6.sysctl.ip6_rt_gc_interval));
	else
		del_timer(&net->ipv6.ip6_fib_timer);
	spin_unlock_bh(&fib6_gc_lock);
}