static void nfs_dentry_handle_enoent(struct dentry *dentry)
{
	if (simple_positive(dentry))
		d_delete(dentry);
}