static int fib6_prune_clone(struct rt6_info *rt, void *arg)
{
	if (rt->rt6i_flags & RTF_CACHE) {
		RT6_TRACE("pruning clone %p\n", rt);
		return -1;
	}

	return 0;
}