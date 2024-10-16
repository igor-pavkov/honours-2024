static struct fib6_node * fib6_locate_1(struct fib6_node *root,
					const struct in6_addr *addr,
					int plen, int offset)
{
	struct fib6_node *fn;

	for (fn = root; fn ; ) {
		struct rt6key *key = (struct rt6key *)((u8 *)fn->leaf + offset);

		/*
		 *	Prefix match
		 */
		if (plen < fn->fn_bit ||
		    !ipv6_prefix_equal(&key->addr, addr, fn->fn_bit))
			return NULL;

		if (plen == fn->fn_bit)
			return fn;

		/*
		 *	We have more bits to go
		 */
		if (addr_bit_set(addr, fn->fn_bit))
			fn = fn->right;
		else
			fn = fn->left;
	}
	return NULL;
}