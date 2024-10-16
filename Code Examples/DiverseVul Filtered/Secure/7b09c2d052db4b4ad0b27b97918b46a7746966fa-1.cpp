static int fib6_nh_drop_pcpu_from(struct fib6_nh *nh, void *_arg)
{
	struct fib6_nh_pcpu_arg *arg = _arg;

	__fib6_drop_pcpu_from(nh, arg->from, arg->table);
	return 0;
}