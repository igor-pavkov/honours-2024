static void fib6_clean_tree(struct net *net, struct fib6_node *root,
			    int (*func)(struct rt6_info *, void *arg),
			    int prune, void *arg)
{
	struct fib6_cleaner_t c;

	c.w.root = root;
	c.w.func = fib6_clean_node;
	c.w.prune = prune;
	c.w.count = 0;
	c.w.skip = 0;
	c.func = func;
	c.arg = arg;
	c.net = net;

	fib6_walk(&c.w);
}