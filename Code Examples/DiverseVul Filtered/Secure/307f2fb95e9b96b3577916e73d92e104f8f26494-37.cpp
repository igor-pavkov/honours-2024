static __inline__ void fib6_start_gc(struct net *net, struct rt6_info *rt)
{
	if (!timer_pending(&net->ipv6.ip6_fib_timer) &&
	    (rt->rt6i_flags & (RTF_EXPIRES | RTF_CACHE)))
		mod_timer(&net->ipv6.ip6_fib_timer,
			  jiffies + net->ipv6.sysctl.ip6_rt_gc_interval);
}