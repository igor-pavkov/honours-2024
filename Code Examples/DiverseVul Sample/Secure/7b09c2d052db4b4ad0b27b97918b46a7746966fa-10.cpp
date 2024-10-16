static void fib6_flush_trees(struct net *net)
{
	int new_sernum = fib6_new_sernum(net);

	__fib6_clean_all(net, NULL, new_sernum, NULL, false);
}