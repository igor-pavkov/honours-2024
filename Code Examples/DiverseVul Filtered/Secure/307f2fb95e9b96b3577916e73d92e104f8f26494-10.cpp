static struct rt6_info *fib6_find_prefix(struct net *net, struct fib6_node *fn)
{
	if (fn->fn_flags & RTN_ROOT)
		return net->ipv6.ip6_null_entry;

	while (fn) {
		if (fn->left)
			return fn->left->leaf;
		if (fn->right)
			return fn->right->leaf;

		fn = FIB6_SUBTREE(fn);
	}
	return NULL;
}