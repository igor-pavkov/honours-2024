static struct fib6_node * fib6_lookup_1(struct fib6_node *root,
					struct lookup_args *args)
{
	struct fib6_node *fn;
	__be32 dir;

	if (unlikely(args->offset == 0))
		return NULL;

	/*
	 *	Descend on a tree
	 */

	fn = root;

	for (;;) {
		struct fib6_node *next;

		dir = addr_bit_set(args->addr, fn->fn_bit);

		next = dir ? fn->right : fn->left;

		if (next) {
			fn = next;
			continue;
		}
		break;
	}

	while (fn) {
		if (FIB6_SUBTREE(fn) || fn->fn_flags & RTN_RTINFO) {
			struct rt6key *key;

			key = (struct rt6key *) ((u8 *) fn->leaf +
						 args->offset);

			if (ipv6_prefix_equal(&key->addr, args->addr, key->plen)) {
#ifdef CONFIG_IPV6_SUBTREES
				if (fn->subtree)
					fn = fib6_lookup_1(fn->subtree, args + 1);
#endif
				if (!fn || fn->fn_flags & RTN_RTINFO)
					return fn;
			}
		}

		if (fn->fn_flags & RTN_ROOT)
			break;

		fn = fn->parent;
	}

	return NULL;
}