static int nfs_lookup_revalidate(struct dentry *dentry, unsigned int flags)
{
	return __nfs_lookup_revalidate(dentry, flags, nfs_do_lookup_revalidate);
}