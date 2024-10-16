static int proc_tgid_base_readdir(struct file *file, struct dir_context *ctx)
{
	return proc_pident_readdir(file, ctx,
				   tgid_base_stuff, ARRAY_SIZE(tgid_base_stuff));
}