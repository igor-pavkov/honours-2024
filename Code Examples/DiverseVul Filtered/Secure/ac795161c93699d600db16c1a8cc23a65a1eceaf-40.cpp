static struct nfs_open_context *create_nfs_open_context(struct dentry *dentry, int open_flags, struct file *filp)
{
	return alloc_nfs_open_context(dentry, flags_to_mode(open_flags), filp);
}