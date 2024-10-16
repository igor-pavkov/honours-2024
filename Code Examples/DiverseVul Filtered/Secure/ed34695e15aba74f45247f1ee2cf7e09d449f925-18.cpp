static void ff_layout_put_mirror(struct nfs4_ff_layout_mirror *mirror)
{
	if (mirror != NULL && refcount_dec_and_test(&mirror->ref))
		ff_layout_free_mirror(mirror);
}