static int gadgetfs_init_fs_context(struct fs_context *fc)
{
	fc->ops = &gadgetfs_context_ops;
	return 0;
}