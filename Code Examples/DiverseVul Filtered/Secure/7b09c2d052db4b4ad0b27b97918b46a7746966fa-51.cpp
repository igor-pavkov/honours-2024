struct fib6_info *fib6_info_alloc(gfp_t gfp_flags, bool with_fib6_nh)
{
	struct fib6_info *f6i;
	size_t sz = sizeof(*f6i);

	if (with_fib6_nh)
		sz += sizeof(struct fib6_nh);

	f6i = kzalloc(sz, gfp_flags);
	if (!f6i)
		return NULL;

	/* fib6_siblings is a union with nh_list, so this initializes both */
	INIT_LIST_HEAD(&f6i->fib6_siblings);
	refcount_set(&f6i->fib6_ref, 1);

	return f6i;
}