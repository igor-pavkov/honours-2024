static void fib6_rt_dump(struct fib6_info *rt, struct fib6_dump_arg *arg)
{
	if (rt == arg->net->ipv6.fib6_null_entry)
		return;
	call_fib6_entry_notifier(arg->nb, arg->net, FIB_EVENT_ENTRY_ADD, rt);
}