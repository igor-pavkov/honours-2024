static int check_loop(struct Qdisc *q, struct Qdisc *p, int depth)
{
	struct check_loop_arg	arg;

	if (q->ops->cl_ops == NULL)
		return 0;

	arg.w.stop = arg.w.skip = arg.w.count = 0;
	arg.w.fn = check_loop_fn;
	arg.depth = depth;
	arg.p = p;
	q->ops->cl_ops->walk(q, &arg.w);
	return arg.w.stop ? -ELOOP : 0;
}