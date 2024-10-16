int fib6_del(struct rt6_info *rt, struct nl_info *info)
{
	struct net *net = info->nl_net;
	struct fib6_node *fn = rt->rt6i_node;
	struct rt6_info **rtp;

#if RT6_DEBUG >= 2
	if (rt->dst.obsolete>0) {
		WARN_ON(fn != NULL);
		return -ENOENT;
	}
#endif
	if (!fn || rt == net->ipv6.ip6_null_entry)
		return -ENOENT;

	WARN_ON(!(fn->fn_flags & RTN_RTINFO));

	if (!(rt->rt6i_flags & RTF_CACHE)) {
		struct fib6_node *pn = fn;
#ifdef CONFIG_IPV6_SUBTREES
		/* clones of this route might be in another subtree */
		if (rt->rt6i_src.plen) {
			while (!(pn->fn_flags & RTN_ROOT))
				pn = pn->parent;
			pn = pn->parent;
		}
#endif
		fib6_prune_clones(info->nl_net, pn, rt);
	}

	/*
	 *	Walk the leaf entries looking for ourself
	 */

	for (rtp = &fn->leaf; *rtp; rtp = &(*rtp)->dst.rt6_next) {
		if (*rtp == rt) {
			fib6_del_route(fn, rtp, info);
			return 0;
		}
	}
	return -ENOENT;
}