static int nfs4_lookup_revalidate(struct dentry *dentry, unsigned int flags)
{
	return __nfs_lookup_revalidate(dentry, flags,
			nfs4_do_lookup_revalidate);
}