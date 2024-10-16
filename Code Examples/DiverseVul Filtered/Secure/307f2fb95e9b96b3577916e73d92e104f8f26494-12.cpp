struct fib6_node * fib6_locate(struct fib6_node *root,
			       const struct in6_addr *daddr, int dst_len,
			       const struct in6_addr *saddr, int src_len)
{
	struct fib6_node *fn;

	fn = fib6_locate_1(root, daddr, dst_len,
			   offsetof(struct rt6_info, rt6i_dst));

#ifdef CONFIG_IPV6_SUBTREES
	if (src_len) {
		WARN_ON(saddr == NULL);
		if (fn && fn->subtree)
			fn = fib6_locate_1(fn->subtree, saddr, src_len,
					   offsetof(struct rt6_info, rt6i_src));
	}
#endif

	if (fn && fn->fn_flags & RTN_RTINFO)
		return fn;

	return NULL;
}