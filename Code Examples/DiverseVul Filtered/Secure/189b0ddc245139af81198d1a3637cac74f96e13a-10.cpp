static int pipefs_init_fs_context(struct fs_context *fc)
{
	struct pseudo_fs_context *ctx = init_pseudo(fc, PIPEFS_MAGIC);
	if (!ctx)
		return -ENOMEM;
	ctx->ops = &pipefs_ops;
	ctx->dops = &pipefs_dentry_operations;
	return 0;
}