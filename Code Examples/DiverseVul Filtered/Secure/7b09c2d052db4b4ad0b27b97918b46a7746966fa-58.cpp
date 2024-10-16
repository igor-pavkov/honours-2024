static void fib6_gc_timer_cb(struct timer_list *t)
{
	struct net *arg = from_timer(arg, t, ipv6.ip6_fib_timer);

	fib6_run_gc(0, arg, true);
}