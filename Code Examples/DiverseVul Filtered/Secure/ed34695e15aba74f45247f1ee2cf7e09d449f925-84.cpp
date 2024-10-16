static void _ff_layout_free_lseg(struct nfs4_ff_layout_segment *fls)
{
	if (fls) {
		ff_layout_free_mirror_array(fls);
		kfree(fls);
	}
}