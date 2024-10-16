static int fib6_age(struct fib6_info *rt, void *arg)
{
	struct fib6_gc_args *gc_args = arg;
	unsigned long now = jiffies;

	/*
	 *	check addrconf expiration here.
	 *	Routes are expired even if they are in use.
	 */

	if (rt->fib6_flags & RTF_EXPIRES && rt->expires) {
		if (time_after(now, rt->expires)) {
			RT6_TRACE("expiring %p\n", rt);
			return -1;
		}
		gc_args->more++;
	}

	/*	Also age clones in the exception table.
	 *	Note, that clones are aged out
	 *	only if they are not in use now.
	 */
	rt6_age_exceptions(rt, gc_args, now);

	return 0;
}