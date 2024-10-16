static struct fib6_node *fib6_node_lookup_1(struct fib6_node *root,
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

		next = dir ? rcu_dereference(fn->right) :
			     rcu_dereference(fn->left);

		if (next) {
			fn = next;
			continue;
		}
		break;
	}

	while (fn) {
		struct fib6_node *subtree = FIB6_SUBTREE(fn);

		if (subtree || fn->fn_flags & RTN_RTINFO) {
			struct fib6_info *leaf = rcu_dereference(fn->leaf);
			struct rt6key *key;

			if (!leaf)
				goto backtrack;

			key = (struct rt6key *) ((u8 *)leaf + args->offset);

			if (ipv6_prefix_equal(&key->addr, args->addr, key->plen)) {
#ifdef CONFIG_IPV6_SUBTREES
				if (subtree) {
					struct fib6_node *sfn;
					sfn = fib6_node_lookup_1(subtree,
								 args + 1);
					if (!sfn)
						goto backtrack;
					fn = sfn;
				}
#endif
				if (fn->fn_flags & RTN_RTINFO)
					return fn;
			}
		}
backtrack:
		if (fn->fn_flags & RTN_ROOT)
			break;

		fn = rcu_dereference(fn->parent);
	}

	return NULL;
}