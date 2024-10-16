static void ff_layout_free_mirror_array(struct nfs4_ff_layout_segment *fls)
{
	u32 i;

	for (i = 0; i < fls->mirror_array_cnt; i++)
		ff_layout_put_mirror(fls->mirror_array[i]);
}