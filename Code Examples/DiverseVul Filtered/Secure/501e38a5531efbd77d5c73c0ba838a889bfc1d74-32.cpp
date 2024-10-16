static int gadgetfs_get_tree(struct fs_context *fc)
{
	return get_tree_single(fc, gadgetfs_fill_super);
}