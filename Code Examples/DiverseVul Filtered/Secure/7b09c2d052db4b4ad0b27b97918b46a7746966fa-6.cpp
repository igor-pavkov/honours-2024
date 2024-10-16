static void fib6_clean_tree(struct net *net, struct fib6_node *root,
			    int (*func)(struct fib6_info *, void *arg),
			    int sernum, void *arg, bool skip_notify)
{
	struct fib6_cleaner c;

	c.w.root = root;
	c.w.func = fib6_clean_node;
	c.w.count = 0;
	c.w.skip = 0;
	c.w.skip_in_node = 0;
	c.func = func;
	c.sernum = sernum;
	c.arg = arg;
	c.net = net;
	c.skip_notify = skip_notify;

	fib6_walk(net, &c.w);
}