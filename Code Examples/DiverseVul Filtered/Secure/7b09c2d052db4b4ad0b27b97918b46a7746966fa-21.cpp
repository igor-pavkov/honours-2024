struct fib6_node *fib6_locate(struct fib6_node *root,
			      const struct in6_addr *daddr, int dst_len,
			      const struct in6_addr *saddr, int src_len,
			      bool exact_match)
{
	struct fib6_node *fn;

	fn = fib6_locate_1(root, daddr, dst_len,
			   offsetof(struct fib6_info, fib6_dst),
			   exact_match);

#ifdef CONFIG_IPV6_SUBTREES
	if (src_len) {
		WARN_ON(saddr == NULL);
		if (fn) {
			struct fib6_node *subtree = FIB6_SUBTREE(fn);

			if (subtree) {
				fn = fib6_locate_1(subtree, saddr, src_len,
					   offsetof(struct fib6_info, fib6_src),
					   exact_match);
			}
		}
	}
#endif

	if (fn && fn->fn_flags & RTN_RTINFO)
		return fn;

	return NULL;
}