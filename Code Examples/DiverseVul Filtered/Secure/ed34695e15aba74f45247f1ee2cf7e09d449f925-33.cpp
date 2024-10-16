static struct nfs4_ff_layout_mirror *ff_layout_alloc_mirror(gfp_t gfp_flags)
{
	struct nfs4_ff_layout_mirror *mirror;

	mirror = kzalloc(sizeof(*mirror), gfp_flags);
	if (mirror != NULL) {
		spin_lock_init(&mirror->lock);
		refcount_set(&mirror->ref, 1);
		INIT_LIST_HEAD(&mirror->mirrors);
	}
	return mirror;
}